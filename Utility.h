#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <optional>


const float EPSILON = std::numeric_limits<float>::epsilon();

bool equals(float x, float y);

std::string readTextFile(const std::string &file);

#endif // UTILITY_H
