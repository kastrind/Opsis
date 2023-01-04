#include "Engine3D.h"

Engine3D::Engine3D(HWND hWnd, int width, int height, float fNear, float fFar, float fFov)
			: hWnd(hWnd), width(width), height(height), fNear(fNear), fFar(fFar), fFov(fFov)
{
	fAspectRatio = (float)width / (float)height;
	fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	// Projection Matrix
	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

}

std::thread Engine3D::Start() {
	bAtomActive = true;
	bLockRaster = false;

	// Start the thread
	std::thread t = std::thread::thread(&Engine3D::EngineThread, this);
	
	return t;
}

void Engine3D::EngineThread()
{
	// Create user resources as part of this thread
	if (!OnUserCreate())
		bAtomActive = false;

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	while (bAtomActive)
	{
		// Run as fast as possible
		while (bAtomActive)
		{

			// Handle Timing
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			fElapsedTime = elapsedTime.count();

			// Handle Frame Update
			if (!OnUserUpdate(fElapsedTime))
				bAtomActive = false;

			if (!bLockRaster) RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);

		}

		if (OnUserDestroy())
		{
			// User has permitted destroy, so exit and clean up

		}
		else
		{
			// User denied destroy for some reason, so continue running
			bAtomActive = true;
		}
	}
}

// Optional for clean up 
bool Engine3D::OnUserDestroy()
{
	return true;
}

void Engine3D::MultiplyMatrixVector(vec3d& in, vec3d& out, mat4x4& m) {
	out.x = in.x * m.m[0][0] + in.y * m.m[1][0] + in.z * m.m[2][0] + m.m[3][0];
	out.y = in.x * m.m[0][1] + in.y * m.m[1][1] + in.z * m.m[2][1] + m.m[3][1];
	out.z = in.x * m.m[0][2] + in.y * m.m[1][2] + in.z * m.m[2][2] + m.m[3][2];
	float w = in.x * m.m[0][3] + in.y * m.m[1][3] + in.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f) {
		out.x /= w; out.y /= w; out.z /= w;
	}
}

mat4x4 Engine3D::getProjMatrix()
{
	return matProj;
}

mat4x4 Engine3D::getRotMatrixZ(float fTheta)
{
	mat4x4 matRotZ;
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;
	return matRotZ;
}

mat4x4 Engine3D::getRotMatrixX(float fTheta)
{
	mat4x4 matRotX;
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta * 0.5f);
	matRotX.m[1][2] = sinf(fTheta * 0.5f);
	matRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matRotX.m[2][2] = cosf(fTheta * 0.5f);;
	matRotX.m[3][3] = 1;
	return matRotX;
}

bool Engine3D::loadObj(std::string sFilename, mesh& outMesh) {
	std::ifstream f(sFilename);
	if (!f.is_open())
		return false;

	std::vector<vec3d> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;
		if (line[0] == 'v')
		{
			vec3d v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			outMesh.tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
		}
	}
	return false;

}

