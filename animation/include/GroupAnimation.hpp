#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <memory>

namespace animation
{
template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class GroupAnimationImpl : public CAnimation<DurationType>
{
public:
    GroupAnimationImpl() = default;
    template <typename... Args>
    explicit GroupAnimationImpl(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    template <typename ContainerType>
    void addAnimationsFromContainer(ContainerType&& animation_container) // handle all cases
    {
        static_assert(detail::has_member_value_type_v<ContainerType>);
        static_assert(std::is_constructible_v<AnimationPointerType, typename ContainerType::value_type>);
        static_assert(!std::is_const_v<ContainerType> || std::is_copy_constructible_v<AnimationPointerType>);
        assert(!animation_container.empty());
        if constexpr (!std::is_copy_constructible_v<AnimationPointerType> ||
                      (std::is_rvalue_reference_v<ContainerType> && !std::is_const_v<ContainerType>))
        {
            for (std::move_iterator start{animation_container.begin()}, end{animation_container.end()}; start != end; ++start)
            {
                assert(*start);
                m_AnimationList.push_back(*start);
            }
        }
        else
        {
            for (const auto& animation : animation_container)
            {
                m_AnimationList.push_back(animation);
            }
        }
    }
    template <typename... Args>
    void addAnimations(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    virtual void play_impl(DurationType const& t) override
    {
        CAnimation<DurationType>::m_Finished = true;
        for (auto& animation : m_AnimationList)
        {
            animation->play(t);
            CAnimation<DurationType>::m_Finished = CAnimation<DurationType>::m_Finished && animation->isFinished();
        }
    }

protected:
    template <typename T, typename... Args>
    void fillContainerFromVariadic(T&& first, Args&&... args)
    {
        static_assert(std::is_constructible_v<AnimationPointerType, T>);
        assert(first);
        m_AnimationList.push_back(std::forward<T>(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::forward<Args>(args)...);
        }
    }
    std::deque<AnimationPointerType> m_AnimationList;
};

using GroupAnimation = GroupAnimationImpl<>;
} // namespace animation