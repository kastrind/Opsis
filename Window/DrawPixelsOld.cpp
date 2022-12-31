#include "ConsoleCanvas.h"
#include "Engine3D.h"

class myEngine3D : public Engine3D {
public:
    myEngine3D(int w, int h, std::wstring appName)
    : Engine3D(w, h, appName) {
    }
    bool OnUserCreate() override
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

        // Projection Matrix

        float fNear = 0.1;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectRatio = (float)cCanvas.ScreenHeight() / (float)cCanvas.ScreenWidth();
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

        matProj.m[0][0] = fAspectRatio * fFovRad;
        matProj.m[1][1] = fFovRad;
        matProj.m[2][2] = fFar / (fFar - fNear);
        matProj.m[3][2] = ( - fFar * fNear ) / (fFar - fNear);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        //cCanvas.Clear();
        // Draw Triangles
        
        for (auto tri : meshCube.tris)
        {
            triangle triProjected;
            MultiplyMatrixVector(tri.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(tri.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(tri.p[2], triProjected.p[2], matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= 0.5f * (float)cCanvas.ScreenWidth();
            triProjected.p[0].y *= 0.5f * (float)cCanvas.ScreenWidth();
            triProjected.p[1].x *= 0.5f * (float)cCanvas.ScreenWidth();
            triProjected.p[1].y *= 0.5f * (float)cCanvas.ScreenWidth();
            triProjected.p[2].x *= 0.5f * (float)cCanvas.ScreenWidth();
            triProjected.p[2].y *= 0.5f * (float)cCanvas.ScreenWidth();

            
            cCanvas.DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                                 triProjected.p[1].x, triProjected.p[1].y,
                                 triProjected.p[2].x, triProjected.p[2].y, cCanvas.red);
            
        }
        //cCanvas.DrawLine(0, 350, 300, 350, cCanvas.yellow); // d>=1
        //cCanvas.DrawLine(300, 0, 300, 350, cCanvas.orange); // d>=1
        cCanvas.DrawLine(300 + rand()%100, 100, 100, 400, cCanvas.green); // d>=1
        cCanvas.DrawLine2(301, 101, 101, 401, cCanvas.red);

        /*
        cCanvas.DrawLine(100+rand()%20, 200, 100, 300, cCanvas.red); // d=inf.
        cCanvas.DrawLine(100 + rand() % 20, 200, 200, 200, cCanvas.yellow); // d=0
        cCanvas.DrawLine(100 + rand() % 20, 200, 200, 300, cCanvas.orange); // d=1
        cCanvas.DrawLine(210 + rand() % 20, 200, 310, 201, cCanvas.blue); // d<1
        cCanvas.DrawLine(210 + rand() % 20, 210, 310, 212, cCanvas.blue); // d<1
        cCanvas.DrawLine(210 + rand() % 20, 220, 310, 232, cCanvas.blue); // d<1
        cCanvas.DrawLine(320 + rand() % 20, 200, 321, 300, cCanvas.green); // d>=1
        cCanvas.DrawLine(330 + rand() % 20, 200, 332, 300, cCanvas.green); // d>=1
        cCanvas.DrawLine(340 + rand() % 20, 200, 362, 300, cCanvas.green); // d>=1
        */
        
        return true;
    }

private:

    mesh meshCube;
    mat4x4 matProj;


};
/*
int main()
{
    myEngine3D* myEng3D = new myEngine3D(640, 480, L"Unnamed");
    myEng3D->Start();
    return 0;
}
*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
