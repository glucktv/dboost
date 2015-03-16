#include "server.h"
#include "timer_impl.h"
#include "timer_adapter.hpp"

int main()
{
    dboost::server s("org.dboost.basicscenario");
    dboost_test::timer_impl t;
    dboost_test::timer_adapter ta(s);
    ta.add_object(&t, "/org/dboost/timer/0");
    s.run();
    return 0;
}
