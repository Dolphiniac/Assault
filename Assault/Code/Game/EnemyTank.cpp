#include "Game/EnemyTank.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Map.hpp"

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const int EnemyTank::kEnemyTankHealth = 3;
STATIC const float EnemyTank::kSpinSpeed = 90.f;
STATIC const float EnemyTank::kMovementSpeed = .5f;
STATIC const float EnemyTank::kSightRange = 20.f;
STATIC const float EnemyTank::kMinimumTimeBetweenFiringSeconds = .5f;
STATIC const float EnemyTank::kMinimumTimeBetweenChangingOrientationSeconds = 3.f;
STATIC const float EnemyTank::kMinimumValueForDrivingToPlayer = cos(45.f * DEG2RAD);
STATIC const float EnemyTank::kMinimumValueForFiring = cos(5.f * DEG2RAD);


//-----------------------------------------------------------------------------------------------
EnemyTank::EnemyTank(const Vector2& position)
: Tank(position, 0.f, kEnemyTankHealth)
, m_timeSinceLastFire(0.f)
, m_timeSinceLastOrientationChange(0.f)
, m_goalOrientation(0.f)
, m_spriteSheet(new SpriteSheet("Data/Images/MulticolorTanks.png", 8, 8))
{
}


//-----------------------------------------------------------------------------------------------
void EnemyTank::Update(float deltaSeconds)
{
	Vector2 movementDirection(cos(m_orientationDegrees * DEG2RAD), sin(m_orientationDegrees * DEG2RAD));

	m_timeSinceLastFire += deltaSeconds;
	m_timeSinceLastOrientationChange += deltaSeconds;

	float distance = GetShortestRotationPath(m_orientationDegrees, m_goalOrientation);

	Vector2 normalizedGoalOrientationVector(cos(m_goalOrientation * DEG2RAD), sin(m_goalOrientation * DEG2RAD));

	if (Vector2::Dot(movementDirection, normalizedGoalOrientationVector) >= .95f)
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
		float vectorSameness = GetValueForSamenessOfVectors();
		Vector2 displacement(m_position - g_theMap->GetPlayerPosition());
		m_goalOrientation = atan2f(displacement.y, displacement.x) * RAD2DEG;
		if (vectorSameness >= kMinimumValueForDrivingToPlayer)
		{
			m_position += movementDirection * kMovementSpeed * deltaSeconds;
		}

		if (vectorSameness >= kMinimumValueForFiring && m_timeSinceLastFire > kMinimumTimeBetweenFiringSeconds)
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
		m_position += movementDirection * kMovementSpeed * deltaSeconds;
	}
}


//-----------------------------------------------------------------------------------------------
void EnemyTank::Render() const
{
	The.Renderer->PushView();
	The.Renderer->TranslateView(m_position);
	The.Renderer->RotateView(m_orientationDegrees);

	AABB2 texAABB = m_spriteSheet->GetTexCoordsForSpriteCoords(0, 1);
	The.Renderer->DrawTexturedAABB(*m_aabb, m_spriteSheet->GetTexture(), texAABB, WHITE);
	The.Renderer->PopView();
}

float EnemyTank::GetValueForSamenessOfVectors() const
{
	Vector2 forward(cos(m_orientationDegrees * DEG2RAD), sin(m_orientationDegrees * DEG2RAD));
	Vector2 toPlayer(g_theMap->GetPlayerPosition() - m_position);
	toPlayer.Normalize();

	return Vector2::Dot(forward, toPlayer);
}


//-----------------------------------------------------------------------------------------------
EnemyTank::~EnemyTank()
{
	delete m_spriteSheet;
}