#include "example_app.hpp"

auto main(const int, char**) -> int
{
  fx::Log::conditionally_enable_debug_backtrace();
  ExampleApp{}(); // there is also a standard .run() method if you prefer. Both are identical in functionality.
}
REDIRECT_WINMAIN_TO_MAIN