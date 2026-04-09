#pragma once
#include <DirectXMath.h>
#include <algorithm>

class Camera {
public:
	Camera(int width, int height);
	virtual ~Camera() = default;
	void Rotate(float pitch, float yaw);
	void UpdateAspectRatio(int width, int height);
	void UpdateFov(float fov);
	inline DirectX::XMMATRIX GetProjection() const { return  m_Projection; }
	inline DirectX::XMMATRIX GetView() const { return m_View; }
private:
	DirectX::XMMATRIX m_Projection; 
	DirectX::XMMATRIX m_View;
	float m_PitchRadians = 0.0f;
	float m_YawRadians = 0.0f;
	float m_FieldOfViewDegrees = 50.0f;
	float m_AspectRatio = 0.0f;
	void CalculateProjection();
};