#include "utils.h"
#include "database_proxy.h"
#include <iostream>

using namespace dboost;
using namespace std;

void print_person(const dboost_test::person& op)
{
    cout << "Got person: " << op.name << " " << op.family_name
         << " (" << op.birth_date.day << "/" << op.birth_date.month << "/"
         << op.birth_date.year << ")" << endl;
}

int main()
{
    dboost_test::database_proxy p(create_connection(), "org.dboost.structpassing", "/org/dboost/database/0");

    dboost_test::person pers;
    pers.name = "Heirich";
    pers.family_name = "Hertz";
    pers.birth_date.year = 1857;
    pers.birth_date.month = 2;
    pers.birth_date.day = 22;

    long id = p.add_person(pers);
    cout << "Got id = " << id << endl;

    dboost_test::person op = p.find_person_by_id(id);
    print_person(op);

    std::vector<dboost_test::person> pp = p.find_by_family("Hertz");
    cout << "There are " << pp.size() << " persons." << endl;
    for (auto person: pp)
        print_person(person);

    pp = p.find_by_family("Hertxz");
    cout << "There are " << pp.size() << " persons." << endl;
    for (auto person: pp)
        print_person(person);


    return 0;
}
