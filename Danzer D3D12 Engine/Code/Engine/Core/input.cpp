#include "stdafx.h"
#include "input.hpp"
#include <cassert>
#pragma comment(lib, "Xinput9_1_0.lib")


using std::bitset;

// Keeping this here makes sure it's actually deallocated
Input Input::ourInput;

/**
* Get the one and only instance of Input.
**/
Input& Input::GetInstance()
{
	return ourInput;
}

bool Input::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		_key_down[wParam] = true;
		return true;
	} break;
	case WM_KEYUP:
	{
		_key_down[wParam] = false;
		return true;
	} break;
	case WM_MOUSEMOVE:
	{
		_mouse_x = GET_X_LPARAM(lParam);
		_mouse_y = GET_Y_LPARAM(lParam);
		return true;
	} break;
	case WM_MOUSEWHEEL:
	{
		_mouse_wheel += GET_WHEEL_DELTA_WPARAM(wParam);
		return true;
	} break;
	case WM_LBUTTONDOWN:
	{
		_mouse_down[static_cast<int>(MouseButton::Left)] = true;
		return true;
	} break;
	case WM_LBUTTONUP:
	{
		_mouse_down[static_cast<int>(MouseButton::Left)] = false;
		return true;
	} break;
	case WM_RBUTTONDOWN:
	{
		_mouse_down[static_cast<int>(MouseButton::Right)] = true;
		return true;
	} break;
	case WM_RBUTTONUP:
	{
		_mouse_down[static_cast<int>(MouseButton::Right)] = false;
		return true;
	} break;
	case WM_MBUTTONDOWN:
	{
		_mouse_down[static_cast<int>(MouseButton::Middle)] = true;
		return true;
	} break;
	case WM_MBUTTONUP:
	{
		_mouse_down[static_cast<int>(MouseButton::Middle)] = false;
		return true;
	} break;
	case WM_XBUTTONDOWN:
	{
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			_mouse_down[static_cast<int>(MouseButton::Mouse4)] = true;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
		{
			_mouse_down[static_cast<int>(MouseButton::Mouse5)] = true;
		}
		return true;
	} break;
	case WM_XBUTTONUP:
	{
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
		{
			_mouse_down[static_cast<int>(MouseButton::Mouse4)] = false;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
		{
			_mouse_down[static_cast<int>(MouseButton::Mouse5)] = false;
		}
		return true;
	} break;
	default:
	{
		return false;
	} break;
	}
}

void Input::Update()
{

	_key_down_last = _key_down;
	_mouse_down_last = _mouse_down;
	_mouse_wheel = 0;
	_mouse_x_last = _mouse_x;
	_mouse_y_last = _mouse_y;

	POINT point;

	if (GetCursorPos(&point))
	{
		_mouse_screen_x = point.x;
		_mouse_screen_y = point.y;
	}

	_controllers_controlls[static_cast<int>(GamePadIndex::one)]._x_button_last = _controllers_XINPUT[static_cast<int>(GamePadIndex::one)].Gamepad.wButtons;
	_controllers_controlls[static_cast<int>(GamePadIndex::two)]._x_button_last = _controllers_XINPUT[static_cast<int>(GamePadIndex::two)].Gamepad.wButtons;
	//_controllers_controlls[static_cast<int>(GamePadIndex::three)]._x_button_last = _controllers_XINPUT[static_cast<int>(GamePadIndex::two)].Gamepad.wButtons;
	//_controllers_controlls[static_cast<int>(GamePadIndex::four)]._x_button_last = _controllers_XINPUT[static_cast<int>(GamePadIndex::two)].Gamepad.wButtons;

	XRefreshControll(GamePadIndex::one);
	XRefreshControll(GamePadIndex::two);
	//x_refresh_controll(GamePadIndex::three);
	//x_refresh_controll(GamePadIndex::four);
}

#pragma region Key Board & Mouse 

bool Input::IsKeyPressed(WPARAM wParam) const
{
	return _key_down[wParam] && !_key_down_last[wParam];
}
bool Input::IsKeyDown(WPARAM wParam) const
{
	return _key_down[wParam];
}
bool Input::IsKeyReleased(WPARAM wParam) const
{
	return (!_key_down[wParam]) && _key_down_last[wParam];
}

float Input::ScaledMouseX() const
{
	RECT rect;
	if (GetWindowRect(*_window_handle, &rect))
	{
		return static_cast<float>(_mouse_x) * _window_scale_x / static_cast<float>(rect.right - rect.left - 16);
	}
	else
	{
		return static_cast<float>(_mouse_x);
	}
}
float Input::ScaledMouseY() const
{
	RECT rect;
	if (GetWindowRect(*_window_handle, &rect))
	{
		return static_cast<float>(_mouse_y) * _window_scale_y / static_cast<float>(rect.bottom - rect.top - 39);
	}
	else
	{
		return static_cast<float>(_mouse_y);
	}
}
float Input::ScaledMouseDeltaX() const
{
	RECT rect;
	if (GetWindowRect(*_window_handle, &rect))
	{
		return static_cast<float>(_mouse_x - _mouse_x_last) * _window_scale_x / static_cast<float>(rect.right - rect.left - 16);
	}
	else
	{
		return static_cast<float>(_mouse_x - _mouse_x_last);
	}
}
float Input::ScaledMouseDeltaY() const
{
	RECT rect;
	if (GetWindowRect(*_window_handle, &rect))
	{
		return static_cast<float>(_mouse_y - _mouse_y_last) * _window_scale_y / static_cast<float>(rect.bottom - rect.top - 39);
	}
	else
	{
		return static_cast<float>(_mouse_y - _mouse_y_last);
	}
}
int Input::MouseX() const
{
	return _mouse_x;
}
int Input::MouseY() const
{
	return _mouse_y;
}
int Input::MouseDeltaX() const
{
	return _mouse_x - _mouse_x_last;
}
int Input::MouseDeltaY() const
{
	return _mouse_y - _mouse_y_last;
}
int Input::MouseScreenX() const
{
	return _mouse_screen_x;
}
int Input::MouseScreenY() const
{
	return _mouse_screen_y;
}
int Input::MouseWheel() const
{
	return _mouse_wheel;
}
void Input::SetMouseScreenPosition(int x, int y)
{
	SetCursorPos(x, y);
}

bool Input::IsMousePressed(MouseButton mouse_button) const
{
	return _mouse_down[static_cast<int>(mouse_button)] && !_mouse_down_last[static_cast<int>(mouse_button)];
}
bool Input::IsMouseDown(MouseButton mouse_button) const
{
	return _mouse_down[static_cast<int>(mouse_button)];
}
bool Input::IsMouseReleased(MouseButton mouse_button) const
{
	return (!_mouse_down[static_cast<int>(mouse_button)]) && _mouse_down_last[static_cast<int>(mouse_button)];
}

#pragma endregion

#pragma region XInput GamePad

XINPUT_GAMEPAD* Input::XStateControll(const GamePadIndex index)
{
	return &_controllers_XINPUT[static_cast<int>(index)].Gamepad;
}

bool Input::XChecConnectionControll(const GamePadIndex index)
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);

	int controller_id = -1;

	for (DWORD i = 0; i < XUSER_MAX_COUNT && controller_id == -1; ++i)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			controller_id = i;
		}
	}

	_controllers_controlls[static_cast<int>(index)]._x_controller_id = controller_id;

	return _controllers_controlls[static_cast<int>(index)]._x_controller_id == static_cast<int>(index);
}

bool Input::XRefreshControll(const GamePadIndex index)
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);

	++_controllers_controlls[static_cast<int>(index)]._x_check_interval;

	_controllers_XINPUT[static_cast<int>(index)].Gamepad.wButtons;

	if (_controllers_controlls[static_cast<int>(index)]._x_controller_id == -1 && _controllers_controlls[static_cast<int>(index)]._x_check_interval >= X_CHECK_INTERVAL)
	{
		XChecConnectionControll(index);
		_controllers_controlls[static_cast<int>(index)]._x_check_interval = 0;
	}

	if (_controllers_controlls[static_cast<int>(index)]._x_controller_id != -1)
	{

		ZeroMemory(&_controllers_XINPUT[static_cast<int>(index)], sizeof(XINPUT_STATE));
		if (XInputGetState(_controllers_controlls[static_cast<int>(index)]._x_controller_id, &_controllers_XINPUT[static_cast<int>(index)]) != ERROR_SUCCESS)
		{
			_controllers_controlls[static_cast<int>(index)]._x_controller_id = -1;
			return false;
		}

		float left_x = fmaxf(-1, (float)_controllers_XINPUT[static_cast<int>(index)].Gamepad.sThumbLX / 32767);
		float left_y = fmaxf(-1, (float)_controllers_XINPUT[static_cast<int>(index)].Gamepad.sThumbLY / 32767);

		float left_distance = sqrtf((left_x * left_x) + (left_y * left_y));

		if (left_distance < _controllers_controlls[static_cast<int>(index)]._x_deadzone)
		{
			left_distance = 0.0f;
		}
		else
		{
			left_distance -= _controllers_controlls[static_cast<int>(index)]._x_deadzone;
			left_distance /= (1.0f - _controllers_controlls[static_cast<int>(index)]._x_deadzone);
		}

		if (left_distance > 1.0f)
		{
			left_x /= left_distance;
			left_y /= left_distance;
		}

		_controllers_controlls[static_cast<int>(index)]._x_left_stick_x = left_x;
		_controllers_controlls[static_cast<int>(index)]._x_left_stick_y = left_y;

		float right_x = fmaxf(-1, (float)_controllers_XINPUT[static_cast<int>(index)].Gamepad.sThumbRX / 32767);
		float right_y = fmaxf(-1, (float)_controllers_XINPUT[static_cast<int>(index)].Gamepad.sThumbRY / 32767);
		float right_distance = sqrtf((right_x * right_x) + (right_y * right_y));

		if (right_distance < _controllers_controlls[static_cast<int>(index)]._x_deadzone)
		{
			right_distance = 0.0f;
		}
		else
		{
			right_distance -= _controllers_controlls[static_cast<int>(index)]._x_deadzone;
			right_distance /= (1.0f - _controllers_controlls[static_cast<int>(index)]._x_deadzone);
		}

		if (right_distance > 1.0f)
		{
			right_x /= right_distance;
			right_y /= right_distance;
		}

		_controllers_controlls[static_cast<int>(index)]._x_right_stick_x = right_x;
		_controllers_controlls[static_cast<int>(index)]._x_right_stick_y = right_y;

		_controllers_controlls[static_cast<int>(index)]._x_left_trigger = (float)_controllers_XINPUT[static_cast<int>(index)].Gamepad.bLeftTrigger / 255;
		_controllers_controlls[static_cast<int>(index)]._x_right_trigger = (float)_controllers_XINPUT[static_cast<int>(index)].Gamepad.bRightTrigger / 255;
	}
	return true;
}

bool Input::XIsConnected(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);

	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));

	if (XInputGetState(static_cast<int>(index), &state) != ERROR_SUCCESS)
	{
		return false;
	}
	return true;
}

bool Input::XIsPressed(const GamePadIndex index, const XButton button) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return (_controllers_XINPUT[static_cast<int>(index)].Gamepad.wButtons & static_cast<WORD>(button)) != 0 && (_controllers_controlls[static_cast<int>(index)]._x_button_last & static_cast<WORD>(button)) == 0;
}

bool Input::XIsDown(const GamePadIndex index, const XButton button) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return (_controllers_XINPUT[static_cast<int>(index)].Gamepad.wButtons & static_cast<WORD>(button)) != 0;
}

bool Input::XIsReleased(const GamePadIndex index, const XButton button) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return  (_controllers_XINPUT[static_cast<int>(index)].Gamepad.wButtons & static_cast<WORD>(button)) == 0 && (_controllers_controlls[static_cast<int>(index)]._x_button_last & static_cast<WORD>(button)) != 0;
}

bool Input::XIsLeftTriggerPressed(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return _controllers_controlls[static_cast<int>(index)]._x_left_trigger > X_TRIGGER_THRESHOLD;
}

float Input::XLeftTrigger(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return _controllers_controlls[static_cast<int>(index)]._x_left_trigger;
}

float Input::XRightTrigger(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return  _controllers_controlls[static_cast<int>(index)]._x_right_trigger;
}

float Input::XLeftStickX(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return _controllers_controlls[static_cast<int>(index)]._x_left_stick_x;
}

float Input::XLeftStickY(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return   _controllers_controlls[static_cast<int>(index)]._x_left_stick_y;
}

float Input::XRightStickX(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return   _controllers_controlls[static_cast<int>(index)]._x_right_stick_x;
}

float Input::XRightStickY(const GamePadIndex index) const
{
	assert(static_cast<int>(index) < XUSER_MAX_COUNT);
	return   _controllers_controlls[static_cast<int>(index)]._x_right_stick_y;
}

#pragma endregion

void Input::SetWindowScale(float scale_x, float scale_y)
{
	_window_scale_x = scale_x;
	_window_scale_y = scale_y;
}
void Input::SetWindow(HWND* window_handle)
{
	_window_handle = window_handle;
}
