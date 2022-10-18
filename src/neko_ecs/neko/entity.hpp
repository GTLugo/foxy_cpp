//
// Originally based on Flugel ECS, created by galex on 3/31/2022.
//
// Sources
// https://austinmorlan.com/posts/entity_component_system/
// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/
// https://skypjack.github.io/2019-02-14-ecs-baf-part-1/
// https://www.youtube.com/watch?v=W3aieHjyNvw
// https://ajmmertens.medium.com/building-an-ecs-1-where-are-my-entities-and-components-63d07c7da742

#pragma once

#include "component.hpp"
#include "system.hpp"

namespace fx {
  /**
   * \brief A handle to an entity in the ECS. This acts similar to a reference-counted pointer.
   *        Upon the last handle losing scope, the entity is killed. Copies should be cheap, as
   *        they are just a copy of the UUID and incrementation of the reference count.
   */
  class Entity {
  public:
    struct EntityTag;
    using ID = strong_alias<uuid, EntityTag>;
    
    Entity();
    
    Entity(const std::string& name);
    
    Entity(const Entity& rhs);
    
    Entity(Entity&& rhs) noexcept;
    
    ~Entity();
    
    [[nodiscard]] auto id() const -> const ID&
    { return id_; }
    
    template<class C, typename... Args>
    auto add(Args&& ...args) -> Entity&;
    
    template<class... Cs>
    void remove();
    
    template<class C>
    [[nodiscard]] auto get_ref() -> C&;
    
    template<class C>
    [[nodiscard]] auto get() const -> const C&;
    
    template<class C, typename... Args>
    void set(Args&& ...args);
    
    template<class... Cs>
    [[nodiscard]] auto has() const -> bool;
    
    auto operator=(Entity rhs) -> Entity&;
    auto operator=(Entity&& rhs) noexcept -> Entity&;
    
    auto operator<(const Entity& rhs) const -> bool
    { return id_ < rhs.id_; }
    
    auto operator==(const Entity& rhs) const -> bool
    { return id_ == rhs.id_; }
  
  private:
    ID id_{};
    ReferenceCount* ref_count_{ nullptr };
  };
} // fx

FOXY_UUID_HASH_IMPL(fx::Entity::ID)

namespace fx {
  class Archetype {
  public:
    struct ArchetypeTag;
    using ID = strong_alias<uuid, ArchetypeTag>;

    explicit Archetype(std::set<Component::ID> components):
      components_{ std::move(components) }
    {}

    [[nodiscard]] auto id() const -> const ID&
    {
      return id_;
    }

    [[nodiscard]] auto components() const -> const std::set<Component::ID>&
    {
      return components_;
    }
  
    auto operator<(const Archetype& rhs) const -> bool
    {
      return id_ < rhs.id_;
    }
  
    auto operator==(const Archetype& rhs) const -> bool
    {
      return id_ == rhs.id_;
    }

  private:
    ID id_{};
    std::set<Component::ID> components_{};
  };
} // fx

FOXY_UUID_HASH_IMPL(fx::Archetype::ID)

namespace fx {
  class EntityCoordinator {
  public:
    using ArchetypeLookupMap = std::unordered_map<std::set<Component::ID>, Archetype>; // archetype storage and lookup
    using EntityArchetypeMap = std::unordered_map<Entity::ID, ArchetypeLookupMap::iterator>; // what archetype does an entity fall into?
    using ComponentArchetypeMap = std::unordered_map<Component::ID, std::set<Archetype::ID>>; // which archetype does a component have?
    using ComponentDataMap = std::unordered_map<Component::ID, shared<packed_array_t<Entity::ID>>>; // what data does each component have?
    using SystemMap = std::unordered_map<std::string, shared<SystemBase>>;
    
    static void set_current(const shared<EntityCoordinator>& coordinator);
    static auto current() -> shared<EntityCoordinator>&;

    EntityCoordinator();
    ~EntityCoordinator();

    void register_entity_id(const Entity::ID& entity_id)
    {
      // register the new entity with the empty archetype
      entities_.try_emplace(entity_id, empty_archetype_);
    }

    void unregister_entity_id(const Entity::ID& entity_id)
    {
      FOXY_ASSERT(entities_.contains(entity_id), "Attempted to erase non-existent entity.");
      
      if (entities_.contains(entity_id)) {
        for (const auto& component: entities_.at(entity_id)->second.components()) {
          component_arrays_.at(component)->erase(entity_id);
        }
        entities_.erase(entity_id);
      }
    }

    template<class C>
    void try_register_component()
    {
      const auto& component_id{ Component::id<C>() };
      component_archetypes_.try_emplace(component_id, std::set<Archetype::ID>{});
      component_arrays_.try_emplace(component_id, std::make_shared<packed_array<Entity::ID, C>>());
    }

    void set_archetype(const Entity& entity, const std::set<Component::ID>& components)
    {
      auto& current_archetype{ entities_.at(entity.id()) };
      // try to find the archetype for the new component set
      if (!archetypes_.contains(components)) {
        // register new archetype
        Archetype new_archetype{ components };
        archetypes_.try_emplace(components, new_archetype);

        // update components with new archetype
        for (const Component::ID& component : components) {
          std::set<Archetype::ID> new_archetype_set{ component_archetypes_.at(component) };
          new_archetype_set.insert(new_archetype.id());
          component_archetypes_.at(component) = new_archetype_set;
        }
      }
      current_archetype = archetypes_.find(components);
    }

    template<class C, typename... Args>
    auto add(Entity& entity, Args&& ...args) -> Entity&
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to add component to non-existent entity.");
      FOXY_ASSERT(!has<C>(entity), "Attempted to add duplicate component data.");

      try {
        try_register_component<C>();

        const auto& component_id_to_add{ Component::id<C>() };
        std::set new_component_set{ entities_.at(entity.id())->second.components() };
        new_component_set.insert(component_id_to_add);
        set_archetype(entity, new_component_set);

        // Set data for new component
        component_array<C>()->insert(entity.id(), std::move(C{ FOXY_FWD(args)... }));
        set<C>(entity, FOXY_FWD(args)...);
      } catch (const std::exception& e) {
        Log::fatal(e.what());
      }

      return entity;
    }

    template<typename C>
    void remove(Entity& entity)
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to remove component from non-existent entity.");
      FOXY_ASSERT(has<C>(entity), "Attempted to remove non-existent component data.");

      try {
        const Component::ID& component_id_to_remove{ Component::id<C>() };
        std::set new_component_set{ entities_.at(entity.id())->second.components() };
        new_component_set.erase(component_id_to_remove);
        set_archetype(entity, new_component_set);

        component_arrays_.at(component_id_to_remove)->erase(entity.id());
      } catch (const std::exception& e) {
        Log::fatal(e.what());
      }
    }

    template<class C>
    [[nodiscard]] auto get_ref(Entity& entity) -> C&
    {
      FOXY_ASSERT(entities_.contains(entity.id()), "Attempted to access non-existent entity.");
      FOXY_ASSERT(has<C>(entity), "Attempted to access non-existent component data.");
      
      return component_array<C>()->get(entity.id());
    }

    template<class C>
    [[nodiscard]] auto get(const Entity& entity) const -> const C&
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
      const auto& archetype{ entities_.at(entity.id())->second };
      return std::ranges::all_of(std::set<Component::ID>{ Component::id<Cs>()... }, [&](const Component::ID& component_id) {
        return component_archetypes_.contains(component_id) && component_archetypes_.at(component_id).contains(archetype.id());
      });
    }

  private:
    static shared<EntityCoordinator> current_coordinator_;
  
    EntityArchetypeMap entities_{}; // what archetype does an entity fall into?
    ArchetypeLookupMap archetypes_{}; // archetype storage and lookup
    ArchetypeLookupMap::iterator empty_archetype_; // this archetype will be accessed frequently as a default for new entities, so caching it is good.
    ComponentArchetypeMap component_archetypes_{}; // which archetype does a component have?
    ComponentDataMap component_arrays_{}; // what data does each component have?
    SystemMap systems_{};
    
    template<class C>
    auto component_array() -> shared<packed_array<Entity::ID, C>>
    {
      FOXY_ASSERT(component_arrays_.contains(Component::id<C>()), "Attempted access of unregistered component.");
      return std::static_pointer_cast<packed_array<Entity::ID, C>>(component_arrays_.at(Component::id<C>()));
    }

    template<class C>
    auto component_array() const -> shared<const packed_array<Entity::ID, C>>
    {
      FOXY_ASSERT(component_arrays_.contains(Component::id<C>()), "Attempted access of unregistered component.");
      return std::static_pointer_cast<packed_array<Entity::ID, C>>(component_arrays_.at(Component::id<C>()));
    }
  };

  template <class C, typename ... Args>
  auto Entity::add(Args&&... args) -> Entity&
  {
    return EntityCoordinator::current()->template add<C>(*this, FOXY_FWD(args)...);
  }

  template <class... Cs>
  void Entity::remove()
  {
    EntityCoordinator::current()->template remove<Cs...>(*this);
  }

  template <class C>
  auto Entity::get_ref() -> C&
  {
    return EntityCoordinator::current()->template get_ref<C>(*this);
  }

  template <class C>
  auto Entity::get() const -> const C&
  {
    return EntityCoordinator::current()->template get<C>(*this);
  }

  template <class C, typename... Args>
  void Entity::set(Args&&... args)
  {
    EntityCoordinator::current()->template set<C>(*this, FOXY_FWD(args)...);
  }

  template <class... Cs>
  auto Entity::has() const -> bool
  {
    return EntityCoordinator::current()->template has<Cs...>(*this);
  }
} // fx