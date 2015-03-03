/*
 * database_utils.h
 *
 *  Created on: 02.03.2015
 *      Author: stas
 */

#ifndef DATABASE_UTILS_H_
#define DATABASE_UTILS_H_

#include <database.hpp>

namespace dboost_test
{

template <typename A>
A& serialize(A& a, date& p)
{
    return a & p.year & p.month & p.day;
}

template <typename A>
A& serialize(A& a, person& p)
{
    return a & p.family_name & p.name & p.birth_date;
}

};

#endif /* DATABASE_UTILS_H_ */
