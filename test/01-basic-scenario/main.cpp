#include "server.h"
#include "timer_impl.h"
#include "timer_adaptor.h"

using namespace dboost;
using namespace std;

int main()
{
	server s("org.dboost.basicscenario");
	dboost_test::timer_impl t;
	timer_adaptor ta(s);
	ta.add_object(&t, "/org/dboost/timer/0");
	s.run();
	return 0;
}
