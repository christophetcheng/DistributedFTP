/*
 * FtpSrvChannel.cpp
 *
 *  Created on: 22 Sep 2010
 *      Author: CTcheng
 */

#include "FtpSrvChannel.h"
#include "FtpCliChannel.h"
#include "FtpCommand.h"
#include "FtpReply.h"
#include "Utils.h"
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <fstream>

using namespace std;
using namespace utils;
using namespace boost;


class FtpAgtChannel_impl : public FtpCommandVisitor{
protected:
	boost::asio::io_service io_service;
	channel_ptr channel_sock_;
	tcp::endpoint data_endpoint_;
	unique_ptr<tcp::acceptor> data_pvfw_acceptor_; // non zero when pvfw is active

public:
	FtpAgtChannel_impl(channel_ptr sock) : io_service(), channel_sock_(sock) {}
	virtual ~FtpAgtChannel_impl() {}

	static void handle_session(channel_ptr sock, bool mag);
	void session();

	virtual void execute(class UnknownCmd&);
	virtual void execute(class LoadCmd&);
	virtual void execute(class TypeCmd&);
	virtual void execute(class PasvCmd&);
	virtual void execute(class PvfwCmd&);
	virtual void execute(class PortCmd&);
	virtual void execute(class ReinCmd&);
	virtual void execute(class StfwCmd&);
	virtual void execute(class StorCmd&);
	virtual void execute(class RetrCmd&);

};

struct FtpMagChannel_impl : public FtpAgtChannel_impl {
	FtpMagChannel_impl(channel_ptr sock) : FtpAgtChannel_impl(sock) {}
	virtual ~FtpMagChannel_impl() {}

//	virtual void execute(class LoadCmd&);
//	virtual void execute(class PasvCmd&);
//	virtual void execute(class PortCmd&);
//	virtual void execute(class ReinCmd&);
//	virtual void execute(class StfwCmd&);
	virtual void execute(class StorCmd&);
	virtual void execute(class RetrCmd&);
private:

};


void FtpAgtChannel_impl::handle_session(channel_ptr sock, bool mag) {
	unique_ptr<FtpAgtChannel_impl> pimpl(mag ? new FtpMagChannel_impl(sock) : new FtpAgtChannel_impl(sock) );
	pimpl->session();
}

bool FtpSrvChannel::run(int port, bool mag)
{
	boost::asio::io_service io_service;
	tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
	for (;;)
	{
		channel_ptr sock(new FtpCliChannel(io_service));
		a.accept(sock->sock());
		// start with one thread per connection, will do async later
		boost::thread t(boost::bind(FtpAgtChannel_impl::handle_session, sock, mag));
	}
	return true;
}

void FtpAgtChannel_impl::session()
{
	try
	{
		// Greetings first
		cout << "Sending greetings" << endl;
		channel_sock_->reply(220);

		for (;;)
		{
			system::error_code error;
			string query = channel_sock_->recv_line(error);

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error) {
				channel_sock_->sock().close();
				return;
			}

			cout << "Received query:" << query << endl;

			unique_ptr<FtpCommand> cmd = FtpCommandFactory::create(query);
			if(cmd.get() != 0 ) {
				cmd->execute(*this);
				cout << "Query executed" << endl;
			}
		}
	}
	catch (std::exception& e)
	{
		cout << "Caught Exception what()=" << e.what() << endl;
	}
}


void FtpAgtChannel_impl::execute(UnknownCmd& cmd) {
	//@todo
	channel_sock_->reply(502);
}
void FtpAgtChannel_impl::execute(LoadCmd& cmd) {
	//@todo
	channel_sock_->reply(502);
}
void FtpAgtChannel_impl::execute(PasvCmd& cmd) {
	// @todo
	// 1. open port
	// 2. listen
	// 3. send 227 response	boost::format( FtpReply::text(227) % .... )
	channel_sock_->reply(502);
}
void FtpAgtChannel_impl::execute(TypeCmd& cmd) {
	channel_sock_->reply(200);
}
void FtpAgtChannel_impl::execute(PvfwCmd& cmd) {

	// 1. open port and listen
	data_pvfw_acceptor_.reset(new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), 0)));
	unsigned short port = data_pvfw_acceptor_->local_endpoint().port();
	cout << "Local Port is now:" << port << endl;
	// 2. send 227 response	boost::format( FtpReply::text(227) % .... )
	FtpReply r(227);
	string s = boost::str(boost::format(r.text()) % utils::format_ipport(data_pvfw_acceptor_->local_endpoint()));
	r.text(s);
	channel_sock_->reply(r);
}

void FtpAgtChannel_impl::execute(PortCmd& cmd) {
	data_endpoint_ = cmd.getPort();
	channel_sock_->reply(200);
}
void FtpAgtChannel_impl::execute(ReinCmd& cmd) {
	//@todo
	channel_sock_->reply(502);
}
void FtpAgtChannel_impl::execute(StfwCmd& cmd) {
	//@todo
	channel_sock_->reply(502);
}

void FtpAgtChannel_impl::execute(RetrCmd& cmd) {

	// open file
	std::ifstream file(cmd.filename().c_str(), ios_base::binary);
	if(!file.is_open()) {
		channel_sock_->reply(450);
		return;
	}

	// open socket
	tcp::socket datasock(io_service);
	system::error_code error;
	datasock.connect(data_endpoint_, error);
	if(error == system::errc::success) {
		channel_sock_->reply(150);
	}
	else {
		channel_sock_->reply(425);
		return;
	}

	// we could use sendfile here, but not sure how it would interact with boost
	size_t total = 0;
	boost::array<char, 1024*1024> buf;
	while(file.good()) {
		file.read(buf.c_array(),buf.size());
		datasock.send(boost::asio::buffer(buf,file.gcount()));
		total += file.gcount();
	}
	file.close();
	datasock.close();
	cout << "Transferred "<< total << " bytes" << endl;

	// Finally reply
	channel_sock_->reply(226);
}


void FtpAgtChannel_impl::execute(StorCmd& cmd) {
	system::error_code error;
	// take ownership of the pvfw acceptor so it get deleted at the end;
	unique_ptr<tcp::acceptor> pvfw_acc(move(data_pvfw_acceptor_));

	tcp::socket datasock(io_service);
	datasock.connect(data_endpoint_, error);
	if(error) {
		channel_sock_->reply(425);
		return;
	}
	channel_sock_->reply(150);


	// Handle pvfw case
	tcp::socket pvfw_sock(io_service);
	if(pvfw_acc.get()) {
		pvfw_acc->accept(pvfw_sock,error);
		if(error) {
			// notify the mag we couldn't accept
			channel_sock_->reply(450);
			return;
		}
	}

	// now write to the disk
	std::ofstream file(cmd.filename().c_str(), std::ios_base::binary);

	size_t total = 0;
	boost::array<char, 1024*1024> buf;
	for(;;) {
		size_t len = datasock.read_some(boost::asio::buffer(buf),error);
		total += len;
		if(len > 0) {
			file.write(buf.data(),len);
			if(pvfw_acc.get())	pvfw_sock.send(boost::asio::buffer(buf.data(),len));
		}

		if (error == boost::asio::error::eof ) { //if end of file reached
			file.close();
			datasock.close();
			if(pvfw_acc.get()) pvfw_sock.close();
			cout << "Transferred "<< total << " bytes" << endl;
			channel_sock_->reply(226);
			return;
		}
		else if (error) {
			channel_sock_->reply(452);
			return;
		}
	}
}


void FtpMagChannel_impl::execute(RetrCmd& cmd) {
	system::error_code error;
	// Now choose an agent
	FtpCliChannel agt_channel(io_service);
	tcp::endpoint agt_endpoint = channel_sock_->sock().local_endpoint();
	agt_endpoint.port(10 * agt_endpoint.port()); // @todo need database access

	agt_channel.sock().connect(agt_endpoint, error);
	if(error) {
		channel_sock_->reply(425);
		return;
	}
	cout << "agtsock: connected" << endl;

	//Expect greetings from agent
	agt_channel.recv(error,220);
	if(error) {
		channel_sock_->reply(450);
		return;
	}

	// Send port command to agent, expect 200
	{
		PortCmd c(data_endpoint_);
		agt_channel.send_recv(c,error,200);
		if( error ) {
			channel_sock_->reply(450);
			return;
		}
	}

	// Send retr command to agent, expect 150
	{
		RetrCmd c(cmd.filename());
		agt_channel.send_recv(c,error,150);
		if( error ) {
			channel_sock_->reply(450);
			return;
		}
		else
			channel_sock_->reply(150);
	}

	// Transfer should happen now, wait for 226 transfer completed
	cout << "waiting for 226 from agtsock" << endl;
	agt_channel.recv(error,226);
	if( error ) {
		channel_sock_->reply(450);
		return;
	}
	else {
		channel_sock_->reply(226);
		cout << "Transfer was reported OK !!!" << endl;
		return;
	}

}

template <class InputIterator, class Func>
system::error_code& broadcast( InputIterator first, InputIterator last, system::error_code& init, Func func)
{
  while ( first!=last  && !init)
	  func(*first++,init);
  return init;
}

void FtpMagChannel_impl::execute(StorCmd& cmd) {
	vector<FtpCliChannel> agtchannels(2,FtpCliChannel(io_service));

	// Now choose two agents
	// @todo need database access
	tcp::endpoint agt_endpoint = channel_sock_->sock().local_endpoint();
	int base_port = 10 * channel_sock_->sock().local_endpoint().port();
	agt_endpoint.port(base_port);

	cout << "---------------------------------------------- " << endl
			<< "Connecting to agent channels: " << base_port << endl;

	system::error_code error;
	// loop until no error
	for(auto i=agtchannels.begin();!error && i!=agtchannels.end();++i) {
		i->sock().connect(agt_endpoint,error);
		agt_endpoint.port(1 + agt_endpoint.port());
	}
	if(error) {
		channel_sock_->reply(490);
		return;
	}
	cout << "agent channels: connected" << endl;

	//Expect greetings from agents
	{
		auto func = boost::bind(&FtpCliChannel::recv,_1,_2,220);
		broadcast(agtchannels.begin(),agtchannels.end(),error,func);
		if(error) {
			channel_sock_->reply(491);
			return;
		}
	}

	// Send pvfw command to all agents except last, expect 227
	{
		PvfwCmd c;
		auto end = agtchannels.end()-1;
		for(auto i = agtchannels.begin();i!=end;++i) {
			FtpReply r = agtchannels[0].send_recv(c,error,227);
			// @TODO: parse response and extract IP for agent 2
			if( error ) {
				channel_sock_->reply(492);
				return;
			}
			// Extract port from response
			tcp::endpoint ip;
			static const boost::regex reg(".*\\((.*)\\).*");
			boost::smatch what;
			if(!boost::regex_match(r.text(), what, reg) || !utils::parse_ipport(what[1],ip)) {
				channel_sock_->reply(493);
				return;
			}
			// Send port command to next agent, expect 200
			PortCmd c(ip);
			(i+1)->send_recv(c,error,200);
			if( error ) {
				channel_sock_->reply(494);
				return;
			}
		}
	}

	// Send port command to agent, expect 200
	{
		PortCmd c(data_endpoint_);
		agtchannels[0].send_recv(c,error,200);
		if( error ) {
			channel_sock_->reply(495);
			return;
		}
	}

	// Transfer Stor command to agents
	{
		StorCmd c(cmd.filename());
		auto func = boost::bind(&FtpCliChannel::send_recv,_1,c,_2,150);
		broadcast(agtchannels.begin(),agtchannels.end(),error,func);
		if( error ) {
			channel_sock_->reply(496);
			return;
		}
		channel_sock_->reply(150);
	}

	{
		// Transfer should happen now, wait for 226 transfer completed
		cout << "waiting for 226 from agents" << endl;
		auto func = boost::bind(&FtpCliChannel::recv,_1,_2,226);
		broadcast(agtchannels.begin(),agtchannels.end(),error,func);
		if(error) {
			channel_sock_->reply(497);
			return;
		}
		channel_sock_->reply(226);
		cout << "Transfer was reported OK on both agents !!!!!!!" << endl;
	}
}


