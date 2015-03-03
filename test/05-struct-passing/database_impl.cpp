/*
 * database_impl.cpp
 *
 *  Created on: 03.03.2015
 *      Author: stas
 */

#include "database_impl.h"
#include <iostream>

namespace dboost_test
{

database_impl::database_impl()
    : m_last_index(0)
{
}

long database_impl::add_person(const dboost_test::person& p)
{
    m_persons.push_back(p);
    return m_last_index++;
}

dboost_test::person database_impl::find_person_by_id(const long id) throw (dboost_test::id_not_found)
{
    return m_persons[id];
}

dboost_test::persons database_impl::find_by_family(const std::string& fn)
{
    dboost_test::persons result;
    for (auto person: m_persons) {
        if (person.family_name == fn) {
            result.push_back(person);
        }
    }
    std::clog << "Found " << result.size() << " persons with family name " << fn << "." << std::endl;
    return result;
}

} // namespace dboost_test
