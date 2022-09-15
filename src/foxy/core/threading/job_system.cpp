//
// Created by galex on 3/24/2022.
//

#include "job_system.hpp"

namespace foxy {
  class JobSystem::Impl {
  public:
    Impl() {
      
    }

    ~Impl() = default;

    Impl(const Impl& other) = delete;
    Impl& operator=(const Impl& other) = delete;
  private:
  
  };

  
  JobSystem::JobSystem()
    : pImpl_{std::make_unique<Impl>()} {}

  JobSystem::~JobSystem() = default;
}