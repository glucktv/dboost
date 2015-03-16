/*
 * serializer.h
 *
 *  Created on: 28.02.2015
 *      Author: stas
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <dbus/dbus.h>
#include <type_traits.h>
#include <string>
#include <exception.h>
#include <utils.h>
#include <sstream>

namespace dboost
{

struct iserializer: public DBusMessageIter
{
    iserializer() = default;
    iserializer(DBusMessage* msg);
};

struct oserializer: public DBusMessageIter
{
    oserializer() = default;
    oserializer(DBusMessage* msg);
};

template <typename T>
struct subserializer;

template <>
struct subserializer<iserializer>: public iserializer
{
    // constructor for structs
    subserializer(iserializer& is);
};

template <>
struct subserializer<oserializer>: public oserializer
{
    // constructor for structs
    subserializer(oserializer& os);
    // constructor for arrays
    subserializer(oserializer& os, const char* signature);
    ~subserializer();

private:
    oserializer& m_parent;
};

inline iserializer::iserializer(DBusMessage* msg)
{
    DBOOST_CHECK(dbus_message_iter_init(msg, this));
}

inline oserializer::oserializer(DBusMessage* msg)
{
    dbus_message_iter_init_append(msg, this);
}

inline subserializer<iserializer>::subserializer(iserializer& is)
{
    dbus_message_iter_recurse(&is, this);
}

inline subserializer<oserializer>::subserializer(oserializer& os)
    : m_parent(os)
{
    DBOOST_CHECK(dbus_message_iter_open_container(&os, DBUS_TYPE_STRUCT, nullptr, this));
}

inline subserializer<oserializer>::subserializer(oserializer& os, const char* signature)
    : m_parent(os)
{
    DBOOST_CHECK(dbus_message_iter_open_container(&os, DBUS_TYPE_ARRAY, signature, this));
}

inline subserializer<oserializer>::~subserializer()
{
    dbus_message_iter_close_container(&m_parent, this);
}

//
// removes constness from type
//
template <class C>
struct remove_const
{
  typedef C type;
};

template <class C>
struct remove_const<const C>
{
  typedef C type;
};

// this type is used to collect a line describing
// a type required for DBus to store vector of arbitrary
// type
// it also requires implementation of
// template <typename A>
// A& serialize(A& a, type& t);
// function
class type_collector: public std::ostringstream {};

//
// Default impl - for types that have corresponding
//
// template <typename A>
// A& serialize(A& a, type& t);
//
// free function (where A is archive type)
//
template <typename T, bool I>
struct serialization_strategy
{
    static void do_serialize(iserializer& is, T& val)
    {
        if (dbus_message_iter_get_arg_type(&is) != DBUS_TYPE_STRUCT) {
            throw exception("Wrong parameter");
        }
        subserializer<iserializer> ss(is);
        serialize(ss, val);
    }
    static void do_serialize(oserializer& os, const T& val)
    {
        subserializer<oserializer> ss(os);
        serialize(ss, const_cast<T&>(val));
    }

    static void do_serialize(type_collector& tc, const T& val)
    {
        tc << "(";
        serialize(tc, const_cast<T&>(val));
        tc << ")";
    }
};

// Integral types, supported by DBUS (specified in type_traits.h)
template <typename T>
struct serialization_strategy<T, true>
{
    static void do_serialize(iserializer& is, T& val)
    {
        if (dbus_message_iter_get_arg_type(&is) != type_traits<T>::id) {
            throw exception("Wrong parameter");
        }
        dbus_message_iter_get_basic(&is, &val);
        dbus_message_iter_next(&is);
    }
    static void do_serialize(oserializer& os, T& val)
    {
        DBOOST_CHECK(dbus_message_iter_append_basic(&os, type_traits<T>::id, &val));
    }

    static void do_serialize(type_collector& tc, const T& val)
    {
        tc << static_cast<char>(type_traits<T>::id);
    }
};

// string
template <>
struct serialization_strategy<std::string, true>
{
    static void do_serialize(iserializer& is, std::string& val)
    {
        if (dbus_message_iter_get_arg_type(&is) != type_traits<std::string>::id) {
            throw exception("Wrong parameter");
        }
        const char* ptr;
        dbus_message_iter_get_basic(&is, &ptr);
        val = ptr;
        dbus_message_iter_next(&is);
    }
    static void do_serialize(oserializer& os, std::string& val)
    {
        auto ptr = val.c_str();
        DBOOST_CHECK(dbus_message_iter_append_basic(&os, type_traits<std::string>::id, &ptr));
    }
    static void do_serialize(type_collector& tc, const std::string& val)
    {
        tc << static_cast<char>(type_traits<std::string>::id);
    }

};

// vector<T>
template <typename T>
struct serialization_strategy<std::vector<T>, false>
{
    static void do_serialize(iserializer& is, std::vector<T>& val)
    {
        if (dbus_message_iter_get_arg_type(&is) != DBUS_TYPE_ARRAY) {
            throw exception("Wrong parameter");
        }
        subserializer<iserializer> ss(is);
        while (dbus_message_iter_get_arg_type(&ss) != DBUS_TYPE_INVALID) {
            T record;
            ss & record;
            val.push_back(record);
            dbus_message_iter_next(&ss);
        }
    }

    static void do_serialize(oserializer& os, std::vector<T>& val)
    {
        // we don't need to add 'a' prefix as it is added automatically
        type_collector c;
        c & *static_cast<T*>(0);
        auto signature = c.str();
        //std::clog << "Signature is " << signature << std::endl;
        subserializer<oserializer> ss(os, signature.c_str());
        for (auto item: val) {
            ss & item;
        }
    }

    static void do_serialize(type_collector& tc, std::vector<T>& val)
    {
        tc << "a(";
        tc & *static_cast<T*>(0);
        tc << ")";
    }
};

//
// The following template derives whether a type is registered
// in type_traits.h as a DBus built-in type.
//
// usage: is_integral<dbus-type-id>::flag
//

template <int>
struct is_integral
{
    enum { flag = true };
};

template <>
struct is_integral<DBUS_TYPE_INVALID>
{
    enum { flag = false };
};

template <typename A, typename T>
A& operator&(A& a, T& t)
{
    typedef typename remove_const<T>::type type;
    const int type_id = type_traits<type>::id;
    const bool integral = is_integral<type_id>::flag;
    typedef serialization_strategy<type, integral> strategy;
    strategy::do_serialize(a, const_cast<type&>(t));
    return a;
}

}

#endif /* SERIALIZER_H_ */
