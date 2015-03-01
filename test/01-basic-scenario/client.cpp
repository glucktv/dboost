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

int main()
{
	dboost_test::timer_proxy p(create_connection(), "org.dboost.basicscenario", "/org/dboost/timer/0");
	long id = p.add_timer(100500);
	cout << "Got id = " << id << endl;
	//usleep(5000 * 1000);
	p.remove_timer(id);
	p.remove_timer(id + 1);
    return 0;
}
