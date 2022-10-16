#pragma once

#include "component.hpp"
#include "system.hpp"

namespace fx {
  template<class T>
  class ComponentID;
  
  struct EntityBase {
    uuid id{};
    
    ~EntityBase();
  };

  class Entity {
  public:
    using ID = uuid;

    struct Name {
      std::string value;
    };

    Entity();
    Entity(const std::string& name);
    Entity(const Entity& rhs);
    ~Entity();

    [[nodiscard]] auto id() const -> const ID& { return id_; }

    template<class C, typename... Args>
    auto add(Args&& ...args) -> Entity&;

    template <class... Cs>
    void remove();

    template<class C>
    [[nodiscard]] auto get_ref() -> C&;

    template<class C>
    [[nodiscard]] auto get() const -> const C&;

    template<class C, typename... Args>
    void set(Args&& ...args);

    template<class... Cs>
    [[nodiscard]] auto has() const -> bool;
  
    Entity& operator=(const Entity& rhs);

    auto operator<(const Entity & rhs) const -> bool { return id_ < rhs.id_; }
    auto operator==(const Entity & rhs) const -> bool { return id_ == rhs.id_; }

  private:
    // shared<EntityBase> entity_base_;
    ID id_{};
    ReferenceCount* ref_count_{ nullptr };
  };

  class EntityCoordinator {
  public:
    static void set_current(const shared<EntityCoordinator>& coordinator);
    static auto current() -> shared<EntityCoordinator>&;

    EntityCoordinator();
    ~EntityCoordinator();

    void register_entity(const Entity::ID& entity)
    {
      if (!entities_.contains(entity)) {
        entities_.emplace(entity, std::vector<Component::ID>{});
      }
    }

    void unregister_entity(const Entity::ID& entity)
    {
      if (entities_.contains(entity)) {
        for (const auto& component: entities_.at(entity)) {
          components_.at(component)->erase(entity);
        }
        entities_.erase(entity);
      }
    }

    template<class C>
    void register_component()
    {
      components_.emplace(ComponentID<C>::value(), std::make_shared<packed_array<Entity::ID, C>>());
    }

    template<class C>
    auto try_register_component() -> bool
    {
      if (!components_.contains(ComponentID<C>::value())) {
        register_component<C>();
        return true;
      }
      return false;
    }

    template<class C, typename... Args>
    auto add(Entity& entity, Args&& ...args) -> Entity&
    {
      try {
        try_register_component<C>();
        FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to add to non-existent entity.");
        FOXY_ASSERT(!has<C>(entity), "Attempted to add duplicate component data.");
        entities_.at(entity.id()).push_back(ComponentID<C>::value());
        component_array<C>()->insert(entity.id(), std::move(C{FOXY_FWD(args)...}));
        set<C>(entity, FOXY_FWD(args)...);
      } catch (const std::exception& e) {
        Log::fatal(e.what());
      }
      return entity;
    }

    template<typename C>
    void remove(Entity& entity)
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to remove from non-existent entity.");
      FOXY_ASSERT(has<C>(entity), "Attempted to remove non-existent component data.");
      const Component::ID& component_id{ ComponentID<C>::value() };
      auto& components{ entities_.at(entity.id()) };
      components.erase(std::ranges::find(components, component_id));
      components_.at(component_id)->erase(entity.id());
    }

    template<class C>
    auto get_ref(Entity& entity) -> C&
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to access non-existent entity.");
      FOXY_ASSERT(has<C>(entity), "Attempted to access non-existent component data.");
      return component_array<C>()->get(entity.id());
    }

    template<class C>
    auto get(const Entity& entity) const -> const C&
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to access non-existent entity.");
      FOXY_ASSERT(has<C>(entity), "Attempted to access non-existent component data.");
      return component_array<C>()->get(entity.id());
    }

    template<class C, typename... Args>
    void set(Entity& entity, Args&& ...args)
    {
      get_ref<C>(entity) = std::move(C{FOXY_FWD(args)...});
    }

    template<typename... Cs>
    [[nodiscard]] auto has(const Entity& entity) const -> bool
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to access non-existent entity.");
      auto& components{ entities_.at(entity.id()) };
      return (... && (std::ranges::find(components, ComponentID<Cs>::value()) != components.end()));
    }

  private:
    static shared<EntityCoordinator> current_coordinator_;

    std::unordered_map<Entity::ID, std::vector<Component::ID>> entities_{}; // what components do entities have?
    std::unordered_map<Component::ID, shared<packed_array_t<Entity::ID>>> components_{}; // what data does each component have?
    std::unordered_map<std::string, shared<SystemBase>> systems_{};

    template<class C>
    auto component_array() -> shared<packed_array<Entity::ID, C>>
    {
      FOXY_ASSERT(components_.contains(ComponentID<C>::value()), "Attempted access of unregistered component.");
      const Component::ID& component_id{ ComponentID<C>::value() };
      return std::static_pointer_cast<packed_array<Entity::ID, C>>(components_.at(component_id));
    }

    template<class C>
    auto component_array() const -> shared<const packed_array<Entity::ID, C>>
    {
      FOXY_ASSERT(components_.contains(ComponentID<C>::value()), "Attempted access of unregistered component.");
      const Component::ID& component_id{ ComponentID<C>::value() };
      return std::static_pointer_cast<packed_array<Entity::ID, C>>(components_.at(component_id));
    }
  };

  template <class C, typename ... Args>
  auto Entity::add(Args&&... args) -> Entity&
  {
    return EntityCoordinator::current()->add<C>(*this, FOXY_FWD(args)...);
  }

  template <class... Cs>
  void Entity::remove()
  {
    EntityCoordinator::current()->remove<Cs...>(*this);
  }

  template <class C>
  auto Entity::get_ref() -> C&
  {
    return EntityCoordinator::current()->get_ref<C>(*this);
  }

  template <class C>
  auto Entity::get() const -> const C&
  {
    return EntityCoordinator::current()->get<C>(*this);
  }

  template <class C, typename... Args>
  void Entity::set(Args&&... args)
  {
    EntityCoordinator::current()->set<C>(*this, FOXY_FWD(args)...);
  }

  template <class... Cs>
  auto Entity::has() const -> bool
  {
    return EntityCoordinator::current()->has<Cs...>(*this);
  }
} // fx