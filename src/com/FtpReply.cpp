/*
 * FtpReply.cpp
 *
 *  Created on: 2 Nov 2010
 *      Author: vbox
 */

#include "FtpReply.h"
#include <map>
using namespace std;

class FtpReply_impl {
	map<int,string> map_;
public:
	FtpReply_impl();
	~FtpReply_impl() {};
	const string& operator[](int code) { return map_[code]; }
};

const std::string & FtpReply::to_text(int code) {
	static FtpReply_impl codes;
	return codes[code];
}

FtpReply_impl::FtpReply_impl() {
	map_[0] = "Empty Reply.";
	map_[110] = "Restart marker reply.";
	map_[120] = "Service ready in nnn minutes.";
	map_[125] = "Data connection already open; transfer starting.";
	map_[150] = "File status okay; about to open data connection.";
	map_[200] = "Command okay.";
	map_[202] = "Command not implemented, superfluous at this site.";
	map_[211] = "System status, or system help reply.";
	map_[212] = "Directory status.";
	map_[213] = "File status.";
	map_[214] = "Help message.";
	map_[215] = "%1% system type.";
	map_[220] = "Service ready for new user.";
	map_[221] = "Service closing control connection.";
	map_[225] = "Data connection open; no transfer in progress.";
	map_[226] = "Closing data connection.";
	map_[227] = "Entering Passive Mode (%1%).";
	map_[230] = "User logged in, proceed.";
	map_[250] = "Requested file action okay, completed.";
	map_[257] = "%1% created.";
	map_[331] = "User name okay, need password.";
	map_[332] = "Need account for login.";
	map_[350] = "Requested file action pending further information.";
	map_[421] = "Service not available, closing control connection.";
	map_[425] = "Can't open data connection.";
	map_[426] = "Connection closed; transfer aborted.";
	map_[450] = "Requested file action not taken.";
	map_[451] = "Requested action aborted. Local error in processing.";
	map_[452] = "Requested action not taken.";

	map_[490] = "Requested file action not taken.";
	map_[491] = "Requested file action not taken.";
	map_[492] = "Requested file action not taken.";
	map_[493] = "Requested file action not taken.";
	map_[494] = "Requested file action not taken.";
	map_[495] = "Requested file action not taken.";
	map_[496] = "Requested file action not taken.";
	map_[497] = "Requested file action not taken.";
	map_[498] = "Requested file action not taken.";
	map_[499] = "Requested file action not taken.";

	map_[500] = "Syntax error, command unrecognized.";
	map_[501] = "Syntax error in parameters or arguments.";
	map_[502] = "Command not implemented.";
	map_[503] = "Bad sequence of commands.";
	map_[504] = "Command not implemented for that parameter.";
	map_[530] = "Not logged in.";
	map_[532] = "Need account for storing files.";
	map_[550] = "Requested action not taken.";
	map_[551] = "Requested action aborted. Page type unknown.";
	map_[552] = "Requested file action aborted.";
	map_[553] = "Requested action not taken.";

}


