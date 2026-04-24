//
// Created by qq103 on 2025/8/12.
//

#pragma once
#include "EntityRegistry.hpp"
#include "src/utils/optional_ref.hpp"

class EntityContext : public EnableGetWeakRef<EntityContext> {

public:
  [[nodiscard]] inline entt::basic_registry<EntityId> &getRegistry() noexcept {
    return mEnTTRegistry;
  }

  [[nodiscard]] inline entt::basic_registry<EntityId> const &
  getRegistry() const noexcept {
    return mEnTTRegistry;
  }

  template <class T>
  [[nodiscard]] inline optional_ref<T const> tryGetComponent() const {
    return getRegistry().try_get<T>(mEntity);
  }

  template <class T> [[nodiscard]] inline optional_ref<T> tryGetComponent() {
    return const_cast<T *>(std::as_const(getRegistry()).try_get<T>(mEntity));
  }

  template <class T> [[nodiscard]] inline bool hasComponent() const {
    return getRegistry().all_of<T>(mEntity);
  }

  template <class T> inline bool removeComponent() {
    return getRegistry().remove<T>(mEntity);
  }

  template <class T, typename... Args>
  [[nodiscard]] inline T &getOrAddComponent(Args &&...args) {
    return getRegistry().get_or_emplace<T>(mEntity,
                                           std::forward<Args>(args)...);
  }
  [[nodiscard]] bool isValid() const {
    // entt 库提供了安全且稳定的 API 来执行此检查。
    return mEnTTRegistry.valid(mEntity);
  }
  EntityContext(EntityRegistry &registry, EntityId entity)
      : mRegistry(registry), mEnTTRegistry(registry.mRegistry),
        mEntity(entity) {}
  EntityRegistry &mRegistry;
  entt::basic_registry<EntityId> &mEnTTRegistry;
  EntityId const mEntity;
};