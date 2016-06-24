#include <iostream>
#include <time.h>
#include "scene_lua.hpp"
int main(int argc, char** argv)
{
    time_t t1, t2;
    time(&t1);
  std::string filename = "Assets/simple.lua";
  if (argc >= 2) {
    filename = argv[1];
  }

  if (!run_lua(filename)) {
    std::cerr << "Could not open " << filename << std::endl;
    return 1;
  }
    time(&t2);
    std::cout << "Programs took " << difftime(t2, t1) << " seconds" << std::endl;
}
