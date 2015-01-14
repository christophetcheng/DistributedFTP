//============================================================================
// Name        : myftp.cpp
// Author      : Christophe Tcheng
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <pthread.h>
#include <sqlite3.h>
#include <sys/poll.h>
#include <iostream>

#include <FtpCommand.h>
#include <arpa/inet.h>
using namespace std;


int main(void) {

	FtpCommandFactory::unit_test();
	return 0;
}



