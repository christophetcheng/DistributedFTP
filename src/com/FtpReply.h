/*
 * FtpReply.h
 *
 *  Created on: 2 Nov 2010
 *      Author: vbox
 */

#ifndef FTPREPLY_H_
#define FTPREPLY_H_
#include <string>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <iostream>
#include "Utils.h"



class FtpReply {
	int code_;
	std::string text_;
	const std::string& to_text(int code);
public:
	FtpReply() {code(0);}
	FtpReply(int c) { code(c); }
	~FtpReply() {}

	void reset() { code(0); }

	int code() { return code_; }
	void code(int code) { set( code, to_text(code) ); }

	const std::string& text() { return text_; }
	void text(const std::string& t) { text_ = t; }

	void set(int code,const std::string& text) {
		code_ = code; text_ = text;
	}

	std::string format() {
		return boost::str(boost::format("%1% %2%") % code_ % text_);
	};


	bool parse(const std::string& r){
		reset();
		static const boost::regex reg("(\\d{3}) (.*)");
		boost::smatch what;

		if(boost::regex_match(r, what, reg))
		{
			code_ = strtol(what[1].str().c_str(),0,10); // conversion should word as matched against 3 digits
			text_ = what[2].str();
			return true;
		}
		else {
			std::cout << "Reply parsing Failed for ["<< r << "]."<< r.size() << std::endl;
			return false;
		}
	};
};

#endif /* FTPREPLY_H_ */
