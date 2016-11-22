#include "Game/GameObject.hpp"
#include "Game/Map.hpp"

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const float GameObject::kSightRange = 20.f;


//-----------------------------------------------------------------------------------------------
GameObject::GameObject()
: m_position(0.f, 0.f)
, m_orientationDegrees(0.f)
, m_radius(0.f)
, m_health(0)
{
}


//-----------------------------------------------------------------------------------------------
GameObject::GameObject(const Vector2& position, float startOrientation, float radius, int health)
: m_position(position)
, m_orientationDegrees(startOrientation)
, m_radius(radius)
, m_health(health)
{
}

//-----------------------------------------------------------------------------------------------
bool GameObject::CanSeePlayer() const
{
	Vector2 playerPos = g_theMap->GetPlayerPosition();
	if (playerPos == Vector2::Zero)
	{
		return false;
	}

	Vector2 displacementFromPlayer(playerPos - m_position);
	float sightRangeSquared = kSightRange * kSightRange;

	if (displacementFromPlayer.LengthSquared() > sightRangeSquared)
	{
		return false;
	}

	if (g_theMap->Raycast2D(m_position, playerPos).didImpact)
	{
		return false;
	}

	return true;
}
