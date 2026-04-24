//
// Created by qq103 on 2025/8/12.
//

#pragma once

class EntityId;

struct EntityIdTraits {
  using value_type = EntityId;

  using entity_type = unsigned int;
  using version_type = unsigned short;

  static constexpr entity_type entity_mask = 0x3FFFF;
  static constexpr entity_type version_mask = 0x3FFF;
};