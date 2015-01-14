/*
 * ftpagt.cpp
 *
 *  Created on: 21 Sep 2010
 *      Author: CTcheng
 */

#include "FtpSrvChannel.h"
#include <iostream>
using namespace std;

int main(int argc, char * argv[]) {

	if(argc != 2) {
		cout << "Usage: " << argv[0] << " [0|1|2|...]" << endl;
		return 0;
	}
	int i = strtol(argv[1],0,10);
	FtpSrvChannel::run(21210+i,false);

	return 0;
}
