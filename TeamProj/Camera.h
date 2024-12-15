#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    
    float yaw;
    float pitch;
    
    float sensitivity;
public:
    Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 0.0f));
    
    void UpdateCamera();
    void SetMouseLook(float xoffset, float yoffset);
    glm::mat4 GetViewMatrix() const;
    
    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos) { position = pos; }
    
    glm::vec3 GetFront() const { return front; }
    void SetFront(const glm::vec3& newFront) { 
        front = newFront;
    }
    
    glm::vec3 GetUp() const { return up; }
    void SetUp(const glm::vec3& newUp) { up = glm::normalize(newUp); }
}; 