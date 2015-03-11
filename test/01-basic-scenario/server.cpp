#include "server.h"
#include "timer_impl.h"
#include "timer_adaptor.h"
#include <boost_dispatcher.h>

int main()
{
    boost::asio::io_service ios;
    boost::asio::io_service::work work(ios);
    dboost::boost_dispatcher d(ios);
    dboost::server s("org.dboost.basicscenario");
    s.set_dispatcher(&d);
    dboost_test::timer_impl t;
    s.register_object<dboost_test::timer>(&t, "/org/dboost/timer/0");
    ios.run();
    return 0;
}
