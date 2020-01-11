#include <fstream>
#include <sstream>
#include <glm/gtc/epsilon.hpp>
#include "Utility.h"


bool equals(float x, float y) {
    return glm::epsilonEqual(x, y, EPSILON);
}


std::string readTextFile(const std::string &file) {
    std::ifstream t(file);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}
