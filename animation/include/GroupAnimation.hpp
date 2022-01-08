#pragma once
#include "CAnimation.hpp"
#include <deque>
#include <memory>

class GroupAnimation : public CAnimation
{
public:
    template <typename... Args>
    GroupAnimation(std::unique_ptr<Args>... args)
    {
        fillContainerFromVariadic(std::move(args)...);
    }
    virtual void play_impl(std::chrono::milliseconds ms) override
    {
        m_Finished = true;
        for (auto& animation : m_AnimationList)
        {
            animation->play(ms);
            m_Finished = m_Finished && animation->isFinished();
        }
    }

protected:
    template <typename T, typename... Args>
    void fillContainerFromVariadic(std::unique_ptr<T> first, std::unique_ptr<Args>... args)
    {
        m_AnimationList.push_back(std::move(first));
        if constexpr (sizeof...(Args) > 0)
        {
            fillContainerFromVariadic(std::move(Args)...);
        }
    }
    std::deque<std::unique_ptr<IAnimation>> m_AnimationList;
};