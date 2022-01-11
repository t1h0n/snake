#pragma once
#include "IAnimation.hpp"
#include "IAnimationManager.hpp"
#include <algorithm>
#include <atomic>
#include <deque>
namespace animation
{
template <typename DurationType = std::chrono::milliseconds, typename AnimationPointerType = std::unique_ptr<IAnimationImpl<DurationType>>>
class AnimationManagerImpl : public IAnimationManagerImpl<DurationType, AnimationPointerType>
{
public:
    AnimationManagerImpl() = default;
    virtual void cancelAnimationById(AnimationId const& id) override;
    [[nodiscard]] virtual ScopedAnimationIdImpl<DurationType, AnimationPointerType> addAnimation(AnimationPointerType animation) override;
    virtual AnimationId addAnimationRawId(AnimationPointerType animation) override;
    virtual void play(DurationType const& t) override;

    AnimationManagerImpl(const AnimationManagerImpl&) = delete;
    void operator=(const AnimationManagerImpl&) = delete;

protected:
    static AnimationId createUniqueAnimationId();
    struct AnimationWithId
    {
        AnimationWithId(AnimationId const& id, AnimationPointerType&& ptr) : id{id}, ptr{std::move(ptr)} {}
        AnimationId id;
        AnimationPointerType ptr;
    };
    std::deque<AnimationWithId> m_AnimationList;
};

template <typename DurationType, typename AnimationPointerType>
inline void AnimationManagerImpl<DurationType, AnimationPointerType>::cancelAnimationById(AnimationId const& id)
{
    const auto animation_to_be_removed =
        std::find_if(m_AnimationList.cbegin(), m_AnimationList.cend(),
                     [id](typename decltype(m_AnimationList)::value_type const& animation) { return animation.id == id; });
    if (animation_to_be_removed != m_AnimationList.cend())
    {
        animation_to_be_removed->ptr->invokeFinishedCallback();
        m_AnimationList.erase(animation_to_be_removed);
    }
}
template <typename DurationType, typename AnimationPointerType>
inline ScopedAnimationIdImpl<DurationType, AnimationPointerType>
AnimationManagerImpl<DurationType, AnimationPointerType>::addAnimation(AnimationPointerType animation)
{
    assert(animation);
    m_AnimationList.emplace_back(createUniqueAnimationId(), std::move(animation));
    return {m_AnimationList.back().id, this};
}
template <typename DurationType, typename AnimationPointerType>
inline AnimationId AnimationManagerImpl<DurationType, AnimationPointerType>::addAnimationRawId(AnimationPointerType animation)
{
    assert(animation);
    m_AnimationList.emplace_back(createUniqueAnimationId(), std::move(animation));
    return m_AnimationList.back().id;
}
template <typename DurationType, typename AnimationPointerType>
inline void AnimationManagerImpl<DurationType, AnimationPointerType>::play(DurationType const& t)
{
    for (auto& animation_with_id : m_AnimationList)
    {
        animation_with_id.ptr->play(t);
    }
    const auto new_end =
        std::remove_if(m_AnimationList.begin(), m_AnimationList.end(),
                       [](typename decltype(m_AnimationList)::value_type const& animation) { return animation.ptr->isFinished(); });
    std::for_each(new_end, m_AnimationList.end(),
                  [](typename decltype(m_AnimationList)::value_type const& animation) { animation.ptr->invokeFinishedCallback(); });
    m_AnimationList.erase(new_end, m_AnimationList.end());
}

template <typename DurationType, typename AnimationPointerType>
inline AnimationId AnimationManagerImpl<DurationType, AnimationPointerType>::createUniqueAnimationId()
{
    static constexpr AnimationId INITIAL_VALUE = static_cast<AnimationId>(5000);
    static std::atomic<AnimationId> counter{INITIAL_VALUE};
    return counter++;
}
using AnimationManager = AnimationManagerImpl<>;
} // namespace animation