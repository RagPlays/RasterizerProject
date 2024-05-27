
#ifndef CAMERA_H
#define CAMERA_H

#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle, float _aspectRatio) :
			origin{ _origin },
			fovAngle{ _fovAngle },
			far{ 100.f },
			near{ 0.1f },
			aspectRatio{ _aspectRatio }
		{
			assert(far > near);
		}

		float aspectRatio{};

		Vector3 origin{};
		float fovAngle{ 90.f };
		float fovValue{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};

		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix projectionMatrix{};

		float far{};
		float near{};

		void Initialize(const float _fovAngle = 90.f, const Vector3& _origin = { 0.f, 0.f, 0.f }, float _aspectRatio = 1.f, float _far = 1000.f, float _near = 1.f)
		{
			fovAngle = _fovAngle;
			fovValue = tanf((fovAngle * TO_RADIANS) / 2.f);
			origin = _origin;
			aspectRatio = _aspectRatio;

			far = _far;
			near = _near;

			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}

		void CalculateViewMatrix()
		{
			invViewMatrix = Matrix::CreateRotation(totalPitch * TO_RADIANS, totalYaw * TO_RADIANS, 0.f) * Matrix::CreateTranslation(origin);
			viewMatrix = invViewMatrix.Inverse();  // WorldToCamera Matrix

			forward = viewMatrix.TransformVector(-Vector3::UnitZ).Normalized();
			forward.z *= -1.f;

			right = { Vector3::Cross(Vector3::UnitY, forward).Normalized() };
			up = { Vector3::Cross(forward, right).Normalized() };

			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
		}

		void CalculateProjectionMatrix()
		{
			projectionMatrix = Matrix::CreatePerspectiveFovLH(fovValue, aspectRatio, near, far);

			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			// Camera Logics //
			constexpr int fovMin{ 0 };
			constexpr int fovMax{ 180 };
			constexpr float movementSpeed{ 30.f };
			constexpr float sensitivity{ 0.2f };

			// DeltaTime //
			const float deltaTimeSpeed{ pTimer->GetElapsed() * movementSpeed };

			// Mouse Input //
			int mouseX;
			int mouseY;
			const uint32_t mouseState{ SDL_GetRelativeMouseState(&mouseX, &mouseY) };

			if (mouseState)
			{
				// MOUSE
				if (mouseState & SDL_BUTTON_RMASK && mouseState & SDL_BUTTON_LMASK)
				{
					origin += -mouseY * sensitivity * Vector3::UnitY;
				}
				else if (mouseState & SDL_BUTTON_RMASK)
				{
					totalPitch += mouseY * sensitivity;
					totalYaw += mouseX * sensitivity;
				}
				else if (mouseState & SDL_BUTTON_LMASK)
				{
					origin += -mouseY * sensitivity * Vector3::UnitZ;
					totalYaw += mouseX * sensitivity;
				}
			}

			// Keyboard Input //
			int nrOfKeys;
			bool isKeyPressed{};
			const uint8_t* pKeyboardState{ SDL_GetKeyboardState(&nrOfKeys) };
			for (int idx = 0; idx < nrOfKeys; idx++) 
			{
				if (pKeyboardState[idx])
				{
					isKeyPressed = true;
					break;
				}
			}

			if (isKeyPressed)
			{
				// WASD
				if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])	// forward
				{
					origin += deltaTimeSpeed * forward;
				}
				if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN]) // backwards
				{
					origin -= deltaTimeSpeed * forward;
				}
				if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT]) // left
				{
					origin -= deltaTimeSpeed * right;
				}
				if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT]) // right
				{
					origin += deltaTimeSpeed * right;
				}

				// Up and Down
				if (pKeyboardState[SDL_SCANCODE_SPACE] || pKeyboardState[SDL_SCANCODE_E]) // up
				{
					origin += deltaTimeSpeed * Vector3::UnitY;
				}
				if (pKeyboardState[SDL_SCANCODE_LSHIFT] || pKeyboardState[SDL_SCANCODE_Q]) // down
				{
					origin += deltaTimeSpeed * -Vector3::UnitY;
				}

				//Change POV
				if (pKeyboardState[SDL_SCANCODE_T])	// pov down
				{
					fovAngle -= 1;

					if (fovAngle < fovMin) fovAngle = fovMin;

					fovValue = tanf((fovAngle * TO_RADIANS) * 0.5f);
				}
				else if (pKeyboardState[SDL_SCANCODE_G])	// pov up
				{
					fovAngle += 1;

					if (fovAngle > fovMax) fovAngle = fovMax;

					fovValue = tanf((fovAngle * TO_RADIANS) * 0.5f);
				}
				else if (pKeyboardState[SDL_SCANCODE_R])	// reset pov
				{
					fovAngle = 45;
					fovValue = tanf((fovAngle * TO_RADIANS) * 0.5f);
				}
			}

			if (isKeyPressed || mouseState)
			{
				//Update Matrices
				CalculateViewMatrix();
				CalculateProjectionMatrix();
			}
		}
	};
}

#endif // !CAMERA_H