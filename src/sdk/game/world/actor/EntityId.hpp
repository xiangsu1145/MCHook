//
// Created by qq103 on 2025/8/12.
//

#pragma once
#include "EntityIdTraits.hpp"
#include "include/entt/entt.hpp"

template <>
class entt::entt_traits<EntityId>
    : public entt::basic_entt_traits<EntityIdTraits> {
public:
  static constexpr entity_type page_size = ENTT_SPARSE_PAGE;
};

class EntityId : public entt::entt_traits<EntityId> {
public:
  entity_type mRawId{};

  [[nodiscard]] constexpr EntityId() = default;

  [[nodiscard]] constexpr EntityId(entity_type rawId) : mRawId(rawId) {}

  [[nodiscard]] constexpr bool isNull() const { return *this == entt::null; }

  [[nodiscard]] constexpr operator entity_type() const { return mRawId; }
};