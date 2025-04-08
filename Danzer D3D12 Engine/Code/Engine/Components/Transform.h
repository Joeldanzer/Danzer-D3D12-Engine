#pragma once
#include "Core/MathDefinitions.h"
#include "ComponentRegister.h"

// Transform entity. Required component to render and update in scene
struct Transform : public BaseComponent {
	COMP_FUNC(Transform)

public:
	Transform() : 
	m_position(0.f, 0.f, 0.f),
	m_scale(1.f, 1.f, 1.f),
	m_rotation(0.f ,0.f ,0.f, 1.f)
	{}

	Vect3f m_position;
	Quat4f m_rotation;
	Vect3f m_scale;

	Mat4f& Local() {
		return m_local;
	}
	Mat4f& Last() {
		return m_last;
	}
	Mat4f& World() {
		return m_world;
	}
	const Mat4f GetWorld() {
		return m_world;
	}

	void SetTransform(const Mat4f mat) {
		DirectX::XMVECTOR s, q, p;
		DirectX::XMMatrixDecompose(&s, &q, &p, mat);
		m_scale    = { s.m128_f32[0], s.m128_f32[1], s.m128_f32[2] };
		m_rotation = { q.m128_f32[0], q.m128_f32[1], q.m128_f32[2], q.m128_f32[3] };
		m_position = { p.m128_f32[0], p.m128_f32[1], p.m128_f32[2] };
	}

	void SetParent(Transform* parent) {
		// Needs more
		if (parent)
			m_parent = parent;
		else
			m_parent = nullptr;
	}
	
	const Transform* Parent() {
		return m_parent;
	}

	void DisplayInEditor(Entity entity);

private:
	friend class SceneManager;
	friend class RenderManager;
	friend class ImguiHandler;

#ifdef EDITOR_DEBUG_VIEW
	Vect3f m_editorRotation;
	Vect3f m_lastEditorRotation;
#endif
	
	Mat4f m_last;
	Mat4f m_world;
	Mat4f m_local; 
	
	Transform* m_parent = nullptr;
};
REGISTER_COMPONENT(Transform);
