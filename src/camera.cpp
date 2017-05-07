#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

glm::mat4 Camera::orientationMatrix() const
{
    float yaw = this->orientation.x;
    float pitch = this->orientation.y;
    float roll = this->orientation.z;
    glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rollMat = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0.0f, 0.0f, 1.0f));
    return rollMat * pitchMat * yawMat;
}

glm::vec3 Camera::forwardDirection() const
{
    glm::vec4 forward = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f); // Direction vector
    return glm::vec3(this->orientationMatrix() * forward);
}

glm::vec3 Camera::upDirection() const
{
    glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Direction vector
    return glm::vec3(this->orientationMatrix() * up);
}

glm::vec3 Camera::lookingAt() const
{
    return this->position + this->forwardDirection();
}
