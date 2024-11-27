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
    // 중력 적용
    const float GRAVITY = -9.8f;
    velocity.y += GRAVITY * deltaTime;
    
    // 임시 위치 계산
    glm::vec3 newPosition = position + velocity * deltaTime;
    
    // 바닥 충돌 검사
    if (newPosition.y < 0.0f) {
        newPosition.y = 0.0f;  // 바닥 위치로 보정
        velocity.y = 0.0f;     // 수직 속도를 0으로
    }
    
    // 위치 업데이트
    position = newPosition;
    
    // 수평 방향 감속 (더 강한 감속 적용)
    glm::vec2 horizontalVel(velocity.x, velocity.z);
    if (glm::length(horizontalVel) > 0.1f) {
        velocity.x *= 0.85f;  // 0.95f에서 0.85f로 변경
        velocity.z *= 0.85f;  // 더 빠른 감속
    } else {
        velocity.x = 0.0f;
        velocity.z = 0.0f;
    }
    
    // 카메라 위치 업데이트
    camera.SetPosition(position);
}

void Player::SetMouseLook(float xoffset, float yoffset)
{
    camera.SetMouseLook(xoffset, yoffset);
}

void Player::Move(const glm::vec3& direction, float deltaTime)
{
    float speed = 50.0f;  // 60.0f에서 20.0f로 감소
    
    glm::vec3 forward = glm::normalize(glm::vec3(camera.GetFront().x, 0.0f, camera.GetFront().z));
    glm::vec3 right = glm::normalize(glm::cross(forward, camera.GetUp()));
    
    glm::vec3 newVelocity = direction * speed;
    
    float maxSpeed = 20.0f;  // 35.0f에서 10.0f로 감소
    velocity += newVelocity * deltaTime;
    
    // 수평 속도만 제한
    glm::vec2 horizontalVel(velocity.x, velocity.z);
    if (glm::length(horizontalVel) > maxSpeed) {
        horizontalVel = glm::normalize(horizontalVel) * maxSpeed;
        velocity.x = horizontalVel.x;
        velocity.z = horizontalVel.y;
    }
}

void Player::Jump() {
    // 바닥에 있을 때만 점프 가능
    if (position.y <= 0.01f) {  // 약간의 여유를 둠
        velocity.y = 5.0f;  // 점프 힘
    }
}

void Player::Render()
{
    // 이제 카메라 변환은 Main.cpp에서 처리
}
