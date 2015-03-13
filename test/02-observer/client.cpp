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
#include <boost_dispatcher.h>

using namespace dboost;
using namespace std;

class observer: public dboost_test::timer_observer
{
    int m_call_cnt;
    boost::asio::io_service& m_ios;

public:
    observer(boost::asio::io_service& ios)
        : m_call_cnt(0),
          m_ios(ios) {}

    virtual void on_timeout();
};

void observer::on_timeout()
{
    clog << "*** on_timeout" << endl;
    ++m_call_cnt;
    if (m_call_cnt == 10) {
        m_ios.stop();
    }
    clog << "*** on_timeout end" << endl;
}

int main()
{
    auto conn = dboost::create_connection();
    boost::asio::io_service ios;
    dboost::boost_dispatcher d(ios, conn);
    dboost::server s(conn, "org.dboost.observer.client");

	dboost_test::timer_proxy p(s, "org.dboost.observer.server", "/org/dboost/timer/0");
	observer obs(ios);
	s.register_object<dboost_test::timer_observer>(&obs, "/org/dboost/timer/observer/1");

	long id = p.add_timer(1000, &obs);

	cout << "Got id = " << id << endl;
	s.set_dispatcher(&d);

    ios.run();

	p.remove_timer(&obs);
	clog << "timer removed" << endl;
	usleep(5000 * 1000);
    return 0;
}
