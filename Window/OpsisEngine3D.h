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
    bool bMouseRight = false;
    bool bMouseLeft = false;
    bool bMouseUp = false;
    bool bMouseDown = false;

    vec3d light = { 0.0f, 0.0f, -1.0f };
private:

    mesh meshCube;

    float fTheta = 0;

    vec3d vCamera;
    vec3d vForward;
    vec3d vUp;
    vec3d vLookDir;
    vec3d vTarget;

    mat4x4 matCameraRotY90CW = getRotMatrixY(-1.57075f);
    mat4x4 matCameraRotY90CCW = getRotMatrixY(1.57075f);
    vec3d vRight;
    vec3d vLeft;

    float fYaw = 0;
    float fPitch = 0;



};
