#include "server.h"
#include "database_impl.h"
#include "database_adaptor.h"

int main()
{
    dboost::server s("org.dboost.structpassing");
    dboost_test::database_impl d;
    dboost_test::database_adaptor da(s);
    da.add_object(&d, "/org/dboost/database/0");
    s.run();
    return 0;
}
