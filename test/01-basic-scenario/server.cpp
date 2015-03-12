#include "server.h"
#include "timer_impl.h"
#include "timer_adaptor.h"

//#define DISPATCHER BOOST_DISPATCHER
#define DISPATCHER POLL_DISPATCHER

#if DISPATCHER == BOOST_DISPATCHER
#include <boost_dispatcher.h>
#elif DISPATCHER == POLL_DISPATCHER
#include <poll_dispatcher.h>
#endif

int main()
{
    dboost::server s("org.dboost.basicscenario");
    dboost_test::timer_impl t;
    s.register_object<dboost_test::timer>(&t, "/org/dboost/timer/0");

#ifdef DISPATCHER
#if DISPATCHER == BOOST_DISPATCHER
    boost::asio::io_service ios;
    boost::asio::io_service::work work(ios);
    dboost::boost_dispatcher d(ios, s.get_connection());
    s.set_dispatcher(&d);
    ios.run();
#elif DISPATCHER == POLL_DISPATCHER
    dboost::poll_dispatcher d(s.get_connection().get());
    s.set_dispatcher(&d);
    d.run();
#endif
#else
    s.run();
#endif

    return 0;
}
