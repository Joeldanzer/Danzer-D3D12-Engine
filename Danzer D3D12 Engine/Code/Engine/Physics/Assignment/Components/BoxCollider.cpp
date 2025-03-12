#include "stdafx.h"
#include "BoxCollider.h"

#ifdef EDITOR_DEBUG_VIEW
#include "imgui/imgui.h"
void BoxCollider::DisplayInEditor(const Entity entity)
{

}
#else
void BoxCollider::DisplayInEditor(const Entity entity){}
#endif
