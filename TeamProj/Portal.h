#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include "Camera.h"

class Player;

class Portal
{
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 size;
    GLuint VAO, VBO;
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint renderbuffer;
    bool isInitialized;
    Portal* linkedPortal;
    unsigned int PORTAL_WIDTH;
    unsigned int PORTAL_HEIGHT;
    Camera virtualCamera;

public:
    Portal(glm::vec3 startPos = glm::vec3(0.0f)) :
        position(startPos),
        rotation(glm::vec3(0.0f)),
        size(glm::vec3(1.0f, 2.0f, 0.2f)),
        isInitialized(false),
        linkedPortal(nullptr),
        PORTAL_WIDTH(1024),
        PORTAL_HEIGHT(1024),
        virtualCamera(startPos),
        isend(false) {}

    ~Portal();

    bool InitializeBuffers();
    void Render(GLuint shaderProgramID);
    void RenderView(GLuint shaderProgramID, const Camera& playerCamera);
    glm::mat4 ModifyProjectionMatrixForClipping(const glm::mat4& proj, const glm::vec4& clipPlane);
    bool CheckCollision(const glm::vec3& objPos, const glm::vec3& objSize) const;
    bool ShouldTeleport(const glm::vec3& prevPos, const glm::vec3& currentPos, const glm::vec3& objSize) const;
    void Teleport(Player& player) const;
    void LinkPortal(Portal* other) { linkedPortal = other; }

    glm::vec3 GetPosition() const { return position; }
    void SetPosition(const glm::vec3& pos) { 
        position = pos;
        virtualCamera.SetPosition(pos);
    }

    glm::vec3 GetRotation() const { return rotation; }
    void SetRotation(const glm::vec3& rot) { rotation = rot; }

    glm::vec3 GetSize() const { return size; }
    void SetSize(const glm::vec3& s) { size = s; }

    Portal* GetLinkedPortal() const { return linkedPortal; }
    bool IsInitialized() const { return isInitialized; }
    
    GLuint GetTexture() const { return textureColorbuffer; }

    bool isend;
    void Setend() { isend = true; }
}; 