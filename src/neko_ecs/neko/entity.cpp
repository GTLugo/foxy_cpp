#include "entity.hpp"

namespace fx {
  Entity::Entity()
  {
    FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to create Entity while EntityCoordinator was null.");
    EntityCoordinator::current()->register_entity_id(id_);
    ref_count_ = new ReferenceCount{};
    ++*ref_count_;
    
    #ifdef FOXY_DEBUG_MODE
    // Log::trace("CTOR DEFAULT | {}{} | ref count: {}", to_string(id_), has<components::Name>() ? ":\"" + get<components::Name>().value + "\"" : "", ref_count_->count());
    #endif
  }
  
  Entity::Entity(const std::string& name):
    Entity{}
  {
    add<components::Name>(name);
    
    #ifdef FOXY_DEBUG_MODE
    // Log::trace("CTOR NAMED | {}{} | ref count: {}", to_string(id_), has<components::Name>() ? ":\"" + get<components::Name>().value + "\"" : "", ref_count_->count());
    #endif
  }
  
  Entity::Entity(const Entity& rhs):
    id_{ rhs.id_ },
    ref_count_{ &++*rhs.ref_count_ }
  {
    #ifdef FOXY_DEBUG_MODE
    // Log::trace("CTOR COPY | {}{} | ref count: {}", to_string(id_), has<components::Name>() ? ":\"" + get<components::Name>().value + "\"" : "", ref_count_->count());
    #endif
  }
  
  Entity::Entity(Entity&& rhs) noexcept:
    Entity{}
  {
    using std::swap;
    swap(id_, rhs.id_);
    swap(ref_count_, rhs.ref_count_);
    
    #ifdef FOXY_DEBUG_MODE
    // Log::trace("MOVE CTOR | {}{} | ref count: {}", to_string(id_), has<components::Name>() ? ":\"" + get<components::Name>().value + "\"" : "", ref_count_->count());
    #endif
  }
  
  Entity::~Entity()
  {
    if (ref_count_ != nullptr) {
      #ifdef FOXY_DEBUG_MODE
      // Debugging
      const bool has_name{ has<components::Name>() };
      std::size_t count{ (--*ref_count_).count() };
      const std::string name{ has_name ? get<components::Name>().value : "" };
      #endif
      
      if (ref_count_->count() <= 0) {
        delete ref_count_;
        FOXY_ASSERT(EntityCoordinator::current() != nullptr, "Attempted to kill Entity while EntityCoordinator was null.");
        EntityCoordinator::current()->unregister_entity_id(id_);
        ref_count_ = nullptr;
      }
  
      #ifdef FOXY_DEBUG_MODE
      // Debugging
      // Log::trace("DTOR {}| {}{} | ref count: {}", ref_count_ == nullptr ? "FINAL " : "", to_string(id_), has_name ? ":\"" + name + "\"" : "", count);
      #endif
    }
  }
  
  auto Entity::operator=(Entity rhs) -> Entity&
  {
    using std::swap;
    swap(id_, rhs.id_);
    swap(ref_count_, rhs.ref_count_);
    
    #ifdef FOXY_DEBUG_MODE
    // Log::trace("ASST COPY | {}{} | ref count: {}", to_string(id_), has<components::Name>() ? ":\"" + get<components::Name>().value + "\"" : "", ref_count_->count());
    #endif
    
    return *this;
  }
  
  auto Entity::operator=(Entity&& rhs) noexcept -> Entity& = default;
  
  shared<EntityCoordinator> EntityCoordinator::current_coordinator_{ std::make_shared<EntityCoordinator>() };

  void EntityCoordinator::set_current(const shared<EntityCoordinator>& coordinator) {
    current_coordinator_ = coordinator;
  }

  auto EntityCoordinator::current() -> shared<EntityCoordinator>&
  {
    return current_coordinator_;
  }

  EntityCoordinator::EntityCoordinator():
    archetypes_{ // create empty archetype (as a default)
      std::unordered_map{
        std::pair{
          std::set<Component::ID>{},
          Archetype{ std::set<Component::ID>{} }
        }
      }
    },
    empty_archetype_{ archetypes_.find({}) }
  {
    FOXY_ASSERT(archetypes_.contains(std::set<Component::ID>{}), "Empty archetype not found.");
  }

  /// TODO: Make EntityCoordinator no longer a global static. Perhaps move the reference back into Entity?
  EntityCoordinator::~EntityCoordinator() = default;
} // fx