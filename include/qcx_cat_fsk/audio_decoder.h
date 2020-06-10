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

#pragma once

#include <pulse/error.h>
#include <pulse/simple.h>

#include <memory>
#include <mutex>

#include "qcx_cat_fsk/cat_interface.h"

namespace qcx_cat_fsk {

class AudioDecoder {
 public:
  AudioDecoder(const std::shared_ptr<CatInterface>& cat_interface,
               const uint32_t& vfo_b_hz);
  ~AudioDecoder();

  void Decode();

 private:
  pa_simple* audio_stream_;

  uint32_t vfo_b_hz_;
  std::mutex vfo_b_mutex_;

  std::shared_ptr<CatInterface> cat_interface_;

  bool is_tx_on_;
};
}  // namespace qcx_cat_fsk