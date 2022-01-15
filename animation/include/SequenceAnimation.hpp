#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <iterator>
#include <memory>

namespace animation
{
template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class SequenceAnimationImpl : public CAnimation<DurationType>
{
public:
    SequenceAnimationImpl() : m_Start{m_AnimationList.begin()} {}
    template <typename... Args>
    explicit SequenceAnimationImpl(Args&&... args)
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
        if (m_Start != m_AnimationList.end())
        {
            auto& top_animation = *m_Start;
            top_animation->play(t);
            if (top_animation->isFinished())
            {
                top_animation->invokeFinishedCallback();
                ++m_Start;
            }
        }
        else
        {
            CAnimation<DurationType>::m_Finished = true;
        }
    }
    virtual void reset_impl() override
    {
        for (auto& animation : m_AnimationList)
        {
            animation->reset();
        }
        m_Start = m_AnimationList.begin();
    }
    template <typename T, typename... Args>
    void fillContainerFromVariadic(T&& first, Args&&... args)
    {
        static_assert(std::is_constructible_v<AnimationPointerType, decltype(first)>);
        assert(static_cast<bool>(first));
        m_AnimationList.push_back(std::forward<T>(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::forward<Args>(args)...);
        }
    }

protected: // members
    using InnerAnimationContainer = std::deque<AnimationPointerType>;

    InnerAnimationContainer m_AnimationList;
    typename InnerAnimationContainer::iterator m_Start;
};

using SequenceAnimation = SequenceAnimationImpl<>;
} // namespace animation
