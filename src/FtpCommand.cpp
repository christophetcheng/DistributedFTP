/*
 * FtpCommands.cpp
 *
 *  Created on: 21 Sep 2010
 *      Author: CTcheng
 */

#include "FtpCommand.h"
#include <string>
#include <iostream>

using namespace std;

FtpCommand::FtpCommand() {
	// TODO Auto-generated constructor stub

}

FtpCommand::~FtpCommand() {
	// TODO Auto-generated destructor stub
}

auto_ptr<FtpCommand> FtpCommandFactory::create(const std::string& query) {
	FtpCommand* ret = 0;
	string s = query.substr(0,4);
	if(s=="PORT") ret = new PortCmd();
	else if(s=="PASV") ret = new PasvCmd();
	else if(s=="STOR") ret = new StorCmd();
	else if(s=="STFW") ret = new StfwCmd();
	else if(s=="LOAD") ret = new LoadCmd();
	else return auto_ptr<FtpCommand>(0);

	if(ret->parseQuery(query)) return auto_ptr<FtpCommand>(ret);
	else return auto_ptr<FtpCommand>(0);
}

void FtpCommandFactory::unit_test() {
	auto_ptr<FtpCommand> cmd;

	if( (cmd = FtpCommandFactory::create("PORT 10,9,8,7,6,5")).get() != 0) {
		cmd->parseReply("200 Command OK.");
		cout << cmd->formatQuery() << endl;
		cout << cmd->formatReply() << endl;
	}
	else
		cout << "Error in PORT" << endl;

	if( (cmd = FtpCommandFactory::create("PASV")).get() != 0) {
		cmd->parseReply("502 Command not implemented.");
		cout << cmd->formatQuery() << endl;
		cout << cmd->formatReply() << endl;
	}
	else
		cout << "Error in PASV" << endl;

	if( (cmd = FtpCommandFactory::create("STOR myfile.txt")).get() != 0) {
		cmd->parseReply("200 Command OK.");
		cout << cmd->formatQuery() << endl;
		cout << cmd->formatReply() << endl;
	}
	else
		cout << "Error in STOR" << endl;

	if( (cmd = FtpCommandFactory::create("STFW myfile.txt | PORT 10,9,8,7,6,5")).get() != 0) {
		cmd->parseReply("200 Command OK.");
		cout << cmd->formatQuery() << endl;
		cout << cmd->formatReply() << endl;
	}
	else
		cout << "Error in STFW" << endl;


	if( (cmd = FtpCommandFactory::create("LOAD")).get() != 0) {
		cmd->parseReply("200 Load=10");
		cout << cmd->formatQuery() << endl;
		cout << cmd->formatReply() << endl;
	}
	else
		cout << "Error in STFW" << endl;
}
