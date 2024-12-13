void Renderer::renderPortals(const Camera& playerCamera) {
    for (Portal* portal : portals) {
        for (bool isFrontSide : {true, false}) {
            glm::mat4 virtualCamMatrix = portal->getVirtualCameraMatrix(playerCamera, isFrontSide);
            
            glm::vec4 clipPlane = calculatePortalClipPlane(portal, isFrontSide);
            
            glm::mat4 projMatrix = getObliqueViewFrustumMatrix(playerCamera.GetProjectionMatrix(), clipPlane);
            
            useStencilBuffer();
            renderPortalStencil(portal);
            
            renderSceneFromPortal(virtualCamMatrix, projMatrix);
            
            clearStencilBuffer();
        }
    }
} 