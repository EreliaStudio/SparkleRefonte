#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include "structure/spk_iostream.hpp"
#include <dinput.h>
#include <iostream>
#include "structure/thread/spk_persistant_worker.hpp"
#include "structure/system/event/spk_event.hpp"
#include "structure/math/spk_vector2.hpp"
#include "structure/system/device/spk_controller.hpp"
#include <windows.h>

#include "spk_debug_macro.hpp"

namespace spk
{
	class ControllerInputThread
	{
	private:
		PersistantWorker _worker;
		HWND _hWnd = NULL;
		bool _initialization = false;
		IDirectInput8* _directInput = nullptr;
		IDirectInputDevice8* _controller = nullptr;
		DIJOYSTATE _controllerState = DIJOYSTATE();
		DIJOYSTATE _prevControllerState = DIJOYSTATE();

		bool InitializeDirectInput()
		{
			HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL);
			if (FAILED(hr))
			{
				return false;
			}

			hr = _directInput->CreateDevice(GUID_Joystick, &_controller, NULL);
			if (FAILED(hr))
			{
				return false;
			}

			hr = _controller->SetDataFormat(&c_dfDIJoystick);
			if (FAILED(hr))
			{
				return false;
			}

			hr = _controller->SetCooperativeLevel(_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
			if (FAILED(hr))
			{
				return false;
			}

			return true;
		}

		void PollController()
		{
			if (!_controller)
			{
				return;
			}

			HRESULT hr = _controller->Poll();
			if (FAILED(hr))
			{
				hr = _controller->Acquire();
				while (hr == DIERR_INPUTLOST)
				{
					hr = _controller->Acquire();
				}
				return;
			}

			hr = _controller->GetDeviceState(sizeof(DIJOYSTATE), &_controllerState);
			if (FAILED(hr))
			{
				return;
			}

			// Process buttons
			for (int i = 0; i < 32; ++i)
			{
				if ((_controllerState.rgbButtons[i] & 0x80) && !(_prevControllerState.rgbButtons[i] & 0x80))
				{
					spk::cout << "Pressing button ID [" << i << "]" << std::endl;
					PostButtonPress(_hWnd, static_cast<spk::Controller::Button>(i));
					_prevControllerState.rgbButtons[i] = !_controllerState.rgbButtons[i];
				}
				else if (!(_controllerState.rgbButtons[i] & 0x80) && (_prevControllerState.rgbButtons[i] & 0x80))
				{
					spk::cout << "Releasing button ID [" << i << "]" << std::endl;
					PostButtonRelease(_hWnd, static_cast<spk::Controller::Button>(i));
					_prevControllerState.rgbButtons[i] = !_controllerState.rgbButtons[i];
				}
			}

			if (_controllerState.lX != _prevControllerState.lX || _controllerState.lY != _prevControllerState.lY)
			{
				spk::cout << "Moving joystick Left by [" << _controllerState.lX << " / " << _controllerState.lY << "]" << std::endl;
				PostLeftJoystickMove(_hWnd, _controllerState.lX, _controllerState.lY);
				_prevControllerState.lX = _controllerState.lX;
				_prevControllerState.lY = _controllerState.lY;
			}

			if (_controllerState.lRx != _prevControllerState.lRx || _controllerState.lRy != _prevControllerState.lRy)
			{
				spk::cout << "Moving joystick Right by [" << _controllerState.lRx << " / " << _controllerState.lRy << "]" << std::endl;
				PostRightJoystickMove(_hWnd, _controllerState.lRx, _controllerState.lRy);
				_prevControllerState.lRx = _controllerState.lRx;
				_prevControllerState.lRy = _controllerState.lRy;
			}
		}

		static void PostButtonPress(HWND hWnd, spk::Controller::Button button)
		{
			PostMessage(hWnd, WM_CONTROLLER_BUTTON_PRESS, static_cast<int>(button), 0);
		}

		static void PostButtonRelease(HWND hWnd, spk::Controller::Button button)
		{
			PostMessage(hWnd, WM_CONTROLLER_BUTTON_RELEASE, static_cast<int>(button), 0);
		}

		static void PostLeftJoystickMove(HWND hWnd, const int& p_x, const int& p_y)
		{
			PostMessage(hWnd, WM_LEFT_JOYSTICK_MOVE, p_x, p_y);
		}

		static void PostRightJoystickMove(HWND hWnd, const int& p_x, const int& p_y)
		{
			PostMessage(hWnd, WM_RIGHT_JOYSTICK_MOVE, p_x, p_y);
		}

	public:
		ControllerInputThread() :
			_worker(L"ControllerInputThread"),
			_directInput(nullptr),
			_controller(nullptr)
		{

		}

		~ControllerInputThread()
		{
			if (_controller)
			{
				_controller->Unacquire();
				_controller->Release();
			}
			if (_directInput)
			{
				_directInput->Release();
			}
		}

		void bind(HWND hWnd)
		{
			_hWnd = hWnd;
		}

		void start()
		{
			_worker.addPreparationStep([this](){
				InitializeDirectInput();
			}).relinquish();
			_worker.addExecutionStep([this](){
					PollController();
				}).relinquish();
			
			_worker.start();
		}

		void stop()
		{
			_worker.stop();
		}
	};
}
