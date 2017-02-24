#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct Camera
{
    glm::vec3 position;
    glm::vec3 orientation; // Yaw, pitch, roll

    glm::mat4 orientationMatrix() const;
    glm::vec3 forwardDirection() const;
    glm::vec3 upDirection() const;
    glm::vec3 lookingAt() const;
};

#endif // CAMERA_H
