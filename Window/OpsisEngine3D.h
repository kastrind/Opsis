#include "Engine3D.h"

class OpsisEngine3D : public Engine3D {

public:
    
    OpsisEngine3D(HWND hWnd, int width, int height);

    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

    std::vector<triangle> trianglesToRaster;

    bool bWKeyHeld = false;
    bool bSKeyHeld = false;
    bool bAKeyHeld = false;
    bool bDKeyHeld = false;
    
private:

    mesh meshCube;

    float fTheta = 0;

    vec3d vCamera;

    vec3d vLookDir;

    vec3d light = { 0.0f, 0.0f, -1.0f };

};
