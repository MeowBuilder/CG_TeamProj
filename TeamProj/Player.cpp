#include "Player.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Player::~Player()
{
    if (isInitialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Player::Update(float deltaTime)
{
    previousPosition = position;
    
    velocity.y -= 9.8f * deltaTime;

    if (velocity.y < -20.0f) {
        velocity.y = -20.0f;
    }

    position += velocity * deltaTime;
    camera.SetPosition(position);
}

void Player::Jump() {
    if (isGrounded) {
        velocity.y = 4.0f;
        isGrounded = false;
    }
}

bool Player::InitializeBuffers()
{
    if (isInitialized) return true;

    try {
        float vertices[] = {
            -0.5f, -1.0f, -0.5f,
             0.5f, -1.0f, -0.5f,
             0.5f,  1.0f, -0.5f,
            -0.5f,  1.0f, -0.5f,
            -0.5f, -1.0f,  0.5f,
             0.5f, -1.0f,  0.5f,
             0.5f,  1.0f,  0.5f,
            -0.5f,  1.0f,  0.5f,
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        isInitialized = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing player buffers: " << e.what() << std::endl;
        return false;
    }
}

void Player::Render(GLuint shaderProgramID)
{
    if (!isInitialized) {
        if (!InitializeBuffers()) {
            std::cerr << "Failed to initialize player buffers" << std::endl;
            return;
        }
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, colliderSize);

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

    unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
    glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glDrawArrays(GL_LINE_LOOP, 4, 4);
    glBegin(GL_LINES);
    glVertex3f(-0.5f, -1.0f, -0.5f);
    glVertex3f(-0.5f, -1.0f,  0.5f);
    
    glVertex3f( 0.5f, -1.0f, -0.5f);
    glVertex3f( 0.5f, -1.0f,  0.5f);
    
    glVertex3f( 0.5f,  1.0f, -0.5f);
    glVertex3f( 0.5f,  1.0f,  0.5f);
    
    glVertex3f(-0.5f,  1.0f, -0.5f);
    glVertex3f(-0.5f,  1.0f,  0.5f);
    glEnd();
    glBindVertexArray(0);
}

bool Player::CheckCollision(const glm::vec3& objPos, const glm::vec3& objSize) const
{
    glm::vec3 colliderPos = position;
    colliderPos.y += colliderSize.y/2;

    bool collisionX = colliderPos.x + colliderSize.x/2 >= objPos.x - objSize.x/2 &&
                     objPos.x + objSize.x/2 >= colliderPos.x - colliderSize.x/2;
    bool collisionY = colliderPos.y + colliderSize.y/2 >= objPos.y - objSize.y/2 &&
                     objPos.y + objSize.y/2 >= colliderPos.y - colliderSize.y/2;
    bool collisionZ = colliderPos.z + colliderSize.z/2 >= objPos.z - objSize.z/2 &&
                     objPos.z + objSize.z/2 >= colliderPos.z - colliderSize.z/2;

    return collisionX && collisionY && collisionZ;
}

void Player::SetMouseLook(float xoffset, float yoffset)
{
    camera.SetMouseLook(xoffset, yoffset);
}

void Player::Move(const glm::vec3& direction, float deltaTime)
{
    const float SPEED = 5.0f;
    
    velocity.x = 0.0f;
    velocity.z = 0.0f;
    
    if (glm::length(direction) > 0.0f) {
        glm::vec3 normalizedDir = glm::normalize(direction);
        velocity.x = normalizedDir.x * SPEED;
        velocity.z = normalizedDir.z * SPEED;
    }
}
