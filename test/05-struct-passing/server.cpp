#include "server.h"
#include "database_impl.h"
#include "database_adaptor.h"
#include <boost_dispatcher.h>
#include <utils.h>


int main()
{
    auto conn = dboost::create_connection();
    boost::asio::io_service ios;
    dboost::boost_dispatcher disp(ios, conn);
    dboost::server s(conn, "org.dboost.structpassing");
    s.set_dispatcher(&disp);
    dboost_test::database_impl impl;
    dboost_test::database_adaptor da(s);
    da.add_object(&impl, "/org/dboost/database/0");
    ios.run();
    return 0;
}
