module;
#ifdef _WIN32
  #include <conio.h>
#else
  #include <cstdio>
  #include <termios.h>
  #include <unistd.h>
  #include <fcntl.h>
#endif

export module foxy.wait;

import <iostream>;

namespace foxy {
  export bool poll_keyboard() {
  #ifdef _WIN32
    return _kbhit();
  #else
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
      ungetc(ch, stdin);
      return 1;
    }

    return 0;
  #endif
  }

  export void wait_on_keypress() {
    std::cout << "Press any key to continue...";
    while (!poll_keyboard());
    std::cout << '\n';
  }
}
