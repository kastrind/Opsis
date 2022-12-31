#include "Engine3D.h"

Engine3D::Engine3D(int w, int h, std::wstring appName=L"Default")
	: cCanvas(w, h) {
	Engine3D::appName = appName;
}

void Engine3D::Start() {
	bAtomActive = true;

	// Start the thread
	std::thread t = std::thread::thread(&Engine3D::EngineThread, this);

	// Wait for thread to be exited
	t.join();
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
			float fElapsedTime = elapsedTime.count();


			// Handle Frame Update
			if (!OnUserUpdate(fElapsedTime))
				bAtomActive = false;

			// Update Title & Present Screen Buffer
			wchar_t s[256];
			swprintf_s(s, 256, L"%s - FPS: %3.2f", appName.c_str(), 1.0f / fElapsedTime);
			SetConsoleTitle(s);

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

