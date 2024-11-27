#include "Player.h"

Player::Player(glm::vec3 startPos) : 
    position(startPos),
    rotation(0.0f),
    velocity(0.0f),
    size(1.0f),
    camera(startPos)
{
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
    position += velocity * deltaTime;
    
    velocity *= 0.8f;
    
    camera.SetPosition(position);
    
    if (glm::length(velocity) < 0.1f) {
        velocity = glm::vec3(0.0f);
    }
}

void Player::SetMouseLook(float xoffset, float yoffset)
{
    camera.SetMouseLook(xoffset, yoffset);
}

void Player::Move(const glm::vec3& direction, float deltaTime)
{
    float speed = 60.0f;
    
    glm::vec3 forward = glm::normalize(glm::vec3(camera.GetFront().x, 0.0f, camera.GetFront().z));
    glm::vec3 right = glm::normalize(glm::cross(forward, camera.GetUp()));
    
    glm::vec3 newVelocity = direction * speed;
    
    float maxSpeed = 35.0f;
    velocity += newVelocity * deltaTime;
    
    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
}

void Player::Jump() {
    velocity.y = 1.0f;
}

void Player::Render()
{
    // 이제 카메라 변환은 Main.cpp에서 처리
}
