#include "FtpCommand.h"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace boost::asio;
using namespace std;
int main(void) {
	boost::asio::io_service io;
	{
		tcp::resolver resolver(io);
		tcp::resolver::query query("localhost","");
		auto it=resolver.resolve(query);

		while(it!=tcp::resolver::iterator())
		{
		   boost::asio::ip::address addr=(it++)->endpoint().address();
		   if(addr.is_v6())
		   {
		    std::cout<<"ipv6 address: ";
		   }
		   else
		    std::cout<<"ipv4 address: ";

		   std::cout<<addr.to_string()<<std::endl;

		}
	}
	{
		tcp::endpoint e(tcp::v4(),0);
		tcp::acceptor a(io);

		a.open(tcp::v4());

		cout << "hostname: " << ip::host_name() << endl;

		cout << a.local_endpoint() << endl;

		a.listen(1);
		cout << a.local_endpoint() << endl;
	}
	FtpCommandFactory::unit_test();
}
