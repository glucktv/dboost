#include "server.h"
#include "database_impl.h"
#include "database_adaptor.h"
#include <boost_dispatcher.h>
#include <utils.h>


int main()
{
    auto conn = dboost::create_connection();
    boost::asio::io_service ios;
    dboost::boost_dispatcher d(ios, conn);
    dboost::server s(conn, "org.dboost.structpassing");
    dboost_test::database_impl db;
    s.register_object<dboost_test::database>(&db, "/org/dboost/database/0");
    s.set_dispatcher(&d);
    ios.run();
    return 0;
}
