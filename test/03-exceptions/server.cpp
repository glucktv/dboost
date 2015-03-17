#include <boost_dispatcher.h>
#include <utils.h>
#include "server.h"
#include "timer_impl.h"
#include "timer_adaptor.hpp"

int main()
{
    auto conn = dboost::create_connection();
    boost::asio::io_service ios;
    dboost::boost_dispatcher d(ios, conn);
    dboost::server s(conn, "org.dboost.basicscenario");
    s.set_dispatcher(&d);
    dboost_test::timer_impl t;
    dboost_test::timer_adaptor ta(s);
    ta.add_object(&t, "/org/dboost/timer/0");
    ios.run();
    return 0;
}
