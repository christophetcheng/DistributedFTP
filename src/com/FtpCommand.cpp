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


// Tiny factory - move to boost/functional/factory
typedef std::unique_ptr<class FtpCommand> (*creator)();
static map<std::string,creator> factory;
template <class T> unique_ptr<class FtpCommand> create() {
	return unique_ptr<class FtpCommand>(new T());
}
template <class T> register_factory<T>::register_factory(const std::string& s) {
	factory[s] = create<T>;
}
register_factory<PasvCmd> PasvCmd::r("PASV");
register_factory<PvfwCmd> PvfwCmd::r("PVFW");
register_factory<PortCmd> PortCmd::r("PORT");
register_factory<ReinCmd> ReinCmd::r("REIN");
register_factory<StorCmd> StorCmd::r("STOR");
register_factory<RetrCmd> RetrCmd::r("RETR");
register_factory<StfwCmd> StfwCmd::r("STFW");
register_factory<LoadCmd> LoadCmd::r("LOAD");
register_factory<TypeCmd> TypeCmd::r("TYPE");


// end of tiny factory

unique_ptr<FtpCommand> FtpCommandFactory::create(const std::string& query) {

	map<std::string,creator>::const_iterator i = factory.find(query.substr(0,4));
	if(i == factory.end())
		return unique_ptr<FtpCommand>(new UnknownCmd());
	else {
		unique_ptr<FtpCommand> ret( (i->second)() ); // call the creator function
		if(ret->parse(query)) {
			return unique_ptr<FtpCommand>(move(ret));
		}
		else {
			cout << "Could not parse query [" << query << "]"<< endl;
			return unique_ptr<FtpCommand>(new UnknownCmd());
		}
	}
}


void FtpCommandFactory::unit_test() {

	const char* tests[] = {
			"PORT 10,9,8,7,6,5",
			"PASV",
			"RETR myfile.txt",
			"STOR myfile.txt",
			"STFW myfile.txt | PORT 10,9,8,7,6,5",
			"LOAD",
			0
	};

	// loop on test strings
	for(const char** ptr = tests; *ptr ; ++ptr){
		unique_ptr<FtpCommand> cmd = FtpCommandFactory::create(*ptr);
		// Try formatting, parsing
		unique_ptr<FtpCommand> cmd2 = FtpCommandFactory::create( cmd->format() );
		bool match = (cmd->format() == cmd2->format());
		cout << (match ? "OK: ":"----- KO: ") << *ptr << endl;
		if(!match)
			cout << "!=" << cmd->format() << endl
				<< "!=" << cmd2->format() << endl;
	}
}


