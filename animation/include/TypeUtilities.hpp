#pragma once
#include <type_traits>
#include <utility>

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

template <typename ContainerType, typename AnimationPtrType, typename = std::void_t<>>
struct is_animation_container : std::false_type
{
};
template <typename ContainerType, typename AnimationPtrType>
struct is_animation_container<
    ContainerType, AnimationPtrType,
    std::void_t<decltype(std::declval<ContainerType>().begin()), decltype(std::declval<ContainerType>().end()),
                decltype(std::declval<ContainerType>().empty()), std::enable_if_t<has_member_value_type_v<std::decay_t<ContainerType>>>,
                std::enable_if_t<std::is_constructible_v<AnimationPtrType, typename std::decay_t<ContainerType>::value_type>>>>
    : std::true_type
{
};
template <typename ContainerType, typename AnimationPtrType>
inline constexpr bool is_animation_container_v = is_animation_container<ContainerType, AnimationPtrType>::value;

} // namespace detail
} // namespace animation
