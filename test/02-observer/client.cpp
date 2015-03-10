/*
 * client.cpp
 *
 *  Created on: 26.02.2015
 *      Author: stas
 */
#include "utils.h"
#include "timer_proxy.h"
#include <iostream>
#include <unistd.h>
#include <server.h>
#include <timer_observer_adaptor.h>

using namespace dboost;
using namespace std;

class observer: public dboost_test::timer_observer
{
public:
    virtual void on_timeout();
};

void observer::on_timeout()
{
    clog << "on_timeout" << endl;
}

int main()
{
    dboost::server s("org.dboost.observer.client");
	dboost_test::timer_proxy p(s, "org.dboost.observer.server", "/org/dboost/timer/0");
	observer obs;
	s.register_object<dboost_test::timer_observer>(&obs, "/org/dboost/timer/observer/1");

	long id = p.add_timer(100500, &obs);
	cout << "Got id = " << id << endl;


	usleep(5000 * 1000);

	p.remove_timer(&obs);
	usleep(5000 * 1000);
    return 0;
}
