#pragma once

#include <bitset>

#include "Windows.h"
#include "Windowsx.h"
#include <Xinput.h>
#include <array>
#include <queue>
#include <tuple>

using std::bitset;

class Input
{
public:

	static Input& GetInstance();
	enum class MouseButton
	{
		Left = 0,
		Right = 1,
		Middle = 2,
		Mouse4 = 3,
		Mouse5 = 4,
	};

	enum class XButton
	{
		DPAD_UP = 0x0001,
		DPAD_DOWN = 0x0002,
		DPAD_LEFT = 0x0004,
		DPAD_RIGHT = 0x0008,
		START = 0x0010,
		BACK = 0x0020,
		LEFT_THUMB = 0x0040,
		RIGHT_THUMB = 0x0080,
		LEFT_SHOULDER = 0x0100,
		RIGHT_SHOULDER = 0x0200,
		A = 0x1000,
		B = 0x2000,
		X = 0x4000,
		Y = 0x8000
	};

	enum class GamePadIndex { one = 0, two, three, four };

	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void Update();

	bool IsKeyPressed(WPARAM wParam) const;
	bool IsKeyDown(WPARAM wParam) const;
	bool IsKeyReleased(WPARAM wParam) const;

	float ScaledMouseX() const;
	float ScaledMouseY() const;
	float ScaledMouseDeltaX() const;
	float ScaledMouseDeltaY() const;
	int MouseX() const;
	int MouseY() const;
	int MouseDeltaX() const;
	int MouseDeltaY() const;
	int MouseScreenX() const;
	int MouseScreenY() const;
	int MouseWheel() const;
	void SetMouseScreenPosition(int x, int y);
	bool IsMousePressed(MouseButton mouse_button) const;
	bool IsMouseDown(MouseButton mouse_button) const;
	bool IsMouseReleased(MouseButton mouse_button) const;

	XINPUT_GAMEPAD* XStateControll(const GamePadIndex index);
	bool XChecConnectionControll(const GamePadIndex index);
	bool XRefreshControll(const GamePadIndex index);
	bool XIsConnected(const GamePadIndex index) const;
	bool XIsPressed(const GamePadIndex index, const XButton button) const;
	bool XIsDown(const GamePadIndex index, const XButton button) const;
	bool XIsReleased(const GamePadIndex index, const XButton button) const;
	bool XIsLeftTriggerPressed(const GamePadIndex controll) const;
	float XLeftTrigger(const GamePadIndex index) const;
	float XRightTrigger(const GamePadIndex index) const;
	float XLeftStickX(const GamePadIndex index) const;
	float XLeftStickY(const GamePadIndex index) const;
	float XRightStickX(const GamePadIndex index) const;
	float XRightStickY(const GamePadIndex index) const;

	void SetWindowScale(float scale_x, float scale_y);
	void SetWindow(HWND* window_handle);

private:
	static Input ourInput;

	Input() = default;
	~Input() = default;

	struct Controlls
	{
		int _x_button_last = 0x00000000;
		int _x_controller_id = -1;
		float _x_deadzone = 0.05f;
		float _x_left_stick_x = 0.0f;
		float _x_left_stick_y = 0.0f;
		float _x_right_stick_x = 0.0f;
		float _x_right_stick_y = 0.0f;
		float _x_left_trigger = 0.0f;
		float _x_right_trigger = 0.0f;
		int _x_check_interval = 0;
	};

	HWND* _window_handle = nullptr;

	const int X_CHECK_INTERVAL = 60;
	const float X_TRIGGER_THRESHOLD = 0.9f;

	std::array<XINPUT_STATE, XUSER_MAX_COUNT> _controllers_XINPUT = { 0 };
	std::array<Controlls, XUSER_MAX_COUNT> _controllers_controlls = { 0 };

	float _window_scale_x = 640.0f;
	float _window_scale_y = 360.0f;
	int _mouse_wheel = 0;
	int _mouse_x = 0;
	int _mouse_y = 0;
	int _mouse_x_last = 0;
	int _mouse_y_last = 0;
	int _mouse_screen_x = 0;
	int _mouse_screen_y = 0;
	std::bitset<5> _mouse_down_last;
	std::bitset<5> _mouse_down;
	std::bitset<256> _key_down_last;
	std::bitset<256> _key_down;

};