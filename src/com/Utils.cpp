/*
 * Utils.cpp
 *
 *  Created on: 8 Nov 2010
 *      Author: vbox
 */

#include "Utils.h"
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <iostream>
using namespace std;

void utils::send_line(socket_buf& sock, const std::string& s) {
	cout << "Sending: " << s << endl;
	sock.sock_.send(boost::asio::buffer(s));
	sock.sock_.send(boost::asio::buffer("\r\n"));
}

std::string  utils::recv_line(socket_buf& sock, boost::system::error_code& ec) {
	size_t length = boost::asio::read_until(sock.sock_, sock.sbuf_, "\r\n", ec);
	std::string l;

	if(!ec) {
		std::getline(sock.is_, l);
		cout << "Received " << length << " bytes: " << l << endl;
		// trim string
		while(l[0]==0 || l[0]=='\r' || l[0]=='\n') l = l.substr(1,l.size()-1);
		while(*l.rbegin()==0 || *l.rbegin()=='\r' || *l.rbegin()=='\n') l = l.substr(0,l.size()-1);
	} else if(ec == boost::asio::error::eof) {
		cout << "Socket closed gracefully" << endl;
	}
	else {
		cout << "Failed to read_until with code: " << ec << endl;
	}

	return l;
}

bool utils::parse_ipport(const std::string& s, tcp::endpoint& ip) {
	boost::smatch what;
	static const boost::regex reg("(\\d*),(\\d*),(\\d*),(\\d*),(\\d*),(\\d*)");

	if(boost::regex_match(s, what, reg))
	{
		ip::address_v4::bytes_type b;
		for(int i=0;i<4;++i)
		{
			// strtol can only work since we have matched \d*
			b[i] = strtol(what[i+1].str().c_str(),0,10);
		}

		// Set ip and port
		ip.port((strtol(what[5].str().c_str(),0,10)<<8) + strtol(what[6].str().c_str(),0,10));
		ip.address(ip::address_v4(b));

		return true;
	}
	else {
		cout << "Could not parse ip/port: " << s << endl;
		return false;
	}

}

string utils::format_ipport(const tcp::endpoint& ip) {
	uint16_t  port = ip.port();
	ip::address_v4::bytes_type addr = ip.address().to_v4().to_bytes();

	return boost::str( boost::format("%1%,%2%,%3%,%4%,%5%,%6%")
	% (unsigned int)addr[0]
	% (unsigned int)addr[1]
	% (unsigned int)addr[2]
	% (unsigned int)addr[3]
	% ((port>>8) & 0xFF)
	% ((port   ) & 0xFF) );
}
