#include "OpsisEngine3D.h"

//std::vector<triangle> myEngine3D::trianglesToProject;


OpsisEngine3D::OpsisEngine3D(HWND hWnd, int width, int height)
    : Engine3D(hWnd, width, height) {
}
bool OpsisEngine3D::OnUserCreate()
{
    meshCube.tris = {
        // SOUTH
        {0.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 0.0f, 0.0f},

        // EAST
        {1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, 1.0f},

        // NORTH
        {1.0f, 0.0f, 1.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 0.0f, 1.0f},

        // WEST
        {0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 1.0f,       0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 0.0f,       0.0f, 0.0f, 0.0f},

        // TOP
        {0.0f, 1.0f, 0.0f,      0.0f, 1.0f, 1.0f,       1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 1.0f, 0.0f},

        // BOTTOM
        {1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f},
    };

    return true;
}
bool OpsisEngine3D::OnUserUpdate(float fElapsedTime)
{
    fTheta += 1.0f * fElapsedTime;

    std::vector<triangle> newTrianglesToProject;
    // Project Triangles
    for (auto tri : meshCube.tris)
    {
        triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

        // Rotate in Z
        mat4x4 matRotZ = getRotMatrixZ(fTheta);
        MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
        MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
        MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

        // Rotate in X
        mat4x4 matRotX = getRotMatrixX(fTheta);
        MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

        // Translate further along Z
        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

        vec3d normal, line1, line2;
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;
        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

        normal.x = line1.y * line2.z - line1.z * line2.y;
        normal.y = line1.z * line2.x - line1.x * line2.z;
        normal.z = line1.x * line2.y - line1.y * line2.x;

        float normalLength = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x /= normalLength; normal.y /= normalLength; normal.z /= normalLength;

        vec3d camLine;
        camLine.x = triTranslated.p[0].x - vCamera.x;
        camLine.y = triTranslated.p[0].y - vCamera.y;
        camLine.z = triTranslated.p[0].z - vCamera.z;

        float camLineNormalDotProduct = normal.x * camLine.x + normal.y * camLine.y + normal.z * camLine.z;

        // Only render visible triangles, i.e. whose normals have negative dot product with the camera line
        if (camLineNormalDotProduct < 0.0f)
        {
            // Illumination
            vec3d lightLine;
            lightLine.x = triTranslated.p[0].x - light.x;
            lightLine.y = triTranslated.p[0].y - light.y;
            lightLine.z = triTranslated.p[0].z - light.z;

            float lightLength = sqrt(lightLine.x * lightLine.x + lightLine.y * lightLine.y + lightLine.z * lightLine.z);

            lightLine.x /= lightLength; lightLine.y /= lightLength; lightLine.z /= lightLength;

            float lightLineNormalDotProduct = lightLine.x * normal.x + lightLine.y * normal.y + lightLine.z * normal.z;

            triTranslated.luminance = lightLineNormalDotProduct;

            // Project triangles from 3D -> 2D
            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= 0.5f * (float)width;
            triProjected.p[0].y *= 0.5f * (float)height;
            triProjected.p[1].x *= 0.5f * (float)width;
            triProjected.p[1].y *= 0.5f * (float)height;
            triProjected.p[2].x *= 0.5f * (float)width;
            triProjected.p[2].y *= 0.5f * (float)height;

            triProjected.luminance = abs(triTranslated.luminance);

            newTrianglesToProject.push_back(triProjected);
        }

    }
    trianglesToProject = newTrianglesToProject;

    return true;
}
