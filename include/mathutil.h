#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace MathUtil
{
    glm::mat4 eulerRotationToMatrix(glm::vec3 orientation);
}

#endif // MATH_UTIL_H
