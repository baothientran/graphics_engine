#include <fstream>
#include <sstream>
#include "Utility.h"


std::string readTextFile(const std::string &file) {
    std::ifstream t(file);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}
