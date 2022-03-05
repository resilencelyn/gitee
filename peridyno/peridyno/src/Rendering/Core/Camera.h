#pragma once

#include <glm/glm.hpp>

// TODO: we prefer not depend on Core library
#include <Vector.h>
#include <Quat.h>

#include "Topology/Primitive3D.h"

namespace dyno
{
	typedef Quat<float> Quat1f;

	class Camera {
	public:
		Camera() {};
		~Camera() {};

		virtual glm::mat4 getViewMat() = 0;
		virtual glm::mat4 getProjMat() = 0;

		virtual void rotateToPoint(float x, float y) = 0;
		virtual void translateToPoint(float x, float y) = 0;
		virtual void zoom(float amount) = 0;

		virtual void registerPoint(float x, float y) = 0;

		void setWidth(int width) { mViewportWidth = width; }
		void setHeight(int height) { mViewportHeight = height; }

		void setClipNear(float zNear) { mNear = zNear; }
		void setClipFar(float zFar) { mFar = zFar; }

		int viewportWidth() const {	return mViewportWidth; }
		int viewportHeight() const { return mViewportHeight; }

		float clipNear() const { return mNear; }
		float clipFar() const { return mFar; }

		virtual void setEyePos(const Vec3f& p) = 0;
		virtual void setTargetPos(const Vec3f& p) = 0;

		TRay3D<float> castRayInWorldSpace(float x, float y);

	protected:
		float mNear = 0.01f;
		float mFar = 10.0f;
		float mFov = 0.0f;

		int mViewportWidth;
		int mViewportHeight;
	};

}

