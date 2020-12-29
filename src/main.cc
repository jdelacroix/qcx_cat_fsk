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

#define BOOST_LOG_DYN_LINK 1

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <memory>

#include "qcx_cat_fsk/audio_decoder.h"
#include "qcx_cat_fsk/cat_interface.h"

int main(int argc, char *argv[]) {
  boost::log::add_file_log(boost::log::keywords::file_name = "qcx_cat_ft8.log",
                           boost::log::keywords::auto_flush = true);
  boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                      boost::log::trivial::info);

  BOOST_LOG_TRIVIAL(info) << "!log initialized";

  // FIXME: hard coded serial port device
  auto qcx = std::make_shared<qcx_cat_fsk::CatInterface>("/dev/ttyACM1");

  try {
    auto reply = qcx->SendCommand("FT2;");
    BOOST_LOG_TRIVIAL(info) << "FT2; " << reply;
  } catch (...) {
    // no reply from QCX on this command
  }

  // FIXME: hard coded FT8 frequency
  try {
    auto reply = qcx->SendCommand("FA00007074700;");
    BOOST_LOG_TRIVIAL(info) << "FA00007074700; " << reply;
  } catch (...) {
    // no reply from QCX on this command
  }

  // FIXME: hard coded FT8 frequency
  qcx_cat_fsk::AudioDecoder ft8_decoder(qcx, 7074000);

  ft8_decoder.Decode();
}