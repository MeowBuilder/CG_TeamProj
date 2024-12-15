#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include "Camera.h"

class Player
{
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 colliderSize;
    GLuint VAO, VBO;
    bool isInitialized;
    bool isGrounded;
    
    Camera camera;

public:
    Player(glm::vec3 startPos = glm::vec3(0.0f)) : 
        position(startPos),
        velocity(0.0f),
        colliderSize(glm::vec3(0.8f, 1.6f, 0.8f)),
        isInitialized(false),
        isGrounded(false),
        camera(startPos) {}

    ~Player();

    void Update(float deltaTime);
    void Render(GLuint shaderProgramID);
    bool InitializeBuffers();
    
    bool CheckCollision(const glm::vec3& objPos, const glm::vec3& objSize) const;
    
    bool IsInitialized() const { return isInitialized; }
    bool IsGrounded() const { return isGrounded; }
    void SetGrounded(bool grounded) { isGrounded = grounded; }
    
    glm::vec3 GetColliderSize() const { return colliderSize; }
    void SetColliderSize(const glm::vec3& size) { colliderSize = size; }
    
    void SetMouseLook(float xoffset, float yoffset);
    void Move(const glm::vec3& direction, float deltaTime);
    
    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos) { 
        position = pos;
        camera.SetPosition(pos);
    }
    
    glm::vec3 GetVelocity() const { return velocity; }
    void SetVelocity(const glm::vec3& vel) { velocity = vel; }
    
    Camera& GetCamera() { return camera; }
    
    void Jump();
};

