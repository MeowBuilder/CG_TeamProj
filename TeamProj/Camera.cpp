#include "Camera.h"

Camera::Camera(glm::vec3 startPos) :
    position(startPos),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)),
    yaw(-90.0f),
    pitch(0.0f),
    sensitivity(0.1f)
{
    UpdateCamera();
}

void Camera::UpdateCamera()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}

void Camera::SetMouseLook(float xoffset, float yoffset)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    UpdateCamera();
}

glm::mat4 Camera::GetViewMatrix() const
{
    glm::vec3 adjustedPosition = position + glm::vec3(0.0f, 2.0f, 0.0f);
    return glm::lookAt(adjustedPosition, adjustedPosition + front, up);
} 