#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <memory>

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