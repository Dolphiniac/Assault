#include "Game/Bullet.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"

#include <math.h>

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const float Bullet::kBulletRadius = .2f;
STATIC const float Bullet::kBulletSpeedEnemy = 5.f;
STATIC const float Bullet::kBulletSpeedPlayer = 12.f;


//-----------------------------------------------------------------------------------------------
Bullet::Bullet(const Vector2& startPosition, float orientation, bool isPlayer)
: m_movementDirection(Vector2(cos(orientation * DEG2RAD), sin(orientation * DEG2RAD)))
, m_spriteSheet(new SpriteSheet("Data/Images/bullet.png", 5, 5))
, m_isPlayer(isPlayer)
, GameObject(startPosition, orientation, kBulletRadius, 0)
{
	Vector2 mins(-m_radius, -m_radius);
	Vector2 maxs(m_radius, m_radius);
	m_aabb = new AABB2(mins, maxs);
}


//-----------------------------------------------------------------------------------------------
void Bullet::Update(float deltaSeconds)
{
	m_position += deltaSeconds * m_movementDirection * ((m_isPlayer) ? kBulletSpeedPlayer : kBulletSpeedEnemy);
}


//-----------------------------------------------------------------------------------------------
void Bullet::Render() const
{
	Rgba color = (m_isPlayer) ? WHITE : RED;
	The.Renderer->PushView();
	The.Renderer->TranslateView(m_position);
	The.Renderer->RotateView(m_orientationDegrees);
	AABB2 texaabb = m_spriteSheet->GetTexCoordsForSpriteCoords(1, 2);
	The.Renderer->DrawTexturedAABB(*m_aabb, m_spriteSheet->GetTexture(), texaabb, color);
	The.Renderer->PopView();
}

//-----------------------------------------------------------------------------------------------
Bullet::~Bullet()
{
//	delete m_aabb;
	delete m_spriteSheet;
}
