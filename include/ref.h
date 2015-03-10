/*
 * ref.h
 *
 *  Created on: 06.03.2015
 *      Author: stas
 */

#ifndef REF_H_
#define REF_H_

#include <memory>

namespace dboost
{

template <typename T>
class ref
{
    template <typename U>
    static std::shared_ptr<T> get_refee(U* val, bool owned);
public:
    ref() = default;
    template<typename U>
    ref(U* val, bool owned = false)
        : m_refee(get_refee(val, owned)) {}
    ref(const ref& oth) = default;
    ~ref() = default;
    ref& operator= (const ref& oth) = default;
    T* operator&()
    {
        return m_refee.get();
    }
    T& get()
    {
        return *(m_refee.get());
    }

private:
    std::shared_ptr<T> m_refee;
};

template <typename T>
template <typename U>
inline std::shared_ptr<T> ref<T>::get_refee(U* val, bool owned)
{
    return owned ? std::shared_ptr<T>(val) : std::shared_ptr<T>(val, [](T*){});
}

} // namespace dboost

#endif /* REF_H_ */
