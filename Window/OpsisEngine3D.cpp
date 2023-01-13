#include "OpsisEngine3D.h"

//std::vector<triangle> myEngine3D::trianglesToProject;


OpsisEngine3D::OpsisEngine3D(HWND hWnd, int width, int height)
    : Engine3D(hWnd, width, height) {
}
bool OpsisEngine3D::OnUserCreate()
{
    loadObj("assets/axis.obj", meshCube);

    return true;
}
bool OpsisEngine3D::OnUserUpdate(float fElapsedTime)
{
    //fTheta += 1.0f * fElapsedTime;

    if (bAKeyHeld)
    {
        vRight = vForward * matCameraRotY90CCW;
        vCamera = vCamera + vRight;
    }
    if (bDKeyHeld)
    {
        vLeft = vForward * matCameraRotY90CW;
        vCamera = vCamera + vLeft;
    }

    if (bWKeyHeld)
    {
        vCamera = vCamera + vForward;
    }
    if (bSKeyHeld)
    {
        vCamera = vCamera - vForward;
    }

    if (bMouseLeft)
    {
        fYaw += 2.0f * fElapsedTime;
    }
    if (bMouseRight)
    {
        fYaw -= 2.0f * fElapsedTime;
    }

    if (bMouseUp)
    {
        if (vForward.z < 0)
        {
            fPitch += 2.0f * fElapsedTime;
            fPitch = min(fPitch, 1.57f);
        }
        else
        {
            fPitch -= 2.0f * fElapsedTime;
            fPitch = max(fPitch, -1.57f);
        }
    }
    if (bMouseDown)
    {
        if (vForward.z < 0)
        {
            fPitch -= 2.0f * fElapsedTime;
            fPitch = max(fPitch, -1.57f);
        }
        else
        {
            fPitch += 2.0f * fElapsedTime;
            fPitch = min(fPitch, 1.57f);
        }
    }

    mat4x4 matRotZ = getRotMatrixZ(fTheta);
    mat4x4 matRotX = getRotMatrixX(fTheta);


    // Create "point at" matrix for camera
    vUp = { 0, 1, 0 };
    vTarget = { 0, 0, 1 };
    vForward = vLookDir * 8.0f * fElapsedTime;
    mat4x4 matCameraRotY = getRotMatrixY(fYaw);
    mat4x4 matCameraRotX = getRotMatrixX(fPitch);
    vLookDir = (vTarget * matCameraRotY) * matCameraRotX;
    vTarget = vCamera + vLookDir;
    mat4x4 matCamera = vCamera.pointAt(vTarget, vUp);

    light = { 0.0f, 0.0f, -1.0f };

    mat4x4 matView = matCamera.invertRotationOrTranslationMatrix();

    std::vector<triangle> trianglesToProject;

    // Project triangles into camera view
    for (auto &tri : meshCube.tris)
    {
        triangle triRotatedZ, triRotatedZX, triTranslated, triViewed, triProjected;

        // Rotate in Z
        triRotatedZ = tri * matRotZ;

        // Rotate in X
        triRotatedZX = triRotatedZ * matRotX;

        // Translate further along Z
        triTranslated = triRotatedZX;
        triTranslated = triRotatedZX + vec3d{ 0.0f, 0.0f, 40.0f };

        // Convert to view space
        triViewed = triTranslated * matView;

        vec3d normal, line1, line2;
        line1 = triViewed.p[1] - triViewed.p[0];
        line2 = triViewed.p[2] - triViewed.p[0];

        normal = line1.getNormal(line2);
        normal.normalize();

        vec3d camLine;
        camLine = triTranslated.p[0] - vCamera;

        // Only render visible triangles, i.e. whose normals have negative dot product with the camera line
        if (normal.getDotProduct(camLine) < 0.0f)
        {
            // Illumination
            vec3d lightLine;
            lightLine = triViewed.p[0] - light;
            lightLine.normalize();

            triViewed.luminance = max(0.2f, abs(normal.getDotProduct(lightLine)));
            
            // Clip Viewed Triangle against near plane, this could form two additional triangles. 
            int nClippedTriangles = 0;
            triangle clipped[2];
            vec3d nearPlane = { 0.0f, 0.0f, 0.1f };
            vec3d nearPlaneNormal = { 0.0f, 0.0f, 1.0f };
            nClippedTriangles = triViewed.clipAgainstPlane(nearPlane, nearPlaneNormal, clipped[0], clipped[1]);

            // We may end up with multiple triangles form the clip, so project as required
            for (int n = 0; n < nClippedTriangles; n++)
            {
                // Project triangles from 3D -> 2D
                triProjected = clipped[n] * matProj;
                if (triProjected.p[0].w > 0) triProjected.p[0] = triProjected.p[0] / triProjected.p[0].w;
                if (triProjected.p[1].w > 0) triProjected.p[1] = triProjected.p[1] / triProjected.p[1].w;
                if (triProjected.p[2].w > 0) triProjected.p[2] = triProjected.p[2] / triProjected.p[2].w;

                triProjected.luminance = clipped[n].luminance;

                //wchar_t s[256];
                //swprintf_s(s, 256, L"%3.2f", triProjected.luminance);
                //OutputDebugString(s);

                // Convert to screen coords: -1...+1 => 0...2 and adjust it with halved screen dimensions
                triProjected = triProjected + vec3d{ 1, 1, 0, 0 };
                triProjected = triProjected * vec3d{ 0.5f * (float)width, 0.5f * (float)height, 1, 1 };
                triProjected = triProjected * vec3d{ 1, -1, 1, 1 };
                triProjected = triProjected + vec3d{ 0, (float)height, 0, 0 };

                trianglesToProject.push_back(triProjected);
            }
        }
    }

    //Sort triangles from back to front
    std::sort(trianglesToProject.begin(), trianglesToProject.end(), [](triangle& t1, triangle& t2)
        {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return z1 > z2;
        });


    int nNewTriangles = 1;
    triangle clipped[2];
    std::list<triangle> listTriangles;
    vec3d planeTop = { 1.0f, 0.0f, 0.0f };
    vec3d planeTopNormal = { 0.0f, 1.0f, 0.0f };

    vec3d planeBottom = { 0.0f, (float)height-1, 0.0f};
    vec3d planeBottomNormal = { 0.0f, -1.0f, 0.0f };

    vec3d planeLeft = { 0.0f, 1.0f, 0.0f };
    vec3d planeLeftNormal = { 1.0f, 0.0f, 0.0f };

    vec3d planeRight = { (float)width-1, 0.0f, 0.0f };
    vec3d planeRightNormal = { -1.0f, 0.0f, 0.0f };
    
    for (auto& tri : trianglesToProject)
    {
        // Clip triangles against all four screen edges, this could yield
        // a bunch of triangles, so create a queue that we traverse to 
        //  ensure we only test new triangles generated against planes
        nNewTriangles = 1;

        // Add initial triangle
        listTriangles.push_back(tri);

        for (int p = 0; p < 4; p++)
        {
            int nTrisToAdd = 0;
            while (nNewTriangles > 0)
            {
                // Take triangle from front of queue
                triangle test = listTriangles.front();
                listTriangles.pop_front();
                nNewTriangles--;

                // Clip it against a plane. We only need to test each 
                // subsequent plane, against subsequent new triangles
                // as all triangles after a plane clip are guaranteed
                // to lie on the inside of the plane. I like how this
                // comment is almost completely and utterly justified
                switch (p)
                {
                case 0:	nTrisToAdd = test.clipAgainstPlane(planeTop, planeTopNormal, clipped[0], clipped[1]); break;
                //case 0: nTrisToAdd = 1; clipped[0] = test; break;
                case 1:	nTrisToAdd = test.clipAgainstPlane(planeBottom, planeBottomNormal, clipped[0], clipped[1]); break;
                //case 1: nTrisToAdd = 1; clipped[0] = test; break;
                case 2:	nTrisToAdd = test.clipAgainstPlane(planeLeft, planeLeftNormal, clipped[0], clipped[1]); break;
                //case 2: nTrisToAdd = 1; clipped[0] = test; break;
                case 3:	nTrisToAdd = test.clipAgainstPlane(planeRight, planeRightNormal, clipped[0], clipped[1]); break;
                //case 3: nTrisToAdd = 1; clipped[0] = test; break;
                }

                // Clipping may yield a variable number of triangles, so
                // add these new ones to the back of the queue for subsequent
                // clipping against next planes
                for (int w = 0; w < nTrisToAdd; w++)
                    listTriangles.push_back(clipped[w]);
            }
            nNewTriangles = listTriangles.size();
        }
    }

    if (!bLockRaster)
    {
        trianglesToRaster = listTriangles;
        //trianglesToRaster = trianglesToProject;

    }

    return true;
}
