/*
 * database_impl.h
 *
 *  Created on: 03.03.2015
 *      Author: stas
 */

#ifndef DATABASE_IMPL_H_
#define DATABASE_IMPL_H_

#include "database.hpp"
#include <vector>

namespace dboost_test
{

class database_impl: public virtual database
{
public:
    database_impl();
    virtual long add_person(const dboost_test::person& p);
    virtual dboost_test::person find_person_by_id(const long id) throw (dboost_test::id_not_found);
    virtual dboost_test::persons find_by_family(const std::string& fn);

private:
    int m_last_index;
    std::vector<person> m_persons;
};

} // namespace dboost_test

#endif /* DATABASE_IMPL_H_ */
