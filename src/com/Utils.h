/*
 * Utils.h
 *
 *  Created on: 4 Nov 2010
 *      Author: vbox
 */

#ifndef UTILS_H_
#define UTILS_H_



#include <iostream>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace boost::asio;

namespace utils {

struct socket_buf : private boost::noncopyable_::noncopyable {
	socket_buf(boost::asio::io_service& io) : sock_(io), sbuf_(), is_(&sbuf_) {}
	tcp::socket sock_;
	boost::asio::streambuf sbuf_;
	std::istream is_;
};

void send_line(socket_buf& sock_, const std::string& s);
std::string recv_line(socket_buf& sock, boost::system::error_code& ec);

bool parse_ipport(const std::string& s, tcp::endpoint& ip);
std::string format_ipport(const tcp::endpoint& ip);

}



#endif /* UTILS_H_ */
