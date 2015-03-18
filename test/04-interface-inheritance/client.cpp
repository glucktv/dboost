/*
 * client.cpp
 *
 *  Created on: 26.02.2015
 *      Author: stas
 */
#include <iostream>
#include <unistd.h>
#include "utils.h"
#include "monitorable_timer_proxy.hpp"

using namespace dboost;
using namespace std;

int main()
{
    dboost_test::monitorable_timer_proxy mp(create_connection(),
                                            "org.dboost.interfaceinheritance",
                                            "/org/dboost/monitorabletimer/0");


    // timer ifc
    long id = mp.add_timer(100500);
    cout << "Got id = " << id << endl;
    // monitorable timer ifc
    mp.set_monitor();
    mp.remove_timer(id);
    mp.remove_timer(id + 1);

    dboost_test::timer_proxy p(create_connection(),
                               "org.dboost.interfaceinheritance",
                               "/org/dboost/timer/0");
    id = p.add_timer(100500);
    cout << "Got id = " << id << endl;
    // won't compile:
    // p.set_monitor();

    return 0;
}
