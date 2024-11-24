#pragma once
#include <GL/glut.h>  // OpenGL 헤더
#include <glm/glm.hpp>  // GLM 수학 라이브러리
#include "Camera.h"

class Player
{
private:
    glm::vec3 position;    // 플레이어 위치
    glm::vec3 rotation;    // 플레이어 회전 (pitch, yaw, roll)
    glm::vec3 velocity;    // 플레이어 속도
    float size;            // 플레이어 크기
    
    Camera camera;         // 플레이어 카메라

public:
    Player(glm::vec3 startPos = glm::vec3(0.0f));  // 생성자
    ~Player();                                      // 소멸자

    // 기본 함수들
    void Update(float deltaTime);  // deltaTime 매개변수 추가
    void Render();         // 렌더링 함수
    
    // 카메라 제어 함수
    void SetMouseLook(float xoffset, float yoffset);
    void Move(const glm::vec3& direction, float deltaTime);
    
    // getter/setter
    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos) { 
        position = pos;
        camera.SetPosition(pos);  // 카메라 위치도 함께 업데이트
    }
    
    glm::vec3 GetRotation() const { return rotation; }
    void SetRotation(const glm::vec3& rot) { rotation = rot; }
    
    glm::vec3 GetVelocity() const { return velocity; }
    void SetVelocity(const glm::vec3& vel) { velocity = vel; }
    
    glm::vec3 GetCameraFront() const { return camera.GetFront(); }
    glm::vec3 GetCameraUp() const { return camera.GetUp(); }
    
    Camera& GetCamera() { return camera; }
};

