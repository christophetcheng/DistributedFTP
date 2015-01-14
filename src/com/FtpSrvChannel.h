/*
 * FtpSrvChannel.h
 *
 *  Created on: 22 Sep 2010
 *      Author: CTcheng
 */

#ifndef FTPSRVCHANNEL_H_
#define FTPSRVCHANNEL_H_


class FtpSrvChannel {

public:
	// Launch the server channel
	static bool run(int port, bool mag);
};

#endif /* FTPSRVCHANNEL_H_ */
