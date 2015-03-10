#include "server.h"
#include "timer_impl.h"
#include "timer_adaptor.h"

int main()
{
    dboost::server s("org.dboost.observer.server");
    dboost_test::timer_impl t;
    s.register_object<dboost_test::timer>(&t, "/org/dboost/timer/0");
    s.run();
    return 0;
}
