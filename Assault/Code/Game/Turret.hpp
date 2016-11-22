#pragma once

#include "Game/GameObject.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;
class AABB2;


//-----------------------------------------------------------------------------------------------
class Turret : public GameObject
{
	static const float kTurretRadius;
	static const float kMinimumTimeBetweenFiringSeconds;
	static const float kMinimumTimeBetweenChangingOrientationSeconds;
	static const float kSpinSpeed;
	static const int kTurretHealth;
	static const float kMinimumValueForFireRange;

public:
	Turret(const Vector2& position);
	void Update(float deltaSeconds);
	void Render() const;
	~Turret();
	bool CanSeePlayer() const;
	float GetValueForSamenessOfVectors() const;
private:
	Texture*		m_texture;
	AABB2*			m_aabb;
	float			m_timeSinceLastFire;
	float			m_timeSinceLastOrientationChange;
	float			m_goalOrientation;
};