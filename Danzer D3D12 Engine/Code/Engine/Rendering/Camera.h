#pragma once

#include "Core/MathDefinitions.h"
#include "Rendering/FrustrumCulling.h"
#include "Components/Transform.h"

#include <array>

struct Camera : public BaseComponent
{
	COMP_FUNC(Camera)
public:
	enum Projection : uint8_t{
		PERSPECTIVE,
		ORTHOGRAPHIC,
		COUNT
	};

	enum Faces : uint32_t {
		NEAR_FACE   = 0,
		FAR_FACE    = 1,
		TOP_FACE    = 2,
		BOTTOM_FACE = 3,
		RIGHT_FACE  = 4,
		LEFT_FACE   = 5,
		FACE_COUNT  = 6 // Number of faces
	};
	
	Camera();
	~Camera();
	
	const Mat4f& GetProjection() { return m_projection[m_currentProj]; }

	void SetFrustrumTest(Transform* transform) {
		m_editorFrustrum = transform;
	}

	void SetFov(float fov);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);
	void SetAspectRatio(float aspectRatio);

	void SetViewWidth(float width);
	void SetViewHeight(float height);
	void SetNearZ(float nearZ);
	void SetFarZ(float farZ);

	uint32_t& RenderTarget() {
		return m_renderTarget;
	}
	const Planef GetFrustrumFace(Faces face) const {
		return m_frustrum[face];
	}

	void DisplayInEditor(const Entity entity) override;
	
	void SetCameraProjection(const Projection proj) {
		if (proj == Projection::COUNT)
			m_currentProj = PERSPECTIVE;
		else
			m_currentProj = proj;
	}

private:
	friend class Editor;
	friend class Renderer;
	friend class RenderManager;

	void ConstructPerspective();
	void ConstructOrthographic();
	void ConstructFrustrum(const Mat4f& transform, const Vect3f& position);

	uint32_t m_renderTarget;

	std::array<Planef, FACE_COUNT> m_frustrum;

	// Perspective Values
	float m_fov;
	float m_nearPlane;
	float m_farPlane;
	float m_aspectRatio;

	// Orthographic Values
	float m_viewWidth;
	float m_viewHeight;
	float m_nearZ;
	float m_farZ;

	Transform* m_editorFrustrum = nullptr;
	
	Projection m_currentProj;
	Mat4f      m_projection[Projection::COUNT];
};
REGISTER_COMPONENT(Camera)

