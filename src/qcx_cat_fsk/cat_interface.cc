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

#include "qcx_cat_fsk/cat_interface.h"

#include <boost/lambda/bind.hpp>
#include <boost/log/trivial.hpp>
#include <sstream>

namespace qcx_cat_fsk {

CatInterface::CatInterface(const std::string& qcx_serial_device)
    : serial_port_(io_service_), deadline_timer_(io_service_) {
  this->serial_port_.open(qcx_serial_device);
  this->serial_port_.set_option(boost::asio::serial_port::baud_rate(38400));

  CheckDeadline();
}

std::string CatInterface::SendCommand(const std::string& qcx_cat_command,
                                      const bool wait_for_reply) {
  std::size_t bytes_written = boost::asio::write(
      this->serial_port_, boost::asio::buffer(qcx_cat_command));
  BOOST_LOG_TRIVIAL(info) << "C: [" << qcx_cat_command << ", " << bytes_written
                          << "]";

  if (wait_for_reply == false) {
    return "";
  }

  boost::system::error_code ec = boost::asio::error::would_block;
  boost::asio::streambuf buffer;

  {
    std::lock_guard<std::mutex> lock(this->deadline_timer_mutex_);
    deadline_timer_.expires_from_now(boost::posix_time::seconds(1));
  }
  boost::asio::async_read_until(
      this->serial_port_, buffer, ';',
      (boost::lambda::var(ec) = boost::lambda::_1,
       boost::lambda::var(bytes_written) = boost::lambda::_2));

  do {
    this->io_service_.run_one();
  } while (ec == boost::asio::error::would_block);

  BOOST_LOG_TRIVIAL(info) << "R: [ ," << bytes_written << "]";

  if (ec) {
    throw boost::system::system_error(ec);
  }

  std::stringstream ss;
  ss << &buffer;

  return ss.str();
}

void CatInterface::CheckDeadline() {
  std::lock_guard<std::mutex> lock(this->deadline_timer_mutex_);
  if (this->deadline_timer_.expires_at() <=
      boost::asio::deadline_timer::traits_type::now()) {
    serial_port_.cancel();
    this->deadline_timer_.expires_at(boost::posix_time::pos_infin);
  }
  this->deadline_timer_.async_wait(
      boost::lambda::bind(&CatInterface::CheckDeadline, this));
}

}  // namespace qcx_cat_fsk