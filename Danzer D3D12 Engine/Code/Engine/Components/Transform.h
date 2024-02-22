#pragma once
#include "Core/MathDefinitions.h"

class Transform {
public:
	Transform() : 
	m_position(0.f, 0.f, 0.f),
	m_scale(1.f, 1.f, 1.f),
	m_rotation(0.f ,0.f ,0.f, 1.f) {}

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

	const Vect3f& LastPosition() {
		return m_lastPosition;
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

private:
	friend class Scene;
	friend class RenderManager;

	Vect3f m_lastPosition;

	Mat4f m_last;
	Mat4f m_world;
	Mat4f m_local; 
	
	Transform* m_parent = nullptr;
};

