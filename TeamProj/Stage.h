#include <vector>
#include "Object.h"
#include "Portal.h"
#include "Player.h"
#include "Shader.h"

class Stage {
private:
    Shader* shader;
    Player* player;
    std::vector<Object> cubes;
    std::vector<Portal> portals;

public:
    Stage(Shader* shader, Player* player);
    void Initialize();
    void Update(float deltaTime);
    void Render(static bool skipPortals = false);
    void RenderPortal();
};
