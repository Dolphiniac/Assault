#include "Game/Explosion.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimation.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Renderer/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
Explosion::Explosion(const Vector2& position, float orientationDegrees, float radius, float durationSeconds)
: m_spriteSheet(new SpriteSheet("Data/Images/explosion.png", 5, 5))
, m_spriteAnim(new SpriteAnimation(*m_spriteSheet, durationSeconds, SPRITE_ANIM_MODE_PLAY_TO_END, 0, 24))
, m_shouldDelete(false)
, m_aabb(new AABB2(Vector2(-radius, -radius), Vector2(radius, radius)))
, GameObject(position, orientationDegrees, radius, 1)
{

}


//-----------------------------------------------------------------------------------------------
void Explosion::Update(float deltaSeconds)
{
	m_spriteAnim->Update(deltaSeconds);

	if (m_spriteAnim->IsFinished())
	{
		m_shouldDelete = true;
	}
}


//-----------------------------------------------------------------------------------------------
void Explosion::Render() const
{
	The.Renderer->PushView();
	The.Renderer->TranslateView(m_position);
	The.Renderer->RotateView(m_orientationDegrees);

	AABB2 texCoords = m_spriteAnim->GetCurrentTexCoords();

	The.Renderer->DrawTexturedAABB(*m_aabb, m_spriteAnim->GetTexture(), texCoords, WHITE, ADDITIVE);

	The.Renderer->PopView();
}


//-----------------------------------------------------------------------------------------------
Explosion::~Explosion()
{
	delete m_spriteSheet;
	delete m_spriteAnim;
//	delete m_aabb;
}