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
#include "qcx_cat_fsk/cat_interface.h"
#include "qcx_cat_fsk/version.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>

#include <memory>
#include <iomanip>
#include <iostream>
#include <sstream>

int main(int argc, char *argv[]) {

  boost::program_options::positional_options_description arguments;
  arguments.add("device", 1).add("frequency", 1);

  boost::program_options::options_description hidden_options;
  hidden_options.add_options()
    ("device,D", boost::program_options::value<std::string>()->value_name("<name>"), "set device name for connecting to QCX's CAT interface (e.g., /dev/ttyACM0")
    ("frequency,F", boost::program_options::value<double>()->value_name("<frequency>"), "transmit on and decode from this frequency in MHz (e.g., 7.074)");

  boost::program_options::options_description visible_options;
  visible_options.add_options()
    ("help,h", "show help")
    ("version,v", "show version")
    ("offset,O", boost::program_options::value<uint32_t>()->value_name("<frequency>")->default_value(700), "offset in CW frequency in Hz, by default set to 700 Hz on the QCX")
    ("log,l", boost::program_options::value<std::string>()->value_name("<logfile>"), "write log to a specific file, otherwise by default the log will be written to qcx_cat_ft8.log");

  boost::program_options::options_description all_options;
  all_options.add(hidden_options).add(visible_options);

  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all_options).positional(arguments).run(), vm);
    boost::program_options::notify(vm);
  } catch(const boost::program_options::error &e) {
    std::cout << "qcx_cat_ft8: " << e.what() << std::endl;
    std::cout << "See 'qcx_cat_ft8 --help'." << std::endl;
    return EXIT_FAILURE;
  }
  
  if (vm.count("help")) {
    std::cout << "usage: qcx_cat_ft8 [--version] [--help] [-l <logfile>]" << std::endl;
    std::cout << "                   <device name> <frequency>" << std::endl;
    std::cout << std::endl;
    std::cout << "Controls a QRP Labs QCX CW radio via CAT to transmit a FSK signal decoded from" << std::endl;
    std::cout << "WSTJ-X output audio. Specify a device name (e.g., /dev/ttyACM0) and the FT8" << std::endl;
    std::cout << "frequency used in WSJT-X in MHz (e.g., 7.074)." << std::endl;
    std::cout << std::endl;
    std::cout << "Mandatory arguments to long options are mandatory for short options too." << std::endl;
    std::cout << visible_options << std::endl;
    std::cout << "Report bugs at <https://github.com/jdelacroix/qcx_cat_fsk/issues>." << std::endl;
    return EXIT_SUCCESS;
  }

  if (vm.count("version")) {
    std::cout << "qcx_cat_ft8 version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
    return EXIT_SUCCESS;
  }

  if (vm.count("device") == 0 || vm.count("frequency") == 0) {
    std::cout << "qcx_cat_ft8: missing <device name> or <frequency> argument." << std::endl; 
    std::cout << "See 'qcx_cat_ft8 --help'." << std::endl;
    return EXIT_FAILURE;
  }

  if (vm.count("log")) {
    boost::log::add_file_log(boost::log::keywords::file_name = vm["log"].as<std::string>(),
                           boost::log::keywords::auto_flush = true);
  } else {
    boost::log::add_file_log(boost::log::keywords::file_name = "qcx_cat_ft8.log",
                           boost::log::keywords::auto_flush = true);
  }

  
  boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                      boost::log::trivial::info);

  BOOST_LOG_TRIVIAL(info) << "!log initialized";

  // FIXME: hard coded serial port device
  std::shared_ptr<qcx_cat_fsk::CatInterface> qcx;
  try {
    qcx = std::make_shared<qcx_cat_fsk::CatInterface>(vm["device"].as<std::string>());
  } catch (const boost::system::system_error &e) {
    std::cout << "qcx_cat_ft8: unable to connect to QCX via " << vm["device"].as<std::string>() << std::endl;
    // return EXIT_FAILURE;
  }

  uint32_t vfo_b_hz = static_cast<uint32_t>(vm["frequency"].as<double>()*1e6);
  uint32_t vfo_a_hz = vfo_b_hz + vm["offset"].as<uint32_t>();

  std::ostringstream vfo_a_cmd_sstream;
  vfo_a_cmd_sstream << "FA" << std::setfill('0') << std::setw(11)
                    << vfo_a_hz
                    << ";";
  std::string vfo_a_cmd = vfo_a_cmd_sstream.str();

  try {
    auto reply = qcx->SendCommand("FT2;");
    BOOST_LOG_TRIVIAL(info) << "FT2; " << reply;
  } catch (...) {
    // no reply from QCX on this command
  }

  try {
    auto reply = qcx->SendCommand(vfo_a_cmd); // "FA00007074700;");
    BOOST_LOG_TRIVIAL(info) << vfo_a_cmd << " " << reply;
  } catch (...) {
    // no reply from QCX on this command
  }

  // FIXME: hard coded FT8 frequency
  qcx_cat_fsk::AudioDecoder ft8_decoder(qcx, vfo_a_hz, vfo_b_hz);

  ft8_decoder.Decode();
}
