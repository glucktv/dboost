/*
 * exception.h
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <stdexcept>
#include <dbus/dbus.h>

#define DBOOST_CHECK(x) dboost_check(x, #x, __FILE__, __LINE__)
#define DBOOST_CHECK_WITH_ERR(x, err) if (!x) throw communication_error(err);

namespace dboost
{

struct error: public DBusError
{
    error();
    virtual ~error() noexcept;

    error& operator= (const error&) = delete;
    error(const error&) = delete;
};

struct communication_error: public std::runtime_error
{
	communication_error(const error& e);
};

class exception: public std::runtime_error
{
public:
    exception(const std::string& msg1, const std::string& msg2, const char* file, int line);
    exception(const std::string& what): std::runtime_error(what) {}
};

inline communication_error::communication_error(const error& e)
    : std::runtime_error(std::string("Error: ") +
                         e.name +
                         " (message: " +
                         e.message + ")")
{
}

inline error::error()
{
    dbus_error_init(this);
}

inline error::~error()
{
    dbus_error_free(this);
}

template <typename T>
T dboost_check(T r, const char* call, const char* file, int line)
{
    if (!r)
        throw exception("Error calling", call, file, line);
    else return r;
}

} // namespace dboost

#endif /* EXCEPTION_H_ */
