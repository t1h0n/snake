#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <iterator>
#include <memory>

template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class SequenceAnimationImpl : public CAnimation<DurationType>
{
public:
    SequenceAnimationImpl() = default;
    template <typename... Args>
    explicit SequenceAnimationImpl(Args&&... args)
    {
        fillContainerFromVariadic(std::forward<Args>(args)...);
    }
    template <typename ContainerType>
    void addAnimationsFromContainer(ContainerType&& animation_container)
    {
        assert(!animation_container.empty());
        for (std::move_iterator start{animation_container.begin()}, end{animation_container.end()}; start != end; ++start)
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
    virtual void play_impl(DurationType const& t) override
    {
        if (m_AnimationList.cbegin() != m_AnimationList.cend())
        {
            auto& top_animation = m_AnimationList.front();
            top_animation->play(t);
            if (top_animation->isFinished())
            {
                top_animation->invokeFinishedCallback();
                m_AnimationList.pop_front();
                CAnimation<DurationType>::m_Finished = m_AnimationList.empty();
            }
        }
        else
        {
            CAnimation<DurationType>::m_Finished = true;
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