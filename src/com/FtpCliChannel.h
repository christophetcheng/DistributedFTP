/*
 * FtpCliChannel.h
 *
 *  Created on: 8 Nov 2010
 *      Author: vbox
 */

#ifndef FTPCLICHANNEL_H_
#define FTPCLICHANNEL_H_

#include "Utils.h"
#include "FtpCommand.h"
#include <string>
#include <istream>


class FtpCliChannel {
private:
	utils::socket_buf sock_;
public:
	FtpCliChannel(boost::asio::io_service& io) : sock_(io){}
	// warning: copy ctor will not copy socket but create an empty one
	FtpCliChannel(const FtpCliChannel& o) : sock_(const_cast<tcp::socket&>(o.sock_.sock_).io_service()) {}
	~FtpCliChannel() { close(); }

	tcp::socket& sock() {return sock_.sock_;}

	void close() { sock().close(); }

	void send(FtpCommand& cmd) 	{ 	utils::send_line(sock_,cmd.format());	}

	void reply(FtpReply& r) 	{	utils::send_line(sock_,r.format()); 	}
	void reply(int reply_code)  {	utils::send_line(sock_,FtpReply(reply_code).format());	}

	std::string recv_line(boost::system::error_code& ec) {
		return utils::recv_line(sock_,ec);
	}

	/* If expected_reply is set, the method will check for it */
	FtpReply recv(boost::system::error_code& ec, int expected_reply=-1) {
		FtpReply r;
		std::string s = utils::recv_line(sock_,ec);
		if(ec) {
			std::cout << "Failed to receive Reply ["<< s << "]" << std::endl;
			return r;
		}
		if(!r.parse(s) ) {
			std::cout << "Failed to parse reply ["<< s << "]" << std::endl;
			ec = boost::system::errc::make_error_code(boost::system::errc::invalid_argument);
			return r;
		}
		if(expected_reply!=-1 && expected_reply!=r.code()) {
			std::cout << "Unexpected reply ("<< expected_reply << ") ["<< s << "]" << std::endl;
			ec = boost::system::errc::make_error_code(boost::system::errc::protocol_error);
		}
		return r;
	}

	FtpReply send_recv(FtpCommand& cmd, boost::system::error_code& ec, int expected_reply) {
		send(cmd);
		FtpReply r = recv(ec,expected_reply);
		return r;
	}

};

typedef boost::shared_ptr<FtpCliChannel> channel_ptr;

#endif /* FTPCLICHANNEL_H_ */
