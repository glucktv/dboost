/*
 * type_traits.h
 *
 *  Created on: 28.02.2015
 *      Author: stas
 */

#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

#include <dbus/dbus.h>

namespace dboost
{

template <typename T>
struct type_traits
{
    enum { type = DBUS_TYPE_INVALID };
    enum { is_integral = false };
};

#define TYPE(T, ID)                  \
    template <>                      \
    struct type_traits<T>            \
    {                                \
        enum { type = ID };          \
        enum { is_integral = true }; \
    };

TYPE(bool, DBUS_TYPE_BOOLEAN)
TYPE(int8_t,  DBUS_TYPE_BYTE)
TYPE(uint8_t, DBUS_TYPE_BYTE)
TYPE(int16_t, DBUS_TYPE_INT16)
TYPE(uint16_t, DBUS_TYPE_UINT16)
TYPE(int32_t, DBUS_TYPE_INT32)
TYPE(uint32_t, DBUS_TYPE_UINT32)
TYPE(int64_t, DBUS_TYPE_INT64)
TYPE(uint64_t, DBUS_TYPE_UINT64)
TYPE(double, DBUS_TYPE_DOUBLE)
TYPE(std::string, DBUS_TYPE_STRING)
//TYPE(std::vector, DBUS_TYPE_ARRAY)
#undef TYPE

}

#endif /* TYPE_TRAITS_H_ */
