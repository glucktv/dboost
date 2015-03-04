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

using namespace dboost;
using namespace std;

class observer: public timer_observer
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
	dboost_test::timer_proxy p(create_connection(), "org.dboost.observer.server", "/org/dboost/timer/0");
	dboost::server s("org.dboost.observer.client");
	dboost_test::timer_observer_adaptor toa(s);
	observer obs;
	toa.add_object(obs);

	long id = p.add_timer(100500, obs);
	cout << "Got id = " << id << endl;


	usleep(5000 * 1000);

	p.remove_timer(obs);

    return 0;
}
