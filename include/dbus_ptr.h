/*
 * dbus_ptr.h
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#ifndef DBUS_PTR_H_
#define DBUS_PTR_H_

#include <algorithm>
#include "traits.h"

namespace dboost
{

template <typename T>
class dbus_ptr
{
    T* m_ptr;

public:
    dbus_ptr() noexcept: m_ptr(nullptr) {}
    dbus_ptr(const dbus_ptr* rhs) noexcept
        : m_ptr(rhs.m_ptr)
    {
        if (m_ptr) {
            dbus_traits<T>::ref(m_ptr);
        }
    }
    dbus_ptr(T* ptr, bool already_refed = false) noexcept
        : m_ptr(ptr)
    {
        if (m_ptr && !already_refed) {
            dbus_traits<T>::ref(m_ptr);
        }
    }
    ~dbus_ptr() noexcept
    {
        if (m_ptr) {
            dbus_traits<T>::unref(m_ptr);
        }
    }
    dbus_ptr& operator= (const dbus_ptr& rhs) noexcept
    {
        reset(rhs.m_ptr);
        return *this;
    }
    T* get() const noexcept
    {
        return m_ptr;
    }
    void swap(dbus_ptr& rhs) noexcept
    {
        std::swap(m_ptr, rhs.m_ptr);
    }

    void reset(T* rhs, bool already_refed = false) noexcept
    {
        dbus_ptr<T> p(rhs, already_refed);
        swap(p);
    }

    explicit operator bool() const noexcept
    {
        return m_ptr != NULL;
    }
};


} // namespace dboost

#endif /* DBUS_PTR_H_ */
