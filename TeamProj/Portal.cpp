#include "Portal.h"

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
    cameraTransform = glm::translate(cameraTransform, playerCamera.getPosition());
    
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

    // 뒷면 렌더링
    if (linkedPortal) {
        glDisable(GL_CULL_FACE); // 깊이 테스트 비활성화
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLint useTextureLocation = glGetUniformLocation(shaderProgramID, "useTexture");
        glUniform1i(useTextureLocation, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        GLint portalTextureLocation = glGetUniformLocation(shaderProgramID, "portalTexture");
        glUniform1i(portalTextureLocation, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 뒷면 렌더링

        glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
        glEnable(GL_CULL_FACE); // 다시 활성화
    }

    // 앞면 렌더링
    GLint useTextureLocation = glGetUniformLocation(shaderProgramID, "useTexture");
    glUniform1i(useTextureLocation, 0);

    unsigned int colorLocation = glGetUniformLocation(shaderProgramID, "colorAttribute");
    glUniform3f(colorLocation, 1.0f, 0.5f, 0.0f);
    glDrawArrays(GL_LINE_LOOP, 0, 4); // 앞면 렌더링

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

    // 포털을 양방향으로 통과했는지 확인
    bool crossedPortal = (prevDist * currentDist < 0);  // 부호가 바뀌었다면 통과한 것

    if (crossedPortal) {
        // 포털 평면에서의 상대 위치 계산
        float rightOffset = glm::dot(toCurrentPos, portalRight);
        float upOffset = toCurrentPos.y;

        // 포털 크기의 절반 + 약간의 여유
        float halfWidth = size.x * 0.55f;
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

    // 새로운 위치 계산
    glm::vec3 newPosition = glm::vec3(linkedPortalTransform * glm::inverse(portalTransform) * glm::vec4(player.GetPosition(), 1.0f));

    // 현재 포털의 방향 벡터
    glm::vec3 portalForward = glm::vec3(
        -sin(glm::radians(rotation.y)),
        0.0f,
        cos(glm::radians(rotation.y))
    );

    // 플레이어의 현재 방향
    glm::vec3 playerFront = player.GetCamera().GetFront();
    float playerY = playerFront.y;  // Y 성분 보존

    // 포털과 플레이어 방향 사이의 각도 계산
    playerFront.y = 0.0f;
    playerFront = glm::normalize(playerFront);
    float angle = acos(glm::dot(playerFront, portalForward));
    
    // 각도의 부호 결정 (왼쪽/오른쪽)
    float cross = glm::dot(glm::cross(portalForward, playerFront), glm::vec3(0.0f, 1.0f, 0.0f));
    if (cross < 0) angle = -angle;

    // 새로운 방향 계산 (연결된 포털 기준으로 같은 각도 적용)
    glm::vec3 newFront = glm::vec3(
        -sin(glm::radians(linkedPortal->rotation.y) + angle),
        playerY,
        cos(glm::radians(linkedPortal->rotation.y) + angle)
    );

    // 변환된 값들 적용
    player.SetPosition(newPosition);
    player.GetCamera().SetFront(glm::normalize(newFront));
    
    // 속도 벡터도 같은 각도로 회전
    glm::vec3 velocity = player.GetVelocity();
    if (glm::length(velocity) > 0.0f) {
        velocity.y = 0.0f;
        float velAngle = acos(glm::dot(glm::normalize(velocity), portalForward));
        float velCross = glm::dot(glm::cross(portalForward, glm::normalize(velocity)), glm::vec3(0.0f, 1.0f, 0.0f));
        if (velCross < 0) velAngle = -velAngle;

        float speed = glm::length(velocity);
        velocity = glm::vec3(
            -sin(glm::radians(linkedPortal->rotation.y) + velAngle),
            player.GetVelocity().y,
            cos(glm::radians(linkedPortal->rotation.y) + velAngle)
        ) * speed;
        player.SetVelocity(velocity);
    }

    std::cout << "텔레포트 완료: (" << newPosition.x << ", " << newPosition.y << ", " << newPosition.z << ")" << std::endl;
}

glm::mat4 Portal::getVirtualCameraMatrix(const Camera& playerCamera, bool isFrontSide) {
    glm::mat4 portalToPortal = linkedPortal->transform * glm::inverse(transform);
    
    glm::vec3 camPos = playerCamera.getPosition();
    glm::vec3 camDir = playerCamera.GetFront();
    
    // 포탈 기준 상대 위치 계산
    glm::vec3 relativePos = camPos - glm::vec3(transform[3].x, transform[3].y, transform[3].z);
    
    // 포탈을 통과한 가상 카메라 위치 계산
    glm::vec4 virtualPos = portalToPortal * glm::vec4(relativePos, 1.0f);
    glm::vec4 virtualDir = portalToPortal * glm::vec4(camDir, 0.0f);
    
    // 최종 가상 카메라 매트릭스 생성
    return glm::lookAt(
        glm::vec3(virtualPos.x, virtualPos.y, virtualPos.z) + glm::vec3(linkedPortal->transform[3].x, linkedPortal->transform[3].y, linkedPortal->transform[3].z),
        glm::vec3(virtualPos.x, virtualPos.y, virtualPos.z) + glm::vec3(linkedPortal->transform[3].x, linkedPortal->transform[3].y, linkedPortal->transform[3].z) + glm::normalize(glm::vec3(virtualDir.x, virtualDir.y, virtualDir.z)),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

glm::mat4 Portal::getObliqueViewFrustumMatrix(const glm::mat4& projMatrix, const glm::vec4& clipPlane) {
    glm::vec4 q = glm::inverse(projMatrix) * glm::vec4(
        (clipPlane.x < 0.0f ? -1.0f : 1.0f),
        (clipPlane.y < 0.0f ? -1.0f : 1.0f),
        -1.0f,
        1.0f
    );
    
    glm::vec4 c = clipPlane * (2.0f / glm::dot(clipPlane, q));
    
    glm::mat4 newProj = projMatrix;
    newProj[0][2] = c.x;
    newProj[1][2] = c.y;
    newProj[2][2] = c.z + 1.0f;
    newProj[3][2] = c.w;
    
    return newProj;
} 
