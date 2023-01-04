#include "Engine3D.h"

class OpsisEngine3D : public Engine3D {

public:
    
    OpsisEngine3D(HWND hWnd, int width, int height);

    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

    std::vector<triangle> trianglesToProject;
    
private:

    mesh meshCube;

    float fTheta = 0;

    vec3d vCamera;

};
