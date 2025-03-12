#include "stdafx.h"
#include "SphereCollider.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void SphereCollider::DisplayInEditor(const Entity entity)
{
}
#else
void SphereCollider::DisplayInEditor(const Entity entity){}
#endif
