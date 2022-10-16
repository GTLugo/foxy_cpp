#pragma once

namespace fx {
  class Component {
  public:
    using ID = uuid;
  };

  template<class C>
  class ComponentID {
  public:
    [[nodiscard]] static auto value() -> const Component::ID& { return id_; }
  private:
    static const inline Component::ID id_{};
  };
} // fx