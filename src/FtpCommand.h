/*
 * FtpCommands.h
 *
 *  Created on: 21 Sep 2010
 *      Author: CTcheng
 */

#ifndef FTPCOMMANDS_H_
#define FTPCOMMANDS_H_

#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <arpa/inet.h>

class FtpCommand;
class FtpCommandFactory {
public:
	static void unit_test();
	static std::auto_ptr<FtpCommand> create(const std::string& query);
};

// Double inheritance would provide more compact code
class FtpCommand {
	int replycode_;
	std::string replytext_;
public:
	FtpCommand();
	virtual std::string formatQuery() { return "not supported"; };
	virtual bool 	    parseQuery(const std::string& r) {return false;};

	int getReplyCode() { return replycode_; }
	const std::string& getReplyText() { return replytext_; }

	virtual void setReply(int code, const std::string& text) {
		replycode_ = code;
		replytext_ = text;
	}
	virtual std::string formatReply() {
		return boost::str(boost::format("%1% %2%") % replycode_ % replytext_);
	};
	virtual bool 	    parseReply(const std::string& r){
		static const boost::regex reg("(\\d{3}) (.*)");
		boost::smatch what;
		if(boost::regex_match(r, what, reg))
		{
			replycode_ = strtol(what[1].str().c_str(),0,10); // conversion should word as matched against digits
			replytext_ = what[2].str();
			return true;
		}
		else
			return false;
	};

	virtual ~FtpCommand();
};

class PortCmd : public FtpCommand {
	struct sockaddr_in p_;
public:
	PortCmd() :p_() {}
	PortCmd(struct sockaddr_in* p) :p_(*p) {}

	std::string formatQuery() {
		uint16_t  port = ntohs(p_.sin_port);
		uint32_t ip = ntohl(p_.sin_addr.s_addr);

		return boost::str( boost::format("PORT %1%,%2%,%3%,%4%,%5%,%6%")
		% ((ip>>24) & 0xFF)
		% ((ip>>16) & 0xFF)
		% ((ip>>8 ) & 0xFF)
		% ((ip    ) & 0xFF)
		% ((port>>8) & 0xFF)
		% ((port   ) & 0xFF) );
	}
	bool parseQuery(const std::string& r) {
		boost::smatch what;
		static const boost::regex reg("PORT[ ]*(\\d*),(\\d*),(\\d*),(\\d*),(\\d*),(\\d*)");

		memset(&p_.sin_addr,0,sizeof(p_.sin_addr));
		p_.sin_port = 0;
		if(boost::regex_match(r, what, reg))
		{
			uint32_t ip = 0;
			for(int i=0;i<4;++i)
			{
				// strtol can only work since we have matched \d*
				ip = (ip<<8) + strtol(what[i+1].str().c_str(),0,10);
			}
			p_.sin_addr.s_addr = htonl(ip);
			p_.sin_port = htons( (strtol(what[5].str().c_str(),0,10)<<8) + strtol(what[6].str().c_str(),0,10) );
			return true;
		}
		else
			return false;
	}
};

class PasvCmd : public FtpCommand {
public:
	PasvCmd() {};
	std::string formatQuery() {
		return "PASV";
	}
	bool parseQuery(const std::string& r) {
		return r=="PASV";
	}
};

class StorCmd : public FtpCommand {
	std::string filename_;
public:
	StorCmd() {};
	std::string formatQuery() {
		return "STOR "+filename_;
	}
	bool parseQuery(const std::string& r) {
		boost::smatch what;
		static const boost::regex reg("STOR[ ]*(.*)");
		if(boost::regex_match(r, what, reg))
		{
			filename_ = what[1].str();
			return true;
		}
		else
			return false;
	}
};

class StfwCmd : public FtpCommand {
	std::string filename_;
	PortCmd portcmd_;
public:
	StfwCmd() {};
	StfwCmd(const std::string& f,const PortCmd& p) : filename_(f), portcmd_(p) {};

	std::string formatQuery() {
		return "STFW "+filename_+" | "+portcmd_.formatQuery();
	}
	bool parseQuery(const std::string& r) {
		boost::smatch what;
		static const boost::regex reg("STFW[ ]*([^\\|]*)[ ]*\\|[ ]*(.*)");
		if(boost::regex_match(r, what, reg))
		{
			// needs to parse STOR + PORT
			filename_ = what[1].str();
			return portcmd_.parseQuery(what[2].str());
		}
		else
			return false;
	}
};


class LoadCmd : public FtpCommand {
	int load_;
public:
	LoadCmd() {};
	LoadCmd(int load) : load_(load) {};

	void setLoad(int load) { load_ = load; }
	int getLoad() { return load_; }

	std::string formatQuery() {
		return "LOAD";
	}
	bool parseQuery(const std::string& r) {
		return r=="LOAD";
	}
	virtual std::string formatReply() {
		setReply(getReplyCode(),boost::str(boost::format("Load=%1%") %load_) );
		return FtpCommand::formatReply();
	};
	virtual bool 	    parseReply(const std::string& r){
		static const boost::regex reg("(\\d{3}) Load=(\\d*)");
		boost::smatch what;
		if( FtpCommand::parseReply(r) && (boost::regex_match(r, what, reg)) )
		{
			load_= strtol(what[2].str().c_str(),0,10);
			return true;
		}
		else
			return false;
	};
};

#endif /* FTPCOMMANDS_H_ */
