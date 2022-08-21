#include "foxy/foxy_includes.hpp"

import foxy.app;
import foxy.log;

int main(int argCount = 0, char* args[] = nullptr) {
  foxy::App app{};
  app.run();
}

#if defined(_WIN32) and defined(NDEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif
