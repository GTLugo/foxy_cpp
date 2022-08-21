#pragma once

#ifndef __SPECSTRINGS_STRICT_LEVEL
#define __SPECSTRINGS_STRICT_LEVEL 0
#endif

// use #define USE_ASIO in cpp files
#ifdef _WIN32
  // fix "WinSock.h has already been included" error
#ifdef USE_ASIO
  // Set the proper SDK version before including boost/Asio
#include <SDKDDKVer.h>
// Note boost/ASIO includes Windows.h. 
#include <boost/asio.hpp>
#else //  USE_ASIO
#include <Windows.h>
#endif //  USE_ASIO
#else // _WIN32
#ifdef USE_ASIO
#include <boost/asio.hpp>
#endif // USE_ASIO
#endif // USE_ASIO

//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/random.hpp>
#include <boost/nondet_random.hpp>

#include <foxy/macros.hpp>
#include <foxy/log_macros.hpp>
