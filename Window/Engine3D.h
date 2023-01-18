#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <strstream>
#include <algorithm>
#include "windows.h"

struct mat4x4
{
	float m[4][4] = { 0 };

	inline mat4x4 operator*(mat4x4& in)
	{
		mat4x4 matrix;
		for (int c = 0; c < 4; c++) {
			for (int r = 0; r < 4; r++) {
				matrix.m[r][c] = m[r][0] * in.m[0][c] + m[r][1] * in.m[1][c] + m[r][2] * in.m[2][c] + m[r][3] * in.m[3][c];
			}
		}
		return matrix;
	}

	inline mat4x4 invertRotationOrTranslationMatrix() {
		mat4x4 matrix;
		matrix.m[0][0] = m[0][0]; matrix.m[0][1] = m[1][0]; matrix.m[0][2] = m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m[0][1]; matrix.m[1][1] = m[1][1]; matrix.m[1][2] = m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m[0][2]; matrix.m[2][1] = m[1][2]; matrix.m[2][2] = m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m[3][0] * matrix.m[0][0] + m[3][1] * matrix.m[1][0] + m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m[3][0] * matrix.m[0][1] + m[3][1] * matrix.m[1][1] + m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m[3][0] * matrix.m[0][2] + m[3][1] * matrix.m[1][2] + m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

};

struct vec3d
{
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;

	inline vec3d getNormal(const vec3d& v) {
		vec3d normal;
		normal.x = y * v.z - z * v.y;
		normal.y = z * v.x - x * v.z;
		normal.z = x * v.y - y * v.x;
		return normal;
	}

	inline float getLength() {
		return sqrt(x * x + y * y + z * z);
	}

	inline void normalize() {
		float length = this->getLength();
		x /= length; y /= length; z /= length; w /= length;
	}

	inline float getDotProduct(const vec3d& v) {
		return (x * v.x + y * v.y + z * v.z);
	}

	inline vec3d operator+(const vec3d& in) {
		vec3d out;
		out.x = x + in.x; out.y = y + in.y; out.z = z + in.z; out.w = w + in.w;
		return out;
	}

	inline vec3d operator+(const float& in) {
		vec3d out;
		out.x = x + in; out.y = y + in; out.z = z + in; out.w = w + in;
		return out;
	}

	inline vec3d operator-(const vec3d& in) {
		vec3d out;
		out.x = x - in.x; out.y = y - in.y; out.z = z - in.z; out.w = w - in.w;
		return out;
	}

	inline vec3d operator-(const float& in) {
		vec3d out;
		out.x = x - in; out.y = y - in; out.z = z - in; out.w = w - in;
		return out;
	}

	inline vec3d operator*(const vec3d& in) {
		vec3d out;
		out.x = x * in.x; out.y = y * in.y; out.z = z * in.z; out.w = w * in.w;
		return out;
	}

	inline vec3d operator*(const float& in) {
		vec3d out;
		out.x = x * in; out.y = y * in; out.z = z * in; out.w = w * in;
		return out;
	}

	inline vec3d operator*(const mat4x4& in) {
		vec3d out;
		out.x = x * in.m[0][0] + y * in.m[1][0] + z * in.m[2][0] + w * in.m[3][0];
		out.y = x * in.m[0][1] + y * in.m[1][1] + z * in.m[2][1] + w * in.m[3][1];
		out.z = x * in.m[0][2] + y * in.m[1][2] + z * in.m[2][2] + w * in.m[3][2];
		out.w = x * in.m[0][3] + y * in.m[1][3] + z * in.m[2][3] + w * in.m[3][3];

		return out;
	}

	inline vec3d operator/(const vec3d& in) {
		vec3d out;
		out.x = x / in.x; out.y = y / in.y; out.z = z / in.z; out.w = w / in.w;
		return out;
	}

	inline vec3d operator/(const float& in) {
		vec3d out;
		out.x = x / in; out.y = y / in; out.z = z / in; out.w = w / in;
		return out;
	}

	inline mat4x4 pointAt(vec3d& target, vec3d& up)
	{
		vec3d pos = { x, y, z, w };

		// Calc. new forward direction
		vec3d newForward = target - pos;
		newForward.normalize();

		// Calc. new up direction
		vec3d a = newForward * up.getDotProduct(newForward);
		vec3d newUp = up - a;
		newUp.normalize();

		// New right direction is the cross product
		vec3d newRight = newUp.getNormal(newForward);

		// Construct Dimensioning and Translation Matrix	
		mat4x4 matrix;
		matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
		return matrix;
	}

	inline vec3d intersectPlane(vec3d& planeNormal, vec3d& lineStart, vec3d& lineEnd)
	{
		planeNormal.normalize();
		float planeNormalPointDP = -planeNormal.getDotProduct(*this);
		float lineStartNormalDP = lineStart.getDotProduct(planeNormal);
		float lineEndNormalDP = lineEnd.getDotProduct(planeNormal);
		float t = (-planeNormalPointDP - lineStartNormalDP) / (lineEndNormalDP - lineStartNormalDP);
		vec3d lineStartToEnd = lineEnd - lineStart;
		vec3d lineToIntersect = lineStartToEnd * t;
		return lineStart + lineToIntersect;
	}
};

struct triangle
{
	vec3d p[3] = { 0, 0, 0 };

	unsigned char R = 255; unsigned char G = 255; unsigned char B = 255;

	float luminance = 0.0f;

	inline triangle operator+(const vec3d& in) {
		triangle out;
		out.luminance = luminance; out.R = R; out.G = G; out.B = B;
		out.p[0] = p[0] + in; out.p[1] = p[1] + in; out.p[2] = p[2] + in;
		return out;
	}

	inline triangle operator-(const vec3d& in) {
		triangle out;
		out.luminance = luminance; out.R = R; out.G = G; out.B = B;
		out.p[0] = p[0] - in; out.p[1] = p[1] - in; out.p[2] = p[2] - in;
		return out;
	}

	inline triangle operator*(const vec3d& in) {
		triangle out;
		out.luminance = luminance; out.R = R; out.G = G; out.B = B;
		out.p[0] = p[0] * in; out.p[1] = p[1] * in; out.p[2] = p[2] * in;
		return out;
	}

	inline triangle operator*(const mat4x4& in) {
		triangle out;
		out.luminance = luminance; out.R = R; out.G = G; out.B = B;
		out.p[0] = p[0] * in; out.p[1] = p[1] * in; out.p[2] = p[2] * in;
		return out;
	}

	inline triangle operator/(const vec3d& in) {
		triangle out;
		out.luminance = luminance; out.R = R; out.G = G; out.B = B;
		out.p[0] = p[0] / in; out.p[1] = p[1] / in; out.p[2] = p[2] / in;
		return out;
	}

	inline int clipAgainstPlane(vec3d planePoint, vec3d planeNormal, triangle& outTriangle1, triangle& outTriangle2)
	{
		// Make sure plane normal is indeed normal
		planeNormal.normalize();

		// Return signed shortest distance from point to plane, plane normal must be normalised
		auto dist = [&](vec3d& p)
		{
			return (planeNormal.getDotProduct(p) - planeNormal.getDotProduct(planePoint));
		};

		// Create two temporary storage arrays to classify points either side of plane
		// If distance sign is positive, point lies on "inside" of plane
		vec3d* inside_points[3];  int nInsidePointCount = 0;
		vec3d* outside_points[3]; int nOutsidePointCount = 0;

		// Get signed distance of each point in triangle to plane
		float d0 = dist(p[0]);
		float d1 = dist(p[1]);
		float d2 = dist(p[2]);

		//wchar_t s[256];
		//swprintf_s(s, 256, L"   d0= %3.2f d1= %3.2f d2= %3.2f", d0, d1, d2);
		//OutputDebugString(s);

		if (d0 >= 0) { inside_points[nInsidePointCount++] = &p[0]; }
		else { outside_points[nOutsidePointCount++] = &p[0]; }
		if (d1 >= 0) { inside_points[nInsidePointCount++] = &p[1]; }
		else { outside_points[nOutsidePointCount++] = &p[1]; }
		if (d2 >= 0) { inside_points[nInsidePointCount++] = &p[2]; }
		else { outside_points[nOutsidePointCount++] = &p[2]; }

		// Now classify triangle points, and break the input triangle into 
		// smaller output triangles if required. There are four possible
		// outcomes...
		if (nInsidePointCount == 0)
		{
			// All points lie on the outside of plane, so clip whole triangle
			// It ceases to exist
			return 0; // No returned triangles are valid
		}

		if (nInsidePointCount == 3)
		{
			// All points lie on the inside of plane, so do nothing
			// and allow the triangle to simply pass through
			outTriangle1 = *this;
			return 1; // Just the one returned original triangle is valid
		}

		if (nInsidePointCount == 1 && nOutsidePointCount == 2)
		{
			// Triangle should be clipped. As two points lie outside
			// the plane, the triangle simply becomes a smaller triangle

			// Copy appearance info to new triangle
			outTriangle1.luminance = luminance;
			outTriangle1.R = 255; outTriangle1.G = 1; outTriangle1.B = 1;

			// The inside point is valid, so keep that...
			outTriangle1.p[0] = *inside_points[0];

			// but the two new points are at the locations where the 
			// original sides of the triangle (lines) intersect with the plane
			outTriangle1.p[1] = planePoint.intersectPlane(planeNormal, *inside_points[0], *outside_points[0]);
			outTriangle1.p[2] = planePoint.intersectPlane(planeNormal, *inside_points[0], *outside_points[1]);

			return 1; // Return the newly formed single triangle
		}

		if (nInsidePointCount == 2 && nOutsidePointCount == 1)
		{
			// Triangle should be clipped. As two points lie inside the plane,
			// the clipped triangle becomes a "quad". Fortunately, we can
			// represent a quad with two new triangles

			// Copy appearance info to new triangles
			outTriangle1.luminance = this->luminance;
			outTriangle2.luminance = this->luminance;

			outTriangle1.R = 1; outTriangle1.G = 255; outTriangle1.B = 1;
			outTriangle2.R = 1; outTriangle2.G = 1; outTriangle2.B = 255;

			// The first triangle consists of the two inside points and a new
			// point determined by the location where one side of the triangle
			// intersects with the plane
			outTriangle1.p[0] = *inside_points[0];
			outTriangle1.p[1] = *inside_points[1];
			outTriangle1.p[2] = planePoint.intersectPlane(planeNormal, *inside_points[0], *outside_points[0]);

			// The second triangle is composed of one of the inside points, a
			// new point determined by the intersection of the other side of the 
			// triangle and the plane, and the newly created point above
			outTriangle2.p[0] = *inside_points[1];
			outTriangle2.p[1] = outTriangle1.p[2];
			outTriangle2.p[2] = planePoint.intersectPlane(planeNormal, *inside_points[1], *outside_points[0]);

			return 2; // Return two newly formed triangles which form a quad
		}


		return -1;
	}

};

struct mesh
{
	std::vector<triangle> tris;
};

class Engine3D
{
	public:

		Engine3D(HWND hWnd, int width=320, int height=240, float fNear = 0.1f, float fFar = 1000.0f, float fFov = 90.0f);

		std::thread Start();

		// User MUST OVERRIDE THESE!
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fElapsedTime) = 0;

		// Optional for clean up 
		virtual bool OnUserDestroy();

		bool loadObj(std::string sFilename, mesh& outMesh);

		mat4x4 getProjMatrix();

		mat4x4 getTranslMatrix(float x, float y, float z);

		mat4x4 getRotMatrixX(float fTheta);

		mat4x4 getRotMatrixY(float fTheta);

		mat4x4 getRotMatrixZ(float fTheta);

		std::atomic<bool> bAtomActive;

		std::atomic<bool> bLockRaster = false;

		float fElapsedTime;

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

