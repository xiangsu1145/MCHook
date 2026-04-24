//
// Created by qq103 on 2025/8/12.
//

#pragma once
#include "EntityId.hpp"
#include "EnableGetWeakRef.hpp"
#include <memory>
#include <string>

class EntityRegistry : public EnableGetWeakRef<::EntityRegistry>,
                       public std::enable_shared_from_this<::EntityRegistry> {
public:
  using Base = entt::basic_registry<EntityId>;

  using BaseWeak = EnableGetWeakRef<EntityRegistry>;

  using BaseShared = std::enable_shared_from_this<::EntityRegistry>;

  std::string mDebugName;
  entt::basic_registry<EntityId> mRegistry;
  unsigned mId;
};
