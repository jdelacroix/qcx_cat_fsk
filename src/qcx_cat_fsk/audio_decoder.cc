/*
 * This file is part of qcx_cat_fsk.
 *
 * qcx_cat_fsk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qcx_cat_fsk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qcx_cat_fsk.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "qcx_cat_fsk/audio_decoder.h"

#include <boost/log/trivial.hpp>
#include <deque>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace qcx_cat_fsk {

AudioDecoder::AudioDecoder(const std::shared_ptr<CatInterface> &cat_interface,
                           const uint32_t &vfo_b_hz)
    : vfo_b_hz_(vfo_b_hz), cat_interface_(cat_interface), is_tx_on_(false) {
  uint32_t audio_sample_rate_hz = 48000;

  static const pa_sample_spec ss = {
      .format = PA_SAMPLE_S16LE, .rate = audio_sample_rate_hz, .channels = 1};

  /* Create the recording stream */
  int error;
  if (!(this->audio_stream_ =
            pa_simple_new(NULL, "qcx_cat_fsk", PA_STREAM_RECORD, NULL, "record",
                          &ss, NULL, NULL, &error))) {
    throw std::runtime_error(pa_strerror(error));
  }
}

void AudioDecoder::Decode() {
  double last_crossing = -1.0, last_frequency = 0;
  std::size_t zero_count = 0,
              zero_count_limit = 48000 / 1000 * 3;  // samples/msec*N

  size_t sample_count = 0, symbol_count = 0;
  std::deque<double> frequencies;

  int16_t last_sample = 0;

  const size_t filter_size = 6;

  int error;

  for (;;) {
    std::vector<int16_t> audio_chunk(512, 0);
    /* Record some data ... */
    if (pa_simple_read(this->audio_stream_, audio_chunk.data(),
                       sizeof(int16_t) * audio_chunk.size(), &error) < 0) {
      throw std::runtime_error(pa_strerror(error));
    }
    /* And check for tones. */

    last_crossing = -1.0;

    audio_chunk.insert(audio_chunk.begin(), last_sample);

    for (std::size_t i = 1; i < audio_chunk.size(); ++i) {
      const int16_t sample_a = audio_chunk[i - 1], sample_b = audio_chunk[i];

      // BOOST_LOG_TRIVIAL(info) << "(" << sample_a << ", " << sample_b << ")";

      if (sample_b == 0 && zero_count < zero_count_limit) {  // silence
        ++zero_count;
        if (zero_count == zero_count_limit) {
          try {
            auto reply =
                this->cat_interface_->SendCommand("RX;", false);  // TQ0;
            this->cat_interface_->SendCommand("FA00007074700;", false);
            BOOST_LOG_TRIVIAL(info) << "RX; " << reply;
            is_tx_on_ = false;
          } catch (...) {
            // QCX does not reply to this command
          }
          last_frequency = 0;
          frequencies.clear();
          continue;
        }
      }

      if (sample_a < 0 && sample_b >= 0) {  // forward 0-crossing
        zero_count = 0;

        double crossing =
            static_cast<double>(-sample_a) / (sample_b - sample_a) + i;
        if (last_crossing > 0) {
          if (frequencies.size() == filter_size) {
            frequencies.pop_front();
          }
          frequencies.push_back(static_cast<double>(48000) /
                                (crossing - last_crossing));
        }
        last_crossing = crossing;
      }

      if (frequencies.size() == filter_size) {
        double frequency =
            std::accumulate(frequencies.begin(), frequencies.end(), 0.0) /
            frequencies.size();
        if (std::fabs(frequency - last_frequency) > 1) {  // 1Hz
          // frequencies = {frequency};
          frequencies.clear();

          BOOST_LOG_TRIVIAL(info) << "Tone: " << frequency;

          std::stringstream ss;
          ss << "FB" << std::setfill('0') << std::setw(11)
             << (this->vfo_b_hz_ + static_cast<uint32_t>(std::round(frequency)))
             << ";";

          auto cat_command = ss.str();

          try {
            auto reply = this->cat_interface_->SendCommand(cat_command, false);
            BOOST_LOG_TRIVIAL(info) << cat_command << " " << reply;
          } catch (...) {
            // QCX does not reply to this command
          }

          if (is_tx_on_ == false) {
            try {
              auto reply = this->cat_interface_->SendCommand("TQ1;", false);
              BOOST_LOG_TRIVIAL(info) << "TQ1; " << reply;
              is_tx_on_ = true;
            } catch (...) {
              // QCX does not reply to this command
            }
          }

          last_frequency = frequency;
        }
      }
    }

    last_sample = audio_chunk.back();
  }
}

AudioDecoder::~AudioDecoder() {
  if (this->audio_stream_) {
    pa_simple_free(this->audio_stream_);
  }
}

}  // namespace qcx_cat_fsk