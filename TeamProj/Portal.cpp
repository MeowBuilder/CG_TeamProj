#include "Portal.h"
#include "Player.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Portal::~Portal()
{
    if (isInitialized) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteTextures(1, &textureColorbuffer);
        glDeleteRenderbuffers(1, &renderbuffer);
    }
}

bool Portal::InitializeBuffers()
{
    if (isInitialized) return true;

    try {
        float aspectRatio = size.x / size.y;
        PORTAL_HEIGHT = 1024;
        PORTAL_WIDTH = static_cast<unsigned int>(PORTAL_HEIGHT * aspectRatio);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
             0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, PORTAL_WIDTH, PORTAL_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, PORTAL_WIDTH, PORTAL_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is not complete!" << std::endl;
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(0);

        isInitialized = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing portal buffers: " << e.what() << std::endl;
        return false;
    }
}

void Portal::RenderView(GLuint shaderProgramID, const Camera& playerCamera)
{
    if (!linkedPortal || !isInitialized) return;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, PORTAL_WIDTH, PORTAL_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 portalTransform = glm::mat4(1.0f);
    portalTransform = glm::translate(portalTransform, position);
    portalTransform = glm::rotate(portalTransform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 linkedPortalTransform = glm::mat4(1.0f);
    linkedPortalTransform = glm::translate(linkedPortalTransform, linkedPortal->position);
    linkedPortalTransform = glm::rotate(linkedPortalTransform, glm::radians(linkedPortal->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 portalToPortalTransform = linkedPortalTransform * glm::inverse(portalTransform);

    glm::mat4 cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, playerCamera.GetPosition());
    
    glm::vec3 front = playerCamera.GetFront();
    glm::vec3 up = playerCamera.GetUp();
    glm::vec3 right = glm::normalize(glm::cross(front, up));
    
    glm::mat4 cameraRotation = glm::mat4(
        right.x, right.y, right.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        -front.x, -front.y, -front.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    cameraTransform = cameraTransform * cameraRotation;

    glm::mat4 virtualCameraTransform = portalToPortalTransform * cameraTransform;

    glm::vec3 virtualPos = glm::vec3(virtualCameraTransform[3]);
    
    glm::vec3 virtualFront = -glm::vec3(virtualCameraTransform[2]);
    glm::vec3 virtualUp = glm::vec3(virtualCameraTransform[1]);

    virtualCamera.SetPosition(virtualPos);
    virtualCamera.SetFront(glm::normalize(virtualFront));
    virtualCamera.SetUp(glm::normalize(virtualUp));

    // 뷰 행렬과 투영 행렬 설정
    glm::mat4 view = virtualCamera.GetViewMatrix();
    unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    float aspectRatio = (float)PORTAL_WIDTH / (float)PORTAL_HEIGHT;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    GLint useTextureLocation = glGetUniformLocation(shaderProgramID, "useTexture");
    glUniform1i(useTextureLocation, 0);
}

void Portal::Render(GLuint shaderProgramID)
{
    if (!isInitialized) {
        if (!InitializeBuffers()) {
            std::cerr << "Failed to initialize portal buffers" << std::endl;
            return;
        }
    }

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, size);

    unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "transform");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(VAO);
    
    if (linkedPortal) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLint useTextureLocation = glGetUniformLocation(shaderProgramID, "useTexture");
        glUniform1i(useTextureLocation, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        GLint portalTextureLocation = glGetUniformLocation(shaderProgramID, "portalTexture");
        glUniform1i(portalTextureLocation, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glLineWidth(3.0f);
        useTextureLocation = glGetUniformLocation(shaderProgramID, "useTexture");
        glUniform1i(useTextureLocation, 0);
        unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
        glUniform3f(colorLocation, 0.0f, 0.8f, 1.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glLineWidth(1.0f);
    }
    else {
        GLint useTextureLocation = glGetUniformLocation(shaderProgramID, "useTexture");
        glUniform1i(useTextureLocation, 0);

        unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
        glUniform3f(colorLocation, 1.0f, 0.5f, 0.0f);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    glBindVertexArray(0);
}

bool Portal::CheckCollision(const glm::vec3& objPos, const glm::vec3& objSize) const
{
    glm::vec3 portalPos = position;
    glm::vec3 portalSize = size;

    bool collisionX = portalPos.x + portalSize.x/2 >= objPos.x - objSize.x/2 &&
                     objPos.x + objSize.x/2 >= portalPos.x - portalSize.x/2;
    bool collisionY = portalPos.y + portalSize.y/2 >= objPos.y - objSize.y/2 &&
                     objPos.y + objSize.y/2 >= portalPos.y - portalSize.y/2;
    bool collisionZ = portalPos.z + portalSize.z/2 >= objPos.z - objSize.z/2 &&
                     objPos.z + objSize.z/2 >= portalPos.z - portalSize.z/2;

    return collisionX && collisionY && collisionZ;
}

bool Portal::ShouldTeleport(const glm::vec3& prevPos, const glm::vec3& currentPos, const glm::vec3& objSize) const
{
    if (!linkedPortal) return false;

    // 포털의 앞쪽 방향 벡터 계산
    glm::vec3 portalForward = glm::vec3(
        sin(glm::radians(rotation.y)),
        0.0f,
        -cos(glm::radians(rotation.y))
    );

    // 포털의 오른쪽 방향 벡터 계산
    glm::vec3 portalRight = glm::vec3(
        cos(glm::radians(rotation.y)),
        0.0f,
        sin(glm::radians(rotation.y))
    );

    // 이전 위치와 현재 위치에서 포털까지의 거리 계산
    glm::vec3 toPrevPos = prevPos - position;
    glm::vec3 toCurrentPos = currentPos - position;
    
    float prevDist = glm::dot(toPrevPos, portalForward);
    float currentDist = glm::dot(toCurrentPos, portalForward);

    // 포털을 통과했는지 확인 (이전 위치는 포털 앞쪽, 현재 위치는 포털 뒤쪽)
    bool crossedPortal = (prevDist >= -0.1f && currentDist < 0.1f);  // 약간의 여유 추가

    if (crossedPortal) {
        // 포털 평면에서의 상대 위치 계산
        float rightOffset = glm::dot(toCurrentPos, portalRight);
        float upOffset = toCurrentPos.y;

        // 포털 크기의 절반 + 약간의 여유
        float halfWidth = size.x * 0.55f;  // 10% 여유 추가
        float halfHeight = size.y * 0.55f;

        // 객체 크기의 절반
        float objHalfWidth = objSize.x * 0.5f;
        float objHalfHeight = objSize.y * 0.5f;

        // 포털 영역 내에 있는지 확인 (여유 있게 체크)
        bool inPortal = (abs(rightOffset) - objHalfWidth < halfWidth &&
                        abs(upOffset) - objHalfHeight < halfHeight);

        if (inPortal) {
            std::cout << "포털 통과 감지! 거리: " << currentDist 
                      << ", 좌우 오프셋: " << rightOffset 
                      << ", 상하 오프셋: " << upOffset << std::endl;
        }

        return inPortal;
    }

    return false;
}

#include <iostream>

void Portal::Teleport(Player& player) const
{
    if (!linkedPortal) return;

    // 포털 A의 월드 변환 행렬
    glm::mat4 portalTransform = glm::mat4(1.0f);
    portalTransform = glm::translate(portalTransform, position);
    portalTransform = glm::rotate(portalTransform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    // 포털 B의 월드 변환 행렬
    glm::mat4 linkedPortalTransform = glm::mat4(1.0f);
    linkedPortalTransform = glm::translate(linkedPortalTransform, linkedPortal->position);
    linkedPortalTransform = glm::rotate(linkedPortalTransform, glm::radians(linkedPortal->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    // 포털 A에서 B로의 변환 행렬
    glm::mat4 portalToPortalTransform = linkedPortalTransform * glm::inverse(portalTransform);

    // 플레이어의 현재 방향 벡터
    glm::vec3 playerFront = player.GetCamera().GetFront();

    // 새로운 위치 계산
    glm::vec3 newPosition = glm::vec3(portalToPortalTransform * glm::vec4(player.GetPosition(), 1.0f));

    // 포털의 방향 벡터
    glm::vec3 portalForward = glm::vec3(
        sin(glm::radians(rotation.y)),
        0.0f,
        -cos(glm::radians(rotation.y))
    );

    // 연결된 포털의 방향 벡터
    glm::vec3 linkedPortalForward = glm::vec3(
        sin(glm::radians(linkedPortal->rotation.y)),
        0.0f,
        -cos(glm::radians(linkedPortal->rotation.y))
    );

    // 포털 간의 상대적인 각도 계산
    float portalsAngle = atan2(glm::dot(glm::cross(portalForward, linkedPortalForward), glm::vec3(0.0f, 1.0f, 0.0f)), 
                                glm::dot(portalForward, linkedPortalForward));

    // 포털의 방향과 플레이어의 방향 간의 각도 계산
    float angle = atan2(glm::dot(glm::cross(portalForward, playerFront), glm::vec3(0.0f, 1.0f, 0.0f)), 
                        glm::dot(portalForward, playerFront));

    // 새로운 방향 계산 (포털의 방향을 기준으로 회전)
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), portalsAngle + angle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 newFront = glm::vec3(rotationMatrix * glm::vec4(playerFront, 1.0f));

    // 변환된 값들 적용
    player.SetPosition(newPosition);
    player.GetCamera().SetFront(newFront); // 포털의 방향에 따라 설정
    player.GetCamera().SetUp(glm::vec3(0.0f, 1.0f, 0.0f));  // Up 벡터는 항상 수직 유지
    player.SetVelocity(player.GetVelocity()); // 속도는 그대로 유지

    std::cout << "텔레포트 완료: (" << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << ")" << std::endl;
}