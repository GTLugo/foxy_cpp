#include "foxy/macros.hpp"

import foxy;
import foxy.wait;

int main(int argCount = 0, char* args[] = nullptr) {
  foxy::App app{};
  app.run();
  foxy::wait_on_keypress();
}

#if defined(_WIN32) and defined(NDEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif