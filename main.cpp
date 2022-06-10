#include "generator/generator.h"

using namespace exe::executors;

int main(int argc, char** argv) {
  if (argc == 1) {
    // paste yours directories
    SiteGenerator gen(
        "/home/h4zzkr/CLionProjects/gmi-html-translator/testing/example",
        "/home/h4zzkr/CLionProjects/gmi-html-translator/testing/"
        "result_example");
  } else {
    SiteGenerator gen(argv[1], argv[2]);
  }
  return 0;
}