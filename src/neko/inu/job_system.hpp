//
// Created by galex on 3/24/2022.
//
// Sources:
// https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/
// https://benhoffman.tech/cpp/general/2018/11/13/cpp-job-system.html
// https://dens.website/articles/cpp-threadpool

#pragma once

#include <BS_thread_pool.hpp>

namespace foxy {
  // class JobSystem {
  // public:
  //   JobSystem();
  //   ~JobSystem();

  //   JobSystem(const JobSystem& other) = delete;
  //   JobSystem& operator=(const JobSystem& other) = delete;
  // private:
  //   class Impl;
  //   Unique<Impl> pImpl_;
  // };
}
