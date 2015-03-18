#include <boost_dispatcher.h>
#include <utils.h>
#include "server.h"
#include "monitorable_timer_impl.h"
#include "monitorable_timer_adaptor.hpp"

int main()
{
    auto conn = dboost::create_connection();
    boost::asio::io_service ios;
    dboost::boost_dispatcher d(ios, conn);
    dboost::server s(conn, "org.dboost.interfaceinheritance");
    s.set_dispatcher(&d);
    dboost_test::monitorable_timer_impl mt;
    dboost_test::monitorable_timer_impl t;
    dboost_test::monitorable_timer_adaptor mta(s);
    dboost_test::timer_adaptor ta(s);
    mta.add_object(&mt, "/org/dboost/monitorabletimer/0");
    ta.add_object(&t, "/org/dboost/timer/0");
    ios.run();
    return 0;
}
