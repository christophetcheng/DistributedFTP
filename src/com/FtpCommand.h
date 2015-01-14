/*
 * FtpCommands.h
 *
 *  Created on: 21 Sep 2010
 *      Author: CTcheng
 */

#ifndef FTPCOMMANDS_H_
#define FTPCOMMANDS_H_

#include <string>

#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace boost::asio;


#include "FtpReply.h"

template <class T> struct register_factory {
	register_factory(const std::string& s);
};
class FtpCommandFactory {
public:
	static void unit_test();
	static std::unique_ptr<class FtpCommand> create(const std::string& query);
};

// Let's have a visitor to decouple commands and channels
class FtpCommandVisitor {
public:
	virtual void execute(class UnknownCmd&) =0;
	virtual void execute(class LoadCmd&) =0;
	virtual void execute(class PasvCmd&) =0;
	virtual void execute(class PvfwCmd&) =0;
	virtual void execute(class PortCmd&) =0;
	virtual void execute(class ReinCmd&) =0;
	virtual void execute(class StfwCmd&) =0;
	virtual void execute(class StorCmd&) =0;
	virtual void execute(class RetrCmd&) =0;
	virtual void execute(class TypeCmd&) =0;
};

// Double inheritance would provide more compact code, but less readable
class FtpCommand {
public:
	FtpCommand() {};
	virtual ~FtpCommand() {};

	virtual void execute(FtpCommandVisitor& v) =0;
	virtual std::string format() =0;
	virtual bool 	    parse(const std::string& r) =0;
};

class PortCmd : public FtpCommand {
	tcp::endpoint p_;
	static register_factory<PortCmd> r;
public:
	PortCmd() : p_() {  }
	PortCmd(tcp::endpoint p) :p_(p) {}

	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }

	tcp::endpoint getPort() { return p_; } // can be used for set/get

	std::string format() {
		return "PORT "+utils::format_ipport(p_);
	}
	bool parse(const std::string& r) {
		boost::smatch what;
		static const boost::regex reg("PORT[ ]*([,\\d]*)");
		// Now par
		return boost::regex_match(r, what, reg) && utils::parse_ipport(what[1],p_);
	}
};

class PasvCmd : public FtpCommand {
	static register_factory<PasvCmd> r;
public:
	std::string format() {
		return "PASV";
	}
	bool parse(const std::string& r) {
		return r=="PASV";
	}
	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }
};

// Let's support only binary
class TypeCmd : public FtpCommand {
	static register_factory<TypeCmd> r;
public:
	std::string format() {
		return "TYPE I";
	}
	bool parse(const std::string& r) {
		return r=="TYPE I";
	}
	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }
};

class PvfwCmd : public FtpCommand {
	static register_factory<PvfwCmd> r;
public:
	PvfwCmd()  {};
	std::string format() {
		return "PVFW";
	}
	bool parse(const std::string& r) {
		return r=="PVFW";
	}
	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }
};

class ReinCmd : public FtpCommand {
	static register_factory<ReinCmd> r;
public:
	ReinCmd() {};
	std::string format() {
		return "REIN";
	}
	bool parse(const std::string& r) {
		return r=="REIN";
	}
	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }
};


class StorCmd : public FtpCommand {
	std::string filename_;
	static register_factory<StorCmd> r;
public:
	StorCmd() {};
	StorCmd(const std::string& f) : filename_(f) {};

	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }

	const std::string& filename() { return filename_; }
	void filename(const std::string& f) { filename_ = f; }

	std::string format() {
		return "STOR "+filename_;
	}

	bool parse(const std::string& r) {
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


class StfwCmd : public FtpCommand{
	std::string filename_;
	static register_factory<StfwCmd> r;
public:
	StfwCmd() {};
	StfwCmd(const std::string& f) : filename_(f) {};

	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }

	const std::string& filename() { return filename_; }
	void filename(const std::string& f) { filename_ = f; }

	std::string format() {
		return "STFW "+filename_;
	}

	bool parse(const std::string& r) {
		boost::smatch what;
		static const boost::regex reg("STFW[ ]*(.*)");
		if(boost::regex_match(r, what, reg))
		{
			filename_ = what[1].str();
			return true;
		}
		else
			return false;
	}
};

class RetrCmd : public FtpCommand {
	static register_factory<RetrCmd> r;
	std::string filename_;
public:
	RetrCmd() {};
	RetrCmd(const std::string& f) : filename_(f) {};
	std::string format() {
		return "RETR "+filename_;
	}
	const std::string& filename() { return filename_; }
	void filename(const std::string& f) { filename_ = f; }

	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }
	bool parse(const std::string& r) {
		boost::smatch what;
		static const boost::regex reg("RETR[ ]*(.*)");
		if(boost::regex_match(r, what, reg))
		{
			filename_ = what[1].str();
			return true;
		}
		else
			return false;
	}
};


class LoadCmd : public FtpCommand {
	int load_;
	static register_factory<LoadCmd> r;
public:
	LoadCmd() {};
	LoadCmd(int load) : load_(load) {}

	void setLoad(int load) { load_ = load; }
	int getLoad() { return load_; }

	std::string format() {
		return "LOAD";
	}
	bool parse(const std::string& r) {
		return r=="LOAD";
	}
	virtual void execute(FtpCommandVisitor& v) {v.execute(*this); }
};


class UnknownCmd : public FtpCommand {
public:
	UnknownCmd() {};
	std::string format() {return "UNKNOWN"; }
	bool parse(const std::string& r) {
		return true;
	}
	virtual void execute(FtpCommandVisitor& v) { v.execute(*this); }
};

#endif /* FTPCOMMANDS_H_ */
