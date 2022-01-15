#pragma once
#include "CAnimation.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
namespace animation
{

template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class InfiniteLoopAnimationImpl : public IAnimationImpl<DurationType>
{

public:
    template <typename AnimationPtr>
    explicit InfiniteLoopAnimationImpl(AnimationPtr&& animation_ptr) : m_LoopedAnimation{std::forward<AnimationPtr>(animation_ptr)}
    {
        assert(static_cast<bool>(animation_ptr));
    }
    virtual void play(DurationType const& t) override
    {
        m_LoopedAnimation->play(t);
        if (m_LoopedAnimation->isFinished())
        {
            m_LoopedAnimation->reset();
        }
    }
    virtual void invokeFinishedCallback() const override
    {
        if (m_FinishedCallback)
        {
            m_FinishedCallback();
        }
    }
    virtual bool isFinished() const override { return false; }
    virtual void setFinishedCallback(const std::function<void()>& callback) override
    {
        assert(callback);
        m_FinishedCallback = callback;
    }
    virtual void reset() override { throw std::logic_error("Infinite loop animation shall not be reset!"); }

protected:
    AnimationPointerType m_LoopedAnimation;
    std::function<void()> m_FinishedCallback;
};

using InfiniteLoopAnimation = InfiniteLoopAnimationImpl<>;

template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class FiniteLoopAnimationImpl : public CAnimation<DurationType>
{

public:
    template <typename AnimationPtr>
    explicit FiniteLoopAnimationImpl(AnimationPtr&& animation_ptr, std::int32_t times_to_loop)
        : m_LoopedAnimation{std::forward<AnimationPtr>(animation_ptr)}, m_CurrentTimesLooped{0}, m_RequiredTimesLooped{times_to_loop}
    {
        assert(static_cast<bool>(animation_ptr) && times_to_loop > 1);
    }

protected: // methods
    virtual void play_impl(DurationType const& t) override
    {
        m_LoopedAnimation->play(t);
        if (m_LoopedAnimation->isFinished())
        {
            m_LoopedAnimation->reset();
            if (++m_CurrentTimesLooped == m_RequiredTimesLooped)
            {
                CAnimation<DurationType>::m_Finished = true;
            }
        }
    }
    virtual void reset_impl() override { m_CurrentTimesLooped = 0; }

protected: // members
    AnimationPointerType m_LoopedAnimation;
    std::function<void()> m_FinishedCallback;
    std::int32_t m_CurrentTimesLooped;
    std::int32_t m_RequiredTimesLooped;
};
using FiniteLoopAnimation = FiniteLoopAnimationImpl<>;
} // namespace animation