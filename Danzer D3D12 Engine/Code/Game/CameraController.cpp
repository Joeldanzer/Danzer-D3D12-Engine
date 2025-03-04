#include "GamePCH.h"

#include "CameraController.h"

//#include "Engine/GameObjects/Object.h"
#include "Engine/Rendering/Camera.h"

CameraController::CameraController(Camera* cam, Vect3f offset) :
	m_camera(cam),
	m_offset(offset),
	m_target(nullptr),
	m_smoothMovement(10.f),
	m_boundingArea(nullptr),
	m_outofBoundsPosition(Vect3f(15.f, 5.f, 0.f ))
{
	//cam->SetPosition(Vect3f(0.f, 0.f ,0.f) + m_offset);

	m_points[0] = {  12.f,   0.f, 0.f }; 
	m_points[1] = { -12.f,   0.f, 0.f }; 
	m_points[2] = {   0.f,  6.25f, 0.f }; 
	m_points[3] = {   0.f, -6.25f, 0.f }; 
}

CameraController::~CameraController()
{
	m_target = nullptr;
	m_boundingArea = nullptr;
}

void CameraController::SetCamera(Camera* cam)
{
	m_camera = cam;
	//Vect3f pos = { m_boundingArea->GetMin().x, m_boundingArea->GetMin().y, 0.f };
	//pos += m_outofBoundsPosition;
	//cam->SetPosition(pos + m_offset);

	//m_camera->SetPosition(m_outofBoundsPosition + m_boundingArea->GetMin());
	//if (m_target) 
	//	m_camera->SetPosition(m_target->GetPosition() + m_offset);
}

void CameraController::SetCameraBoundingBox(AABBCollider* boundigArea)
{
	m_boundingArea = boundigArea;
}

void CameraController::UpdateCamera(const float dt)
{
	//Vect3f desiredPos = m_target->GetPosition() + m_offset;
	//Vect3f newPos = Lerp(m_camera->GetPosition(), desiredPos, m_smoothMovement * dt);
	//
	//for (UINT i = 0; i < 4; i++)
	//{
	//	Vect3f point = { newPos.x, newPos.y, m_target->GetPosition().z };
	//	point += m_points[i];
	//	if (!m_boundingArea->IsPointInside(point)) {
	//		Mat4f last = m_camera->GetLastTransform();
	//		Vect3f pos = { last(3,0), last(3, 1), last(3,2) };
	//
	//		switch (i)
	//		{
	//		case 0:
	//		case 1:
	//			newPos = { pos.x, newPos.y, m_camera->GetPosition().z };
	//			break;
	//		case 2:
	//		case 3:
	//			newPos = { newPos.x, pos.y, m_camera->GetPosition().z };
	//			break;
	//		}	
	//	}
	//}
	//m_camera->SetPosition(newPos);	
}
