#include <foxy/framework.hpp>
REDIRECT_WINMAIN_TO_MAIN

struct AppData {
  std::string waifu;
  int hololive_members;
};

void update(foxy::App& app) {
  std::shared_ptr<AppData> data = std::static_pointer_cast<AppData>(app.user_data());
  FOXY_DEBUG << "My favorite out of all " << data->hololive_members << " hololive members is " << data->waifu;
}

auto main(int, char**) -> int {
  using namespace foxy;

  try {
    Shared<AppData> app_data{
      std::make_shared<AppData>(
        AppData{
          .waifu = "Fubuki",
          .hololive_members = 71,
        }
      )
    };

    App{App::CreateInfo{
      .title = "Foxy Example App"
    }}.set_user_data(app_data)
      .add_function_to_stage(foxy::App::Stage::Tick, [](foxy::App& data){ update(data); })
      .add_stage_before()
      .run();

    return EXIT_SUCCESS;
  } catch (const std::exception& e) {
    FOXY_FATAL << "APP EXCEPTION: " << e.what();

    return EXIT_FAILURE;
  }
}