#pragma once


/*----------------------
  VENDOR LIBRARIES
----------------------*/
// use #define USE_ASIO in cpp files
#ifdef _WIN32
// fix "WinSock.h has already been included" error
#ifdef USE_ASIO
// Set the proper SDK version before including boost/Asio
#include <SDKDDKVer.h>
// Note boost/ASIO includes Windows.h.
#include <boost/asio.hpp>
#else //  USE_ASIO
#include <windows.h>
#endif //  USE_ASIO
#else // _WIN32
#ifdef USE_ASIO
#include <boost/asio.hpp>
#endif // USE_ASIO
#endif // _WIN32

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include <boost/functional/hash.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>

/*----------------------
  STD LIBRARY
----------------------*/
// IO
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
// Utilities
#include <compare>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <utility>
#include <functional>
#include <memory>
#include <ranges>
#include <future>
#include <ctime>
#include <chrono>
#include <random>
#include <stdexcept>
#include <limits>
#include <typeinfo>
#include <format>
#include <string_view>
// Threading
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <stop_token>
// Data Structures
#include <variant>
#include <bitset>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

/*----------------------
  FOXY LIBRARIES
----------------------*/
#include "foxy/macros.hpp"
#include "foxy/util/util.hpp"
#include "foxy/util/log.hpp"