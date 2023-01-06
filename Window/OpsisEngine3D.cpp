#include "OpsisEngine3D.h"

//std::vector<triangle> myEngine3D::trianglesToProject;


OpsisEngine3D::OpsisEngine3D(HWND hWnd, int width, int height)
    : Engine3D(hWnd, width, height) {
}
bool OpsisEngine3D::OnUserCreate()
{
    //meshCube.tris = {
    //    // SOUTH
    //    {0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f},
    //    {0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f},

    //    //// EAST
    //    {1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 1.0f},
    //    {1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f},

    //    //// NORTH
    //    {1.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 1.0f},
    //    {1.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f},

    //    //// WEST
    //    {0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f},
    //    {0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 0.0f,       0.0f, 0.0f, 0.0f},

    //    //// TOP
    //    {0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f},
    //    {0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 0.0f},

    //    //// BOTTOM
    //    {1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 0.0f},
    //    {1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f},
    //};
    loadObj("assets/teapot.obj", meshCube);

    return true;
}
bool OpsisEngine3D::OnUserUpdate(float fElapsedTime)
{
    if (bLockRaster) return true;

    fTheta += 1.0f * fElapsedTime;

    std::vector<triangle> trianglesToProject;

    // Project triangles into camera view
    for (auto &tri : meshCube.tris)
    {
        triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

        // Rotate in Z
        mat4x4 matRotZ = getRotMatrixZ(fTheta);
        triRotatedZ = tri * matRotZ;

        // Rotate in X
        mat4x4 matRotX = getRotMatrixX(fTheta);
        triRotatedZX = triRotatedZ * matRotX;

        // Translate further along Z
        triTranslated = triRotatedZX;
        triTranslated = triRotatedZX + vec3d{ 0.0f, 0.0f, 10.0f };

        vec3d normal, line1, line2;
        line1 = triTranslated.p[1] - triTranslated.p[0];
        line2 = triTranslated.p[2] - triTranslated.p[0];

        normal = line1.getNormal(line2);
        normal.normalize();

        vec3d camLine;
        camLine = triTranslated.p[0] - vCamera;

        // Only render visible triangles, i.e. whose normals have negative dot product with the camera line
        if (normal.getDotProduct(camLine) < 0.0f)
        {
            // Illumination
            vec3d lightLine;
            lightLine = triTranslated.p[0] - light;
            lightLine.normalize();

            triTranslated.luminance = abs(normal.getDotProduct(lightLine));

            // Project triangles from 3D -> 2D
            triTranslated.p[0].w = 1;
            triTranslated.p[1].w = 1;
            triTranslated.p[2].w = 1;
            triProjected = triTranslated * matProj;
            if (triProjected.p[0].w>0) triProjected.p[0] = triProjected.p[0] / triProjected.p[0].w;
            if (triProjected.p[1].w>0) triProjected.p[1] = triProjected.p[1] / triProjected.p[1].w;
            if (triProjected.p[2].w>0) triProjected.p[2] = triProjected.p[2] / triProjected.p[2].w;

            // Convert to screen coords: -1...+1 => 0...2 and adjust it with halved screen dimensions
            triProjected = triProjected + vec3d{ 1, 1, 0, 0 };
            triProjected = triProjected * vec3d{ 0.5f * (float)width, 0.5f * (float)height, 1, 1 };

            triProjected.luminance = triTranslated.luminance;

            trianglesToProject.push_back(triProjected);
        }

    }

    //Sort triangles from back to front
    std::sort(trianglesToProject.begin(), trianglesToProject.end(), [](triangle& t1, triangle& t2)
        {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return z1 > z2;
        });

    if (!bLockRaster)
    {
        trianglesToRaster = trianglesToProject;
    }

    return true;
}
