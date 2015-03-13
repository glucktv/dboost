#include "server.h"
#include "timer_impl.h"
#include "timer_adaptor.h"
#include <boost_dispatcher.h>
#include <utils.h>

int main()
{
    auto conn = dboost::create_connection();
    boost::asio::io_service ios;
    boost::asio::io_service::work work(ios);
    dboost::boost_dispatcher d(ios, conn);
    dboost::server s(conn, "org.dboost.basicscenario");
    dboost_test::timer_impl t;
    s.register_object<dboost_test::timer>(&t, "/org/dboost/timer/0");
    s.set_dispatcher(&d);
    ios.run();
    return 0;
}
