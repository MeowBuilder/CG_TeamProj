#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <vector>
#include "Camera.h"

// 함수 포인터 타입 정의 추가
typedef void (*RenderSceneCallback)(GLuint, bool);

class Portal {
private:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 size;
    
    Portal* linkedPortal;
    GLuint textureId;
    GLuint frameBuffer;
    GLuint renderBuffer;

    const int WIN_W = 1920, WIN_H = 1080;

    // 포탈 메쉬를 위한 버퍼
    GLuint VAO, VBO, EBO;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    static RenderSceneCallback renderSceneCallback;

    void CreateRenderTexture(int width, int height);
    void CreatePortalMesh();
    glm::mat4 GetPortalTransform() const;
    glm::mat4 CalculateObliqueViewProjectionMatrix(const glm::mat4& projection, const glm::vec4& clipPlane) const;
    
public:
    Portal(const glm::vec3& pos = glm::vec3(0.0f), 
           const glm::vec3& norm = glm::vec3(0.0f, 0.0f, 1.0f),
           const glm::vec3& portalSize = glm::vec3(2.0f, 3.0f, 0.1f));
    ~Portal();
    
    void Initialize(int screenWidth, int screenHeight);
    void LinkPortal(Portal* other) { linkedPortal = other; }
    
    void RenderView(const Camera& playerCam, GLuint shaderProgram);
    void RenderPortal(GLuint shaderProgram);
    
    glm::vec3 WorldToPortalSpace(const glm::vec3& worldPos) const;
    glm::vec3 PortalToWorldSpace(const glm::vec3& localPos) const;
    
    bool IsInFront(const glm::vec3& point) const;
    bool IsPointInPortal(const glm::vec3& point) const;
    
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetNormal() const { return normal; }
    const glm::vec3& GetSize() const { return size; }
    const glm::vec3& GetRight() const { return right; }
    const glm::vec3& GetUp() const { return up; }

    Portal& operator=(const Portal& other) {
        if (this != &other) {
            position = other.position;
            normal = other.normal;
            up = other.up;
            right = other.right;
            size = other.size;
            linkedPortal = other.linkedPortal;
        }
        return *this;
    }

    static void SetRenderSceneCallback(RenderSceneCallback callback) {
        renderSceneCallback = callback;
    }
};
