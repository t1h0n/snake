#pragma once
#include "IAnimation.hpp"
#include "IAnimationManager.hpp"
#include <algorithm>
#include <atomic>
#include <deque>

template <typename DurationType = std::chrono::milliseconds,
          typename AnimationPointerType = std::unique_ptr<IAnimation<DurationType>>>
class CAnimationManager : public IAnimationManager<DurationType, AnimationPointerType>
{
public:
    CAnimationManager() = default;
    void cancelAnimationById(AnimationId id) override;
    ScopedAnimationId<DurationType, AnimationPointerType> addAnimation(AnimationPointerType animation) override;
    AnimationId addAnimationRawId(AnimationPointerType animation) override;
    void play(DurationType t) override;

    CAnimationManager(const CAnimationManager&) = delete;
    void operator=(const CAnimationManager&) = delete;

protected:
    static AnimationId createUniqueAnimationId();
    struct AnimationWithId
    {
        AnimationWithId(AnimationId id, AnimationPointerType&& ptr) : id{id}, ptr{std::move(ptr)} {}
        AnimationId id;
        AnimationPointerType ptr;
    };
    std::deque<AnimationWithId> m_AnimationList;
};

template <typename DurationType, typename AnimationPointerType>
inline void CAnimationManager<DurationType, AnimationPointerType>::cancelAnimationById(AnimationId id)
{
    const auto animation_to_be_removed = std::find_if(
        m_AnimationList.cbegin(), m_AnimationList.cend(),
        [id](typename decltype(m_AnimationList)::value_type const& animation) { return animation.id == id; });
    if (animation_to_be_removed != m_AnimationList.cend())
    {
        animation_to_be_removed->ptr->invokeFinishedCallback();
        m_AnimationList.erase(animation_to_be_removed);
    }
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationId<DurationType, AnimationPointerType>
CAnimationManager<DurationType, AnimationPointerType>::addAnimation(AnimationPointerType animation)
{
    m_AnimationList.emplace_back(createUniqueAnimationId(), std::move(animation));
    return {m_AnimationList.back().id, this};
}
template <typename DurationType, typename AnimationPointerType>
inline AnimationId
CAnimationManager<DurationType, AnimationPointerType>::addAnimationRawId(AnimationPointerType animation)
{
    m_AnimationList.emplace_back(createUniqueAnimationId(), std::move(animation));
    return m_AnimationList.back().id;
}
template <typename DurationType, typename AnimationPointerType>
inline void CAnimationManager<DurationType, AnimationPointerType>::play(DurationType ms)
{
    for (auto& animation_with_id : m_AnimationList)
    {
        animation_with_id.ptr->play(ms);
    }
    const auto new_end = std::remove_if(
        m_AnimationList.begin(), m_AnimationList.end(),
        [](typename decltype(m_AnimationList)::value_type const& animation) { return animation.ptr->isFinished(); });
    std::for_each(new_end, m_AnimationList.end(), [](typename decltype(m_AnimationList)::value_type const& animation) {
        animation.ptr->invokeFinishedCallback();
    });
    m_AnimationList.erase(new_end, m_AnimationList.end());
}

template <typename DurationType, typename AnimationPointerType>
inline AnimationId CAnimationManager<DurationType, AnimationPointerType>::createUniqueAnimationId()
{
    static constexpr AnimationId INITIAL_VALUE = static_cast<AnimationId>(5000);
    static std::atomic<AnimationId> counter{INITIAL_VALUE};
    return counter++;
}