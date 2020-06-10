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

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/lambda/lambda.hpp>
#include <mutex>
#include <string>

namespace qcx_cat_fsk {

class CatInterface {
 public:
  CatInterface(const std::string& qcx_serial_device);

  std::string SendCommand(const std::string& qcx_cat_command,
                          const bool wait_for_reply = true);

 private:
  void CheckDeadline();

  boost::asio::io_service io_service_;
  boost::asio::serial_port serial_port_;
  boost::asio::deadline_timer deadline_timer_;

  std::mutex deadline_timer_mutex_;
};

}  // namespace qcx_cat_fsk