/*
 * client.cpp
 *
 *  Created on: 26.02.2015
 *      Author: stas
 */
#include <iostream>
#include <unistd.h>
#include "utils.h"
#include "timer_proxy.hpp"

using namespace dboost;
using namespace std;

int main()
{
    dboost_test::timer_proxy p(create_connection(), "org.dboost.basicscenario", "/org/dboost/timer/0");
    try {
        long id = p.add_timer(100500);
        cout << "Got id = " << id << endl;
        p.remove_timer(id);
        p.remove_timer(id + 1);
    }
    catch (const dboost_test::wrong_timeout_exception& e) {
        cerr << "Caught exception: " << e.what() << endl;
    }
    return 0;
}
