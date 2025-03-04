#pragma once
#include "Core/MathDefinitions.h"
#include "Rendering/FrustrumCulling.h"
#include "Components/Transform.h"

#include <array>

struct Transform;

struct Camera
{
public:
	enum Faces : uint32_t {
		NEAR_FACE   = 0,
		FAR_FACE    = 1,
		TOP_FACE    = 2,
		BOTTOM_FACE = 3,
		RIGHT_FACE  = 4,
		LEFT_FACE   = 5,
		FACE_COUNT  = 6 // Number of faces
	};
	
	Camera() : m_fov(0.f), m_nearPlane(0.f), m_farPlane(0.f), m_aspectRatio(0.f), m_renderTarget(0) {}
	Camera(float fov, float aspectRatio, float nearPlane = 0.01f, float farPlane = 1000.f);
	~Camera();
	
	const Mat4f& GetProjection() { return m_projection; }

	void SetFrustrumTest(Transform* transform) {
		m_editorFrustrum = transform;
	}

	void SetFov(float fov);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);
	void SetAspectRatio(float aspectRatio);

	uint32_t& RenderTarget() {
		return m_renderTarget;
	}

	const Planef GetFrustrumFace(Faces face) const {
		return m_frustrum[face];
	}

private:
	friend class Editor;
	friend class Renderer;
	friend class RenderManager;

	void ConstructProjection();
	void ConstructFrustrum(const Mat4f& transform, const Vect3f& position);

	uint32_t m_renderTarget;

	std::array<Planef, FACE_COUNT> m_frustrum;

	float m_fov;
	float m_nearPlane;
	float m_farPlane;
	float m_aspectRatio;

	Transform* m_editorFrustrum = nullptr;
	Mat4f      m_projection;
};

