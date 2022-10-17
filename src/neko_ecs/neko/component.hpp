#pragma once

namespace fx {
  class Component {
  public:
    struct ComponentTag;
    using ID = strong_alias<uuid, ComponentTag>;
    
    template<class C>
    [[nodiscard]] static constexpr auto id() -> const ID&
    {
      static const ID id{};
      return id;
    }
  };

  namespace components {
    struct Name {
      std::string value;
    };

    struct Transform {
      vec3 position;
      vec3 rotation;
      vec3 scale;
    };

    struct Mesh {

    };
  }
} // fx

FOXY_UUID_HASH_IMPL(fx::Component::ID)

template<>
struct std::hash<std::set<fx::Component::ID>> {
  auto operator()(const std::set<fx::Component::ID>& component_ids) const noexcept -> std::size_t
  {
    std::size_t hash{};
    for (const auto& id: component_ids) {
      hash = hash ^ std::hash<uuids::uuid>{}(id);
    }
    return hash;
  }
};