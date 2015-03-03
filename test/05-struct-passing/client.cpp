#include "utils.h"
#include "database_proxy.h"
#include <iostream>
#include <unistd.h>

using namespace dboost;
using namespace std;

int main()
{
    dboost_test::database_proxy p(create_connection(), "org.dboost.structpassing", "/org/dboost/database/0");
    dboost_test::date d;

    dboost_test::person pers;
    pers.name = "Heirich";
    pers.family_name = "Hertz";
    pers.birth_date.year = 1857;
    pers.birth_date.month = 2;
    pers.birth_date.day = 22;

    long id = p.add_person(pers);
    cout << "Got id = " << id << endl;
    //usleep(5000 * 1000);

    dboost_test::person op = p.find_person_by_id(id);
    cout << "Got person: " << op.name << " " << op.family_name
         << " (" << op.birth_date.day << "/" << op.birth_date.month << "/"
         << op.birth_date.year << ")" << endl;


    return 0;
}
