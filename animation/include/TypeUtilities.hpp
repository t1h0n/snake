#pragma once

#include <type_traits>
namespace animation
{
namespace detail
{
#define DEFINE_HAS_MEMBER(Member)                                                                                                          \
    template <typename, typename = std::void_t<>>                                                                                          \
    struct has_member_##Member : std::false_type                                                                                           \
    {                                                                                                                                      \
    };                                                                                                                                     \
    template <typename T>                                                                                                                  \
    struct has_member_##Member<T, std::void_t<typename T::Member>> : std::true_type                                                        \
    {                                                                                                                                      \
    };                                                                                                                                     \
    template <class T>                                                                                                                     \
    inline constexpr bool has_member_##Member##_v = has_member_##Member<T>::value;

DEFINE_HAS_MEMBER(rep);
DEFINE_HAS_MEMBER(value_type);
DEFINE_HAS_MEMBER(period);
} // namespace detail
} // namespace animation
