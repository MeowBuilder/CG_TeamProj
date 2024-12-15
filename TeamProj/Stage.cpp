#include "Stage.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Stage::Stage(Shader* shader, Player* player) : shader(shader), player(player) {}

void Stage::Initialize() {
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, -0.5f, 0.0f), //½ÃÀÛ ¶¥

        glm::vec3(100.0f, -0.5f, -100.0f), // °¡Â¥ ¶¥
        glm::vec3(100.0f, -0.5f, 0.0f), //1¹ø ÁøÂ¥ ¶¥
        glm::vec3(100.0f, -0.5f, 100.0f), // °¡Â¥ ¶¥

        glm::vec3(200.0f, -0.5f, -100.0f), // °¡Â¥ ¶¥
        glm::vec3(200.0f, -0.5f, 0.0f), //2¹ø ÁøÂ¥ ¶¥
        glm::vec3(200.0f, -0.5f, 100.0f), // °¡Â¥ ¶¥

        glm::vec3(300.0f, -0.5f, -100.0f), // °¡Â¥ ¶¥
        glm::vec3(300.0f, -0.5f, 0.0f), //3¹ø ÁøÂ¥ ¶¥
        glm::vec3(300.0f, -0.5f, 100.0f), // °¡Â¥ ¶¥

        glm::vec3(400.0f, -0.5f, -100.0f), // °¡Â¥ ¶¥
        glm::vec3(400.0f, -0.5f, 0.0f), //4¹ø ÁøÂ¥ ¶¥
        glm::vec3(400.0f, -0.5f, 100.0f), // °¡Â¥ ¶¥

    };

    glm::vec3 cubeSizes[] = {
        glm::vec3(50.0f, 1.0f, 50.0f),

        glm::vec3(5.0f, 1.0f, 5.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),

        glm::vec3(5.0f, 1.0f, 5.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),

        glm::vec3(5.0f, 1.0f, 5.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),

        glm::vec3(5.0f, 1.0f, 5.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),
        glm::vec3(50.0f, 1.0f, 50.0f),
    };

    glm::vec3 cubeColor[] = {
        glm::vec3(1.0f,1.0f,1.0f),

        glm::vec3(0.0f,0.0f,1.0f),
        glm::vec3(0.0f,0.0f,1.0f),
        glm::vec3(1.0f,0.0f,0.0f),

        glm::vec3(0.0f,1.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f),
        glm::vec3(1.0f,0.0f,0.0f),

        glm::vec3(0.0f,1.0f,1.0f),
        glm::vec3(0.0f,1.0f,1.0f),
        glm::vec3(1.0f,1.0f,1.0f),

        glm::vec3(1.0f,1.0f,1.0f),
        glm::vec3(1.0f,0.0f,1.0f),
        glm::vec3(0.0f,0.0f,1.0f),
    };

    for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
        Object cube;
        cube.Set_Obj(shader->shaderProgramID, "cube.obj");
        cube.SetPosition(cubePositions[i]);
        cube.SetSize(cubeSizes[i]);
        cube.SetRGB(cubeColor[i]);
        cube.SetMovable(false);
        cube.SetMass(1.0f);
        cubes.push_back(cube);
    }

    for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); i++) {
        Object cube;
        cube.Set_Obj(shader->shaderProgramID, "cube.obj");
        cube.SetPosition(cubePositions[i] + glm::vec3(0.0,1.0,0.0));
        cube.SetSize(glm::vec3(1.0f));
        cube.SetRGB(glm::vec3(1.0,0.0,0.0));
        cube.SetMovable(true);
        cube.SetMass(1.0f);
        cubes.push_back(cube);
    }

    {
        portals.push_back(Portal(glm::vec3(0, 100, 0)));

        portals.push_back(Portal(cubePositions[1] + glm::vec3(0, 2.5, cubeSizes[1].z / 2.0f)));
        portals.push_back(Portal(cubePositions[2] + glm::vec3(0, 2.5, cubeSizes[2].z / 2.0f)));
        portals.push_back(Portal(cubePositions[3] + glm::vec3(0, 2.5, cubeSizes[3].z / 2.0f)));
        portals.push_back(Portal(cubePositions[4] + glm::vec3(0, 2.5, cubeSizes[4].z / 2.0f)));
        portals.push_back(Portal(cubePositions[5] + glm::vec3(0, 2.5, cubeSizes[5].z / 2.0f)));
        portals.push_back(Portal(cubePositions[6] + glm::vec3(0, 2.5, cubeSizes[6].z / 2.0f)));
        portals.push_back(Portal(cubePositions[7] + glm::vec3(0, 2.5, cubeSizes[7].z / 2.0f)));
        portals.push_back(Portal(cubePositions[8] + glm::vec3(0, 2.5, cubeSizes[8].z / 2.0f)));
        portals.push_back(Portal(cubePositions[9] + glm::vec3(0, 2.5, cubeSizes[9].z / 2.0f)));
        portals.push_back(Portal(cubePositions[10] + glm::vec3(0, 2.5, cubeSizes[10].z / 2.0f)));
        portals.push_back(Portal(cubePositions[11] + glm::vec3(0, 2.5, cubeSizes[11].z / 2.0f)));
        portals.push_back(Portal(cubePositions[12] + glm::vec3(0, 2.5, cubeSizes[12].z / 2.0f)));

        portals.push_back(Portal(cubePositions[0] + glm::vec3(0, 2.5, -cubeSizes[0].z / 2.0f)));
        portals.push_back(Portal(cubePositions[0] + glm::vec3(-5, 2.5, -cubeSizes[0].z / 2.0f)));//
        portals.push_back(Portal(cubePositions[0] + glm::vec3(5, 2.5, -cubeSizes[0].z / 2.0f)));

        portals.push_back(Portal(cubePositions[2] + glm::vec3(5, 2.5, -cubeSizes[2].z / 2.0f)));//
        portals.push_back(Portal(cubePositions[2] + glm::vec3(-5, 2.5, -cubeSizes[2].z / 2.0f)));
        portals.push_back(Portal(cubePositions[2] + glm::vec3(0, 2.5, -cubeSizes[2].z / 2.0f)));

        portals.push_back(Portal(cubePositions[5] + glm::vec3(0, 2.5, -cubeSizes[5].z / 2.0f)));
        portals.push_back(Portal(cubePositions[5] + glm::vec3(5, 2.5, -cubeSizes[5].z / 2.0f)));//
        portals.push_back(Portal(cubePositions[5] + glm::vec3(-5, 2.5, -cubeSizes[5].z / 2.0f)));

        portals.push_back(Portal(cubePositions[8] + glm::vec3(5, 2.5, -cubeSizes[8].z / 2.0f)));
        portals.push_back(Portal(cubePositions[8] + glm::vec3(0, 2.5, -cubeSizes[8].z / 2.0f)));
        portals.push_back(Portal(cubePositions[8] + glm::vec3(-5, 2.5, -cubeSizes[8].z / 2.0f)));//

        portals.push_back(Portal(cubePositions[11] + glm::vec3(0, 2.5, -cubeSizes[11].z / 2.0f)));

        portals[0].LinkPortal(&portals[0]);

        portals[1].LinkPortal(&portals[0]);
        portals[2].LinkPortal(&portals[0]);//
        portals[3].LinkPortal(&portals[0]);
        portals[4].LinkPortal(&portals[0]);
        portals[5].LinkPortal(&portals[0]);//
        portals[6].LinkPortal(&portals[0]);
        portals[7].LinkPortal(&portals[0]);
        portals[8].LinkPortal(&portals[0]);//
        portals[9].LinkPortal(&portals[0]);
        portals[10].LinkPortal(&portals[0]);
        portals[11].LinkPortal(&portals[0]);//
        portals[12].LinkPortal(&portals[0]);

        portals[13].LinkPortal(&portals[1]);
        portals[14].LinkPortal(&portals[2]);//
        portals[15].LinkPortal(&portals[3]);

        portals[16].LinkPortal(&portals[5]);//
        portals[17].LinkPortal(&portals[6]);
        portals[18].LinkPortal(&portals[4]);

        portals[19].LinkPortal(&portals[9]);
        portals[20].LinkPortal(&portals[8]);//
        portals[21].LinkPortal(&portals[7]);

        portals[22].LinkPortal(&portals[10]);
        portals[23].LinkPortal(&portals[12]);
        portals[24].LinkPortal(&portals[11]);//

        portals[25].LinkPortal(&portals[0]);
        portals[25].Setend();

    }

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

    if (player->GetPosition().y < - 75.0f)
    {
        player->SetPosition(glm::vec3(0.0f, 75.0f, 0.0f));
    }

    static bool canTeleport = true;
    if (canTeleport) {
        for (auto& portal : portals) {
            if (portal.ShouldTeleport(prevPosition, player->GetPosition(), player->GetColliderSize())) {
                portal.Teleport(*player);
                if (portal.isend)
                    glutLeaveMainLoop();
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
