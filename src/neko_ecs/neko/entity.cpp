#include "entity.hpp"

namespace fx {
  Entity::Entity()
    // entity_base_{ std::make_shared<EntityBase>() }
  {
    FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to create Entity while EntityCoordinator was null.");
    EntityCoordinator::current()->register_entity(id_);
    ref_count_ = new ReferenceCount{};
    ++*ref_count_;
    // Log::info("CTOR | Entity ref count: {}", static_cast<u64>(ref_count_->count()));
  }
  
  Entity::Entity(const std::string& name):
    Entity{}
  {
    add<Entity::Name>(name);
  }
  
  Entity::Entity(const Entity& rhs):
    id_{ rhs.id_ },
    ref_count_{ &++*rhs.ref_count_ }
  {
    // Log::info("COPY CTOR | Entity ref count: {}", static_cast<u64>(ref_count_->count()));
  }
  
  Entity::~Entity()
  {
    if (ref_count_ != nullptr) {
      --*ref_count_;
      // Log::info("DTOR | Entity ref count: {}", static_cast<u64>(ref_count_->count()));
      if (ref_count_->count() <= 0) {
        delete ref_count_;
        FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to kill Entity while EntityCoordinator was null.");
        EntityCoordinator::current()->unregister_entity(id_);
        ref_count_ = nullptr;
      }
    }
  }
  
  Entity& Entity::operator=(const Entity& rhs)
  {
    if (this != &rhs) {
      if (ref_count_ != nullptr && (++*ref_count_).count() == 0) {
        delete ref_count_;
        FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to kill Entity while EntityCoordinator was null.");
        EntityCoordinator::current()->unregister_entity(id_);
      }
      
      ref_count_ = &++*rhs.ref_count_;
      id_ = rhs.id_;
    }
    // Log::trace("COPY ASST | Entity ref count: {}", static_cast<u64>(ref_count_->count()));
    return *this;
  }
  
  shared<EntityCoordinator> EntityCoordinator::current_coordinator_{ std::make_shared<EntityCoordinator>() };

  void EntityCoordinator::set_current(const shared<EntityCoordinator>& coordinator) {
    current_coordinator_ = coordinator;
  }

  auto EntityCoordinator::current() -> shared<EntityCoordinator>&
  {
    return current_coordinator_;
  }

  EntityCoordinator::EntityCoordinator() = default;

  EntityCoordinator::~EntityCoordinator() = default;
  
  EntityBase::~EntityBase()
  {
    FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to kill Entity while EntityCoordinator was null.");
    EntityCoordinator::current()->unregister_entity(id);
  }
} // fx