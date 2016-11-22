#pragma once
#include "Engine/Math/Vector2.hpp"



//-----------------------------------------------------------------------------------------------
class GameObject
{
	static const float kSightRange;
public:
	GameObject();
	GameObject(const Vector2& position, float startOrientation, float radius, int health);
	bool CanSeePlayer() const;
	/*pure*/ virtual void Update(float deltaSeconds) = 0;
	/*pure*/ virtual void Render() const = 0;
	Vector2 GetPosition() const { return m_position; };
	float GetOrientation() const { return m_orientationDegrees; };
	float GetRadius() const { return m_radius; }
	void SetPosition(const Vector2& destination) { m_position = destination; }
	void TakeDamage() { m_health--; }
	bool IsDead() const { return m_health <= 0; }

protected:
	float			m_radius;
	Vector2			m_position;
	float			m_orientationDegrees;
	int				m_health;
};