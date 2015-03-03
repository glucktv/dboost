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
#include <iostream>

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
        std::clog << __PRETTY_FUNCTION__ << std::endl;
        if (dbus_message_iter_get_arg_type(&is) != type_traits<T>::type) {
            throw exception("Wrong parameter");
        }
        dbus_message_iter_get_basic(&is, &val);
        dbus_message_iter_next(&is);
    }
    static void do_serialize(oserializer& os, T& val)
    {
        std::clog << __PRETTY_FUNCTION__ << std::endl;
        DBOOST_CHECK(dbus_message_iter_append_basic(&os, type_traits<T>::type, &val));
    }

    static void do_serialize(type_collector& tc, const T& val)
    {
        tc << static_cast<char>(type_traits<T>::type);
    }
};

template <>
struct serialization_strategy<std::string, true>
{
    static void do_serialize(iserializer& is, std::string& val)
    {
        std::clog << __PRETTY_FUNCTION__ << std::endl;
        if (dbus_message_iter_get_arg_type(&is) != type_traits<std::string>::type) {
            throw exception("Wrong parameter");
        }
        const char* ptr;
        dbus_message_iter_get_basic(&is, &ptr);
        val = ptr;
        dbus_message_iter_next(&is);
    }
    static void do_serialize(oserializer& os, std::string& val)
    {
        std::clog << __PRETTY_FUNCTION__ << std::endl;
        auto ptr = val.c_str();
        DBOOST_CHECK(dbus_message_iter_append_basic(&os, type_traits<std::string>::type, &ptr));
    }
    static void do_serialize(type_collector& tc, const std::string& val)
    {
        tc << static_cast<char>(type_traits<std::string>::type);
    }

};

//TODO: specialize serialization_strategy for vectors and arrays

template <typename T>
struct serialization_strategy<std::vector<T>, false>
{
    static void do_serialize(iserializer& is, std::vector<T>& val)
    {
        std::clog << __PRETTY_FUNCTION__ << std::endl;
        if (dbus_message_iter_get_arg_type(&is) != DBUS_TYPE_ARRAY) {
            throw exception("Wrong parameter");
        }
        subserializer<iserializer> ss(is);
        while (dbus_message_iter_has_next(&ss)) {
            T record;
            ss & record;
            val.push_back(record);
            dbus_message_iter_next(&ss);
        }
    }

    static void do_serialize(oserializer& os, std::vector<T>& val)
    {
        // we don't neet to add 'a' prefix as it is added automatically
        type_collector c;
        c & *static_cast<T*>(0);
        auto signature = c.str();
        std::cout << "Signature is " << signature << std::endl;
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

template <typename A, typename T>
A& operator&(A& a, T& t)
{
    typedef typename remove_const<T>::type type;
    serialization_strategy<type, type_traits<type>::is_integral>::do_serialize(a, const_cast<type&>(t));
    return a;
}

}

#endif /* SERIALIZER_H_ */
