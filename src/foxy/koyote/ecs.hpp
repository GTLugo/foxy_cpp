//
// Created by galex on 3/31/2022.
//
// Sources
// https://austinmorlan.com/posts/entity_component_system/
// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/
// https://skypjack.github.io/2019-02-14-ecs-baf-part-1/
// https://www.youtube.com/watch?v=W3aieHjyNvw

#pragma once

#include "core/ecs/components/component.hpp"
#include "core/threading/concurrent_queue.hpp"
#include "core/threading/job_system.hpp"

namespace ff {
  struct Entity {
    friend class ECSManager;
  public:
    using ID = UUID;
    using BitSet = std::bitset<ComponentBase::maxComponents>;

    explicit Entity(ECSManager& ecsManager);

    ~Entity();

    void kill();

    [[nodiscard]] ID id() const { return id_; }

    [[nodiscard]] const Entity::BitSet& components() const;

    template<class C, typename... Args>
    Entity& add(Args&& ...args);

    template<class C>
    Entity& remove();

    template<class C>
    C& getRef();

    template<class C>
    const C& get() const;

    template<class C, typename... Args>
    void set(Args&& ...args);

    template<typename T, typename... Args>
    bool has() {
      auto& comps{components()};
      return ((comps.test(Component<T>::id())) && ... && (comps.test(Component<Args>::id())));
    }

    bool operator<(const Entity& rhs) const { return id_ < rhs.id_; }
    bool operator==(const Entity& rhs) const { return id_ == rhs.id_; }

  protected:
    ID id_{};
    // This is stored as a pointer because shallow copying is intentional, as it preserves
    // which ecsManager is the one who owns the entity.
    ECSManager* ecsManager_;
  };
}


// Allow hashing
namespace std {
  template<>
  struct hash<ff::Entity> {
    std::size_t operator()(const ff::Entity& entity) const {
      return boost::hash<boost::uuids::uuid>()(entity.id());
    }
  };
}

namespace ff {
  class SystemBase {
    using SystemAction = std::function<void(Entity&)>;

    struct SystemJob : Job {
      std::vector<Entity> entities{};
      SystemAction action{};

      explicit SystemJob(std::vector<Entity> entities_, SystemAction action_)
          : entities{std::move(entities_)}, action{std::move(action_)} {}

      void execute() override {
        for (auto& entity : entities) { action(entity); }
      }
    };

  public:
    std::vector<Entity> entities{};

    virtual ~SystemBase() = default;

    [[nodiscard]] const Entity::BitSet& components() const { return bitSet_; }

    virtual void onUpdate() = 0;

    //TODO: Change algorithm to precalculating division start iterators and then distribute work from there.
    // This would avoid the issue of copying the entity vectors everywhere.
    void parallelFor(const std::function<void(Entity&)>& action);

  protected:
    Entity::BitSet bitSet_{};
  };

  template<class FirstComponent, class... Components>
  class System : public SystemBase {
  public:
    System() {
      // https://www.fluentcpp.com/2021/03/12/cpp-fold-expressions/
      (bitSet_.set(Component<FirstComponent>::id(), true), ..., bitSet_.set(Component<Components>::id(), true));
    }
  };

  class ECSManager {
    class ComponentMapBase {
    public:
      virtual ~ComponentMapBase() = default;
      virtual void onEntityRemoved(Entity& entity) = 0;
    };

    template<class C>
    class ComponentMap : public ComponentMapBase {
    public:
      void add(Entity& entity, C component) {
        FF_ASSERT_E(!entityToComponent_.contains(entity.id()), "Attempted component override upon existent data.");
        Log::debug_e(R"(Adding component "{0}" to entity "{1}")", typeid(C).name(), entity.id());

        entityToComponent_[entity.id()] = components_.size();
        componentToEntity_[components_.size()] = entity.id();
        components_.push_back(std::move(component));
      }

      void remove(Entity& entity) {
        FF_ASSERT_E(entityToComponent_.contains(entity.id()), "Attempted component removal upon non-existent data.");
        Log::debug_e(R"(Removing component "{0}" from entity "{1}")", typeid(C).name(), entity.id());

        // retrieve IDs
        size_t indexTarget{entityToComponent_[entity.id()]};
        size_t indexLast{components_.size() - 1};
        Entity::ID entityLast{componentToEntity_[indexLast]};

        // move target to end of map
        std::iter_swap(components_.begin() + indexTarget, components_.begin() + indexLast);
        entityToComponent_[entityLast] = indexTarget;
        componentToEntity_[indexTarget] = entityLast;

        // pop end of map
        entityToComponent_.erase(entity.id());
        componentToEntity_.erase(indexLast);
        components_.pop_back();
      }

      C& data(Entity& entity) {
        //Log::debug_e(R"(Accessing component "{0}" on entity "{1}")", typeid(C).name(),
        //                     (entity.components().test(Component<Name>::id())) ? entity.component<Name>().name : to_string(entity.id()));
        FF_ASSERT_E(entityToComponent_.contains(entity.id()), "Attempted component access of non-existent data.");
        return components_[entityToComponent_[entity.id()]];
      }

      const C& data(const Entity& entity) const {
        //Log::debug_e(R"(Accessing component "{0}" on entity "{1}")", typeid(C).name(),
        //                     (entity.components().test(Component<Name>::id())) ? entity.component<Name>().name : to_string(entity.id()));
        FF_ASSERT_E(entityToComponent_.contains(entity.id()), "Attempted component access of non-existent data.");
        return components_[entityToComponent_.at(entity.id())];
      }

      void onEntityRemoved(Entity& entity) override {
        if (entityToComponent_.contains(entity.id())) remove(entity);
      }

    private:
      std::vector<C> components_{};
      std::unordered_map<Entity::ID, size_t> entityToComponent_{};
      std::unordered_map<size_t, Entity::ID> componentToEntity_{}; // for removing entity data
    };
  public:
    ECSManager() = default;
    ~ECSManager() = default;

    //Entity createEntity() { return makeShared<Entity_>(this); }
    void registerEntity(Entity& entity) { bitSetMap_[entity.id()] = {}; }
    // Maybe automate with Shared<>
    void removeEntity(Entity& entity);

    void onEntityBitsetMutated(Entity& entity);

    template<class C>
    void registerComponent() {
      FF_ASSERT_E(!componentRegistered<C>(), "Attempted duplicate component registration.");
      FF_ASSERT_E(ComponentBase::componentCount() <= ComponentBase::maxComponents, "Maximum component count reached.");
      componentMaps_.insert(std::make_pair(Component<C>::id(), makeShared<ComponentMap<C>>()));
    }

    template<class C>
    bool tryRegisterComponent() {
      if (!componentRegistered<C>() && ComponentBase::componentCount() <= ComponentBase::maxComponents) {
        componentMaps_.insert(std::make_pair(Component<C>::id(), makeShared<ComponentMap<C>>()));
        return true;
      }
      return false;
    }

    template<class C, typename... Args>
    void addComponent(Entity& entity, Args&& ...args) {
      if (!componentRegistered<C>()) registerComponent<C>();
      componentMap<C>()->add(entity, std::move(C{args...}));
      bitSetMap_[entity.id()].set(Component<C>::id());
      onEntityBitsetMutated(entity);
    }

    template<class C>
    void removeComponent(Entity& entity) {
      FF_ASSERT_E(componentRegistered<C>(), "Attempted removal of unregistered component.");
      componentMap<C>()->remove(entity);
      bitSetMap_[entity.id()].reset(Component<C>::id());
      onEntityBitsetMutated(entity);
    }

    template<class C>
    C& component(Entity& entity) {
      FF_ASSERT_E(componentRegistered<C>(), "Attempted access of unregistered component.");
      return componentMap<C>()->data(entity);
    }

    template<class C>
    const C& component(const Entity& entity) const {
      FF_ASSERT_E(componentRegistered<C>(), "Attempted access of unregistered component.");
      return componentMap<C>()->data(entity);
    }

    [[nodiscard]] const Entity::BitSet& components(const Entity& entity) const {
      return bitSetMap_.at(entity.id());
    }

    template<Derives<SystemBase> S>
    void registerSystem() {
      std::string name{typeid(S).name()};
      FF_ASSERT_E(!systems_.contains(name), "Attempted duplicate system registration.");
      systems_.insert({name, makeShared<S>()});
    }

    template<Derives<SystemBase> S>
    void executeSystem() {
      std::string name{typeid(S).name()};
      FF_ASSERT_E(systems_.contains(name), "Attempted execution of unregistered system.");
      systems_[name]->onUpdate();
    }

//    void executeSystems() {
//      for (auto& [id, system] : systems_) {
//        system->onUpdate();
//      }
//    }

  private:
    std::unordered_map<Entity::ID, Entity::BitSet> bitSetMap_{};
    std::unordered_map<ComponentBase::ID, Shared<ComponentMapBase>> componentMaps_{};
    std::unordered_map<std::string, Shared<SystemBase>> systems_{};

    template<class C>
    Shared<ComponentMap<C>> componentMap() {
      FF_ASSERT_E(componentRegistered<C>(), "Attempted access of unregistered component.");
      return std::static_pointer_cast<ComponentMap<C>>(componentMaps_[Component<C>::id()]);
    };

    template<class C>
    Shared<const ComponentMap<C>> componentMap() const {
      FF_ASSERT_E(componentRegistered<C>(), "Attempted access of unregistered component.");
      return std::const_pointer_cast<const ComponentMap<C>>(
          std::static_pointer_cast<ComponentMap<C>>(componentMaps_.at(Component<C>::id()))
      );
    };

    template<class C>
    bool componentRegistered() {
      return componentMaps_.contains(Component<C>::id());
    }
  };


  template<class C, typename... Args>
  Entity& Entity::add(Args&& ...args) {
    ecsManager_->template addComponent<C>(*this, std::forward<Args>(args)...);
    return *this;
  }

  template<class C>
  Entity& Entity::remove() {
    ecsManager_->template removeComponent<C>(*this);
    return *this;
  }

  template<class C>
  C& Entity::getRef() {
    return ecsManager_->template component<C>(*this);
  }

  template<class C>
  const C& Entity::get() const {
    return ecsManager_->template component<C>(*this);
  }

  template<class C, typename... Args>
  void Entity::set(Args&& ...args) {
    ecsManager_->template component<C>(*this) = std::move(C{args...});
  }
}

