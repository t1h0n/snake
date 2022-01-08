#pragma once
#include "IAnimation.hpp"
#include "IAnimationManager.hpp"
#include <atomic>
#include <deque>
#include <memory>

class CAnimationManager : public IAnimationManager
{
public:
    void cancelAnimationById(AnimationId id) override;
    ScopedAnimationId addAnimation(std::unique_ptr<IAnimation> animation) override;
    AnimationId addAnimationRawId(std::unique_ptr<IAnimation> animation) override;
    void play(std::chrono::milliseconds ms) override;

    CAnimationManager(const CAnimationManager&) = delete;
    void operator=(const CAnimationManager&) = delete;

protected:
    static AnimationId createUniqueAnimationId();
    struct AnimationWithId
    {
        AnimationWithId(AnimationId id, std::unique_ptr<IAnimation>&& ptr) : id{id}, ptr{std::move(ptr)} {}
        AnimationId id;
        std::unique_ptr<IAnimation> ptr;
    };
    std::deque<AnimationWithId> m_AnimationList;
};