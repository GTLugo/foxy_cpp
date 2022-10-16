#include "entity.hpp"

namespace fx {
  Entity::Entity()
    // entity_base_{ std::make_shared<EntityBase>() }
  {
    FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to create Entity while EntityCoordinator was null.");
    EntityCoordinator::current()->register_entity(id_);
    ref_count_ = new ReferenceCount{};
    ++*ref_count_;
    
    #ifdef FOXY_DEBUG_MODE
    if (has<Name>()) {
      Log::info("CTOR DEFAULT | \"{}\" ref count: {}", get<Name>().value, static_cast<u64>(ref_count_->count()));
    }
    #endif
  }
  
  Entity::Entity(const std::string& name):
    Entity{}
  {
    add<Name>(name);
    
    #ifdef FOXY_DEBUG_MODE
    Log::info("CTOR NAMED | \"{}\" ref count: {}", get<Name>().value, static_cast<u64>(ref_count_->count()));
    #endif
  }
  
  Entity::Entity(const Entity& rhs):
    id_{ rhs.id_ },
    ref_count_{ &++*rhs.ref_count_ }
  {
    #ifdef FOXY_DEBUG_MODE
    if (has<Name>()) {
      Log::info("CTOR COPY | \"{}\" ref count: {}", get<Name>().value, static_cast<u64>(ref_count_->count()));
    }
    #endif
  }
  
  Entity::~Entity()
  {
    if (ref_count_ != nullptr) {
      #ifdef FOXY_DEBUG_MODE
      // Debugging
      bool has_name{ has<Name>() };
      std::size_t count{ (--*ref_count_).count() };
      std::string name{ (has_name) ? get<Name>().value : "" };
      #endif
      
      if (ref_count_->count() <= 0) {
        delete ref_count_;
        FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to kill Entity while EntityCoordinator was null.");
        EntityCoordinator::current()->unregister_entity(id_);
        ref_count_ = nullptr;
      }
  
      #ifdef FOXY_DEBUG_MODE
      // Debugging
      if (has_name) {
        Log::info("DTOR {}| \"{}\" ref count: {}", (ref_count_ == nullptr) ? "FINAL " : "", name, static_cast<u64>(count));
      }
      #endif
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
    if (has<Name>()) {
      Log::trace("ASST COPY | \"{}\" ref count: {}", get<Name>().value, static_cast<u64>(ref_count_->count()));
    }
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