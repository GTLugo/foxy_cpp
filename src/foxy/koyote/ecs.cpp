//
// Created by galex on 3/31/2022.
//

#include "ecs.hpp"

namespace ff {
  Entity::Entity(ECSManager& ecs)
    : ecsManager_{&ecs} {
    Log::debug_e(R"(Creating entity "{}")", id_);
    ecsManager_->registerEntity(*this);
  }

  Entity::~Entity() {
    // kill();
  }

  void Entity::kill() {
    Log::debug_e(R"(Removing entity "{}")", id_);
    ecsManager_->removeEntity(*this);
  }

  const Entity::BitSet& Entity::components() const {
    //return bitSet_;
    return ecsManager_->components(*this);
  }

  void ECSManager::removeEntity(Entity& entity) {
    // Remove entity components
    for (auto&& [id, componentMap]: componentMaps_) {
      componentMap->onEntityRemoved(entity);
    }
    // Remove entity from systems
    for (auto& [id, system]: systems_) {
      auto itr{std::find(system->entities.begin(), system->entities.end(), entity)};
      if (itr != system->entities.end()) system->entities.erase(itr);
    }
  }

  void ECSManager::onEntityBitsetMutated(Entity& entity)  {
    for (auto& [id, system]: systems_) {
      if (FF_BIT_COMPARE_TO(entity.components(), system->components())) {
        system->entities.push_back(entity);
      } else {
        auto itr{std::find(system->entities.begin(), system->entities.end(), entity)};
        if (itr != system->entities.end()) system->entities.erase(itr);
      }
    }
  }

  void SystemBase::parallelFor(const std::function<void(Entity&)>& action) {
    std::vector<Shared<Job>> jobs{};
    i32 batch{static_cast<i32>(glm::ceil(static_cast<float>(entities.size()) / static_cast<float>(JobManager::workerCountMinusGameThread())))};
    i32 offset{0};
    i32 leftOver{static_cast<i32>(entities.size())};

    while (leftOver < batch) {
      jobs.emplace_back(new SystemJob{{entities.begin() + offset, entities.begin() + offset + batch}, action});
      offset += batch;
      leftOver -= batch;
    }
    if (leftOver > 0) jobs.emplace_back(new SystemJob{{entities.begin() + offset, entities.end()}, action});

    JobManager::submit(jobs);

    for (auto& job: jobs) {
      job->wait();
    }
  }
}