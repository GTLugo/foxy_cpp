#include "foxy/foxy.hpp"

int main(int, char**) {
  foxy::App{foxy::Window::Properties{
      .title = "Foxy App"
  }}.run();
}

#if defined(_WIN32) and defined(NDEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif
