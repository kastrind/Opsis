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

		Engine3D(HWND hWnd, int width=320, int height=240, float fNear=0.1f, float fFar = 1000.0f, float fFov = 90.0f);

		std::thread Start();

		// User MUST OVERRIDE THESE!
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fElapsedTime) = 0;

		// Optional for clean up 
		virtual bool OnUserDestroy();

		void MultiplyMatrixVector(vec3d &in, vec3d &o, mat4x4 &m);
		mat4x4 getProjMatrix();
		mat4x4 getRotMatrixZ(float fTheta);
		mat4x4 getRotMatrixX(float fTheta);

		std::atomic<bool> bAtomActive;

	protected:

		HWND hWnd;
		int width;
		int height;
		float fNear;
		float fFar;
		float fFov;
		float fAspectRatio;
		float fFovRad;

		mat4x4 matProj;
	
	private:

		void EngineThread();

};

