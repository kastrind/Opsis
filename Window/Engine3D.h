#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include "ConsoleCanvas.h"

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mesh
{
	std::vector<triangle> tris;
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class Engine3D
{
	public:

		Engine3D(int w, int h, std::wstring appName);

		void Start();

		// User MUST OVERRIDE THESE!
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fElapsedTime) = 0;

		// Optional for clean up 
		virtual bool OnUserDestroy();

		void MultiplyMatrixVector(vec3d &in, vec3d &o, mat4x4 &m);

		std::wstring appName;

		ConsoleCanvas cCanvas;
	
	private:

		std::atomic<bool> bAtomActive;

		void EngineThread();

};

