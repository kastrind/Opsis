#include "Engine3D.h"

class OpsisEngine3D : public Engine3D {

public:
    
    OpsisEngine3D(HWND hWnd, int width, int height);

    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

    std::vector<triangle> trianglesToRaster;
    
private:

    mesh meshCube;

    float fTheta = 0;

    vec3d vCamera;

    vec3d light = { 0.0f, 0.0f, -1.0f };

};
