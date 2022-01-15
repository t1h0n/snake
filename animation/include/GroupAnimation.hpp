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
    GroupAnimationImpl() : m_Start{m_AnimationList.begin()} {}
    template <typename... Args>
    explicit GroupAnimationImpl(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
        m_Start = m_AnimationList.begin();
    }
    template <typename ContainerType>
    void addAnimationsFromContainer(ContainerType&& animation_container)
    {
        static_assert(detail::is_animation_container_v<ContainerType, AnimationPointerType>);
        assert(!animation_container.empty());
        if constexpr (!std::is_copy_constructible_v<AnimationPointerType> ||
                      (std::is_rvalue_reference_v<decltype(animation_container)> &&
                       !std::is_const_v<typename std::remove_reference_t<ContainerType>>))
        {
            static_assert(!std::is_const_v<typename std::remove_reference_t<ContainerType>>);
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
        m_Start = m_AnimationList.begin();
    }
    template <typename... Args>
    void addAnimations(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
        m_Start = m_AnimationList.begin();
    }

protected: // methods
    virtual void play_impl(DurationType const& t) override
    {
        auto start = m_Start;
        auto end = m_AnimationList.end();
        bool first_unfinised_found = false;
        for (; start != end; ++start)
        {
            auto& animation = start->animation_ptr;
            animation->play(t);
            if (animation->isFinished())
            {
                if (!start->callback_invoked)
                {
                    start->callback_invoked = true;
                    animation->invokeFinishedCallback();
                }
            }
            else if (!first_unfinised_found)
            {
                first_unfinised_found = true;
                m_Start = start;
            }
        }
        CAnimation<DurationType>::m_Finished = !first_unfinised_found;
    }

    virtual void reset_impl() override
    {
        for (auto& animation : m_AnimationList)
        {
            animation.animation_ptr->reset();
            animation.callback_invoked = false;
        }
        m_Start = m_AnimationList.begin();
    }
    template <typename T, typename... Args>
    void fillContainerFromVariadic(T&& first, Args&&... args)
    {
        static_assert(std::is_constructible_v<AnimationPointerType, decltype(first)>);
        assert(first);
        m_AnimationList.push_back(std::forward<T>(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::forward<Args>(args)...);
        }
    }

protected: // members
    struct AnimationWithFlag
    {
        AnimationWithFlag(AnimationPointerType&& animation_ptr) : animation_ptr{std::move(animation_ptr)}, callback_invoked{false} {}
        AnimationWithFlag(const AnimationPointerType& animation_ptr) : animation_ptr{animation_ptr}, callback_invoked{false} {}
        AnimationPointerType animation_ptr;
        bool callback_invoked;
    };
    using InnerAnimationContainer = std::deque<AnimationWithFlag>;
    InnerAnimationContainer m_AnimationList;
    typename InnerAnimationContainer::iterator m_Start;
};

using GroupAnimation = GroupAnimationImpl<>;
} // namespace animation
