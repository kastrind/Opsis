#include "Engine3D.h"

Engine3D::Engine3D(HWND hWnd, int width, int height, float fNear, float fFar, float fFov)
			: hWnd(hWnd), width(width), height(height), fNear(fNear), fFar(fFar), fFov(fFov)
{
	fAspectRatio = (float)height / (float)width;
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

mat4x4 Engine3D::getProjMatrix()
{
	return matProj;
}

mat4x4 Engine3D::getTranslMatrix(float x, float y, float z)
{
	mat4x4 matTransl;
	matProj.m[0][0] = 1.0f;
	matProj.m[1][1] = 1.0f;
	matProj.m[2][2] = 1.0f;
	matProj.m[3][3] = 1.0f;
	matProj.m[3][0] = x;
	matProj.m[3][1] = y;
	matProj.m[3][2] = z;
	return matTransl;
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

mat4x4 Engine3D::getRotMatrixY(float fTheta)
{
	mat4x4 matRotY;
	matRotY.m[0][0] = cosf(fTheta);
	matRotY.m[0][2] = sinf(fTheta);
	matRotY.m[2][0] = -sinf(fTheta);
	matRotY.m[1][1] = 1.0f;
	matRotY.m[2][2] = cosf(fTheta);
	matRotY.m[3][3] = 1.0f;
	return matRotY;
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



std::vector<texel> Engine3D::TexturedTriangle(int x1, int y1, float u1, float v1,
								int x2, int y2, float u2, float v2,
								int x3, int y3, float u3, float v3) {

	std::vector<texel> out;

	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(u1, u2);
		std::swap(v1, v2);
	}

	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(u1, u3);
		std::swap(v1, v3);
	}

	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(u2, u3);
		std::swap(v2, v3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;

	float tex_u, tex_v;

	float dax_step = 0, dbx_step = 0, du1_step = 0, dv1_step = 0, du2_step = 0, dv2_step = 0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy2) dv1_step = dv1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
			}
			tex_u = tex_su;
			tex_v = tex_sv;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;

				vec2d tx;
				tx.u = tex_u;
				tx.v = tex_v;

				vec2d p;
				p.u = j;
				p.v = i;

				texel txl;
				txl.t = tx;
				txl.p = p;

				out.push_back(txl);

				t += tstep;
			}

		}

	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;

	if (dy1)
	{

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy2) dv1_step = dv1 / (float)abs(dy1);

		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;

			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
			}
			tex_u = tex_su;
			tex_v = tex_sv;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;

				vec2d tx;
				tx.u = tex_u;
				tx.v = tex_v;

				vec2d p;
				p.u = j;
				p.v = i;

				texel txl;
				txl.t = tx;
				txl.p = p;

				out.push_back(txl);

				t += tstep;
			}

		}
	}

	return out;
}


