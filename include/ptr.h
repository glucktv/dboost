/*
 * ptr.h
 *
 *  Created on: 06.03.2015
 *      Author: stas
 */

#ifndef PTR_H_
#define PTR_H_

#include <memory>

namespace dboost
{

template <typename T>
class ptr
{
    template <typename U>
    static std::shared_ptr<T> get_refee(U* val, bool owned);
public:
    ptr() = default;
    template<typename U>
    ptr(U* val, bool owned = false)
        : m_refee(get_refee(val, owned)) {}
    template<typename U>
    ptr(const std::shared_ptr<U> rhs)
        : m_refee(rhs) {}
    ptr& operator= (const ptr& rhs) = default;
    T& operator*() const
    {
        return *m_refee.get();
    }
    T* operator->() const
    {
        return m_refee.get();
    }
    T* get() const
    {
        return m_refee.get();
    }

private:
    std::shared_ptr<T> m_refee;
};

template <typename T>
template <typename U>
inline std::shared_ptr<T> ptr<T>::get_refee(U* val, bool owned)
{
    return owned ? std::shared_ptr<T>(val) : std::shared_ptr<T>(val, [](T*){});
}

} // namespace dboost

#endif /* PTR_H_ */
