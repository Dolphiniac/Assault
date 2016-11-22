#include "Game/Turret.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cstdlib>

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const float Turret::kTurretRadius = .4f;
STATIC const float Turret::kMinimumTimeBetweenFiringSeconds = 1.f;
STATIC const float Turret::kMinimumTimeBetweenChangingOrientationSeconds = 3.f;
STATIC const float Turret::kSpinSpeed = 90.f; //90 degrees per second
STATIC const int Turret::kTurretHealth = 1;
STATIC const float Turret::kMinimumValueForFireRange = cos(5.f * DEG2RAD);


//-----------------------------------------------------------------------------------------------
Turret::Turret(const Vector2& position)
: m_texture(Texture::CreateOrGetTexture("Data/Images/Turret2.png"))
, m_timeSinceLastFire(0.f)
, m_timeSinceLastOrientationChange(0.f)
, m_goalOrientation(0.f)
, GameObject(position, 0.f, kTurretRadius, kTurretHealth)
{
	Vector2 mins(-m_radius, -m_radius);
	Vector2 maxs(m_radius, m_radius);

	m_aabb = new AABB2(mins, maxs);
}


//-----------------------------------------------------------------------------------------------
void Turret::Update(float deltaSeconds)
{
	m_timeSinceLastFire += deltaSeconds;
	m_timeSinceLastOrientationChange += deltaSeconds;

	float distance = GetShortestRotationPath(m_orientationDegrees, m_goalOrientation);

	Vector2 normalizedOrientationVector(cos(m_orientationDegrees * DEG2RAD), sin(m_orientationDegrees * DEG2RAD));

	Vector2 normalizedGoalOrientationVector(cos(m_goalOrientation * DEG2RAD), sin(m_goalOrientation * DEG2RAD));

	if (Vector2::Dot(normalizedOrientationVector, normalizedGoalOrientationVector) >= .95f)
	{
		m_orientationDegrees = m_goalOrientation;
		distance = 0.f;
	}

	if (distance > 0.f)
	{
		m_orientationDegrees -= kSpinSpeed * deltaSeconds;
	}
	else if (distance < 0.f)
	{
		m_orientationDegrees += kSpinSpeed * deltaSeconds;
	}

	if (GameObject::CanSeePlayer())
	{
		Vector2 displacement(m_position - g_theMap->GetPlayerPosition());
		m_goalOrientation = atan2f(displacement.y, displacement.x) * RAD2DEG;

		float vectorSameness = GetValueForSamenessOfVectors();
		if (vectorSameness >= kMinimumValueForFireRange && m_timeSinceLastFire > kMinimumTimeBetweenFiringSeconds)
		{
			Vector2 offsetFromCenter(m_radius * cos(m_orientationDegrees * DEG2RAD), m_radius * sin(m_orientationDegrees * DEG2RAD));
			g_theMap->SpawnBullet(m_position + offsetFromCenter, m_orientationDegrees, false);
			m_timeSinceLastFire = 0.f;
		}
	}
	else
	{
		if (m_timeSinceLastOrientationChange > kMinimumTimeBetweenChangingOrientationSeconds)
		{
			m_goalOrientation = (float)(rand() % 360);
			m_timeSinceLastOrientationChange = 0.f;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Turret::Render() const
{
	The.Renderer->PushView();
	The.Renderer->TranslateView(m_position);
	The.Renderer->RotateView(m_orientationDegrees);
	The.Renderer->DrawTexturedAABB(*m_aabb, m_texture);
	The.Renderer->PopView();
}


//-----------------------------------------------------------------------------------------------
Turret::~Turret()
{
//	delete m_aabb;
}

float Turret::GetValueForSamenessOfVectors() const
{
	Vector2 forward(cos(m_orientationDegrees * DEG2RAD), sin(m_orientationDegrees * DEG2RAD));
	Vector2 toPlayer(g_theMap->GetPlayerPosition() - m_position);
	toPlayer.Normalize();

	return Vector2::Dot(forward, toPlayer);
}

