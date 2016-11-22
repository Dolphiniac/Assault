#pragma once

#include "Game/Tank.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteSheet;


//-----------------------------------------------------------------------------------------------
class EnemyTank : public Tank
{
	static const int kEnemyTankHealth;
	static const float kSpinSpeed;
	static const float kMovementSpeed;
	static const float kSightRange;
	static const float kMinimumTimeBetweenFiringSeconds;
	static const float kMinimumTimeBetweenChangingOrientationSeconds;
	static const float kMinimumValueForDrivingToPlayer;
	static const float kMinimumValueForFiring;
public:
	EnemyTank(const Vector2& position);
	void Update(float deltaSeconds);
	void Render() const;
	float GetValueForSamenessOfVectors() const;
	~EnemyTank();
private:
	float			m_timeSinceLastFire;
	float			m_timeSinceLastOrientationChange;
	float			m_goalOrientation;
	SpriteSheet*	m_spriteSheet;
};