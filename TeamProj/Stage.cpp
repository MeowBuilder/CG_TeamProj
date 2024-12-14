#include "Stage.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Stage::Stage(Shader* shader, Player* player) : shader(shader), player(player) {}

void Stage::Initialize() {
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, -1.0f)
    };

    glm::vec3 cubeSizes[] = {
        glm::vec3(20.0f, 1.0f, 20.0f),
        glm::vec3(1.0f)
    };

    glm::vec3 cubeColor[] = {
        glm::vec3(1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };

    float Moveable[] = { false, true };

    for (int i = 0; i < 2; i++) {
        Object cube;
        cube.Set_Obj(shader->shaderProgramID, "cube.obj");
        cube.SetPosition(cubePositions[i]);
        cube.SetSize(cubeSizes[i]);
        cube.SetRGB(cubeColor[i]);
        cube.SetMovable(Moveable[i]);
        cube.SetMass(1.0f);
        cubes.push_back(cube);
    }

    portals.push_back(Portal(glm::vec3(3.0, 2.0, 0.0)));
    portals.push_back(Portal(glm::vec3(6.0, 2.0, 0.0)));
    portals.push_back(Portal(glm::vec3(9.0, 2.0, 0.0)));
    portals[0].LinkPortal(&portals[1]);
    portals[1].LinkPortal(&portals[2]);
    portals[2].LinkPortal(&portals[0]);

    for (auto& portal : portals) {
        portal.SetRotation(glm::vec3(0.0f));
        portal.SetSize(glm::vec3(2.0f, 4.0f, 0.2f));
        portal.InitializeBuffers();
    }
}

void Stage::Update(float deltaTime) {
    glm::vec3 prevPosition = player->GetPosition();
    player->SetGrounded(false);

    player->Update(deltaTime);

    static bool canTeleport = true;
    if (canTeleport) {
        for (auto& portal : portals) {
            if (portal.ShouldTeleport(prevPosition, player->GetPosition(), player->GetColliderSize())) {
                portal.Teleport(*player);
                canTeleport = false;
                break;
            }
        }
    }
    else {
        bool check = true;
        for (auto& portal : portals) {
            if (portal.CheckCollision(player->GetPosition(), player->GetColliderSize())) {
                check = false;
                break;
            }
        }
        if (check) canTeleport = true;
    }

    std::vector<Object*> staticObjects;
    for (auto& cube : cubes) {
        if (!cube.IsMovable()) staticObjects.push_back(&cube);
    }

    for (auto& cube : cubes) {
        cube.Update(deltaTime, staticObjects);
    }

    for (size_t i = 0; i < cubes.size(); i++) {
        for (size_t j = i + 1; j < cubes.size(); j++) {
            glm::vec3 normal;
            float penetration;
            if (cubes[i].CheckCollisionWithBox(cubes[j].GetPosition(), cubes[j].GetSize(), normal, penetration)) {
                cubes[i].HandleCollision(&cubes[j], -normal, penetration);
                cubes[j].HandleCollision(&cubes[i], normal, penetration);
            }
        }
    }

    for (size_t i = 0; i < cubes.size(); i++) {
        glm::vec3 normal;
        float penetration;

        if (cubes[i].CheckCollisionWithBox(player->GetPosition(), player->GetColliderSize(), normal, penetration)) {
            if (cubes[i].IsMovable()) {
                glm::vec3 pushVelocity = player->GetVelocity();
                if (glm::length(pushVelocity) > 0.1f) {
                    pushVelocity.y = 0.0f;
                    cubes[i].SetVelocity(cubes[i].GetVelocity() + pushVelocity);
                }
            }

            glm::vec3 correction;
            if (normal.y > 0.7f) {
                correction = glm::vec3(0.0f, penetration, 0.0f);
                player->SetPosition(player->GetPosition() + correction);

                if (player->GetVelocity().y < 0) {
                    player->SetGrounded(true);
                    glm::vec3 vel = player->GetVelocity();
                    vel.y = 0.0f;
                    player->SetVelocity(vel);
                }
            }
            else {
                player->SetPosition(prevPosition);
                glm::vec3 vel = player->GetVelocity();
                float velDotNormal = glm::dot(vel, normal);
                if (velDotNormal < 0) {
                    vel = vel - (normal * velDotNormal);
                    if (abs(normal.y) < 0.7f) {
                        vel.x *= 0.1f;
                        vel.z *= 0.1f;
                    }
                }
                player->SetVelocity(vel);
            }
        }
    }
}

void Stage::RenderPortal() {
    for (auto& portal : portals)
    {
        portal.RenderView(shader->shaderProgramID, player->GetCamera());
        Render(true);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Stage::Render(static bool skipPortals) {
    for (auto& cube : cubes) {
        cube.Draw(shader->shaderProgramID);
    }

    player->Render(shader->shaderProgramID);

    if (!skipPortals)
    {
        for (auto& portal : portals) {
            portal.Render(shader->shaderProgramID);
        }
    }

}
