#pragma once

namespace fx {
  class EntityCoordinator;
  class Entity;

  class SystemBase {
  private:
    using SystemAction = std::function<void(Entity&)>;
  public:
    virtual ~SystemBase() = default;

    virtual void execute() = 0;
    void operator()() { execute(); }
  private:
  };

  template<class FirstComponent, class... Components>
  class System : public SystemBase {
  public:
    System()
    {

    }

  private:

  };
} // fx