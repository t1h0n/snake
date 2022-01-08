#pragma once
#include "CAnimation.hpp"
#include <algorithm>
#include <deque>
#include <iterator>
#include <memory>

template <typename DurationType = std::chrono::milliseconds,
          typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class SequenceAnimationImpl : public CAnimation<DurationType>
{
public:
    SequenceAnimationImpl() = default;
    template <typename... Args>
    SequenceAnimationImpl(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    template <typename ContainerType>
    void addAnimationsFromContainer(ContainerType&& animation_container)
    {
        assert(!animation_container.empty());
        for (std::move_iterator start{animation_container.begin()}, end{animation_container.end()}; start != end;
             ++start)
        {
            assert(*start);
            m_AnimationList.push_back(*start);
        }
    }
    template <typename... Args>
    void addAnimations(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    virtual void play_impl(DurationType ms) override
    {
        auto& top_animation = m_AnimationList.front();
        top_animation->play(ms);
        if (top_animation->isFinished())
        {
            top_animation->invokeFinishedCallback();
            m_AnimationList.pop_front();
            CAnimation<DurationType>::m_Finished = m_AnimationList.empty();
        }
    }

protected:
    template <typename T, typename... Args>
    void fillContainerFromVariadic(T&& first, Args&&... args)
    {
        assert(first);
        m_AnimationList.push_back(std::forward<T>(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::forward<Args>(args)...);
        }
    }
    std::deque<AnimationPointerType> m_AnimationList;
};

using SequenceAnimation = SequenceAnimationImpl<>;