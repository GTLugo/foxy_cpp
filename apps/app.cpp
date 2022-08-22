#include "foxy/foxy.hpp"

int main(int argCount = 0, char* args[] = nullptr) {
  try {
    foxy::App app{};
    app.add_global_data()
        .add_step_before()
        .add_system_to_step()
        .run();
  } catch (const std::exception& e) {
    std::cerr << e.what();
  }
}

#if defined(_WIN32) and defined(NDEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif
