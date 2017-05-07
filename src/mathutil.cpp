#include "mathutil.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

namespace MathUtil
{
    glm::mat4 eulerRotationToMatrix(glm::vec3 eulerAngle)
    {
        float yaw = eulerAngle.x;
        float pitch = eulerAngle.y;
        float roll = eulerAngle.z;
        glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0.0f, 0.0f, 1.0f));
        return rollMat * pitchMat * yawMat;
    }
}
