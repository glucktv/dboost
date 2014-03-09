#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;

class hello_world_server
{
public:
  void on_timeout(const boost::system::error_code& error)
  {
    if (error)
    {
      cerr << "Error happened" << endl;
    }
    clog << "Hello, World!" << endl;
  }
  
};

int main(int, char**)
{
  clog << "Starting..." << endl;
  boost::asio::io_service service;
  boost::asio::deadline_timer timer(service, boost::posix_time::seconds(1));
  hello_world_server s;
  timer.async_wait(boost::bind(&hello_world_server::on_timeout, &s, _1));
  service.run();
  clog << "Terminating..." << endl;
  return 0;
}
