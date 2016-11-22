#pragma once

#include "Game/GameObject.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteSheet;
class SpriteAnimation;
class AABB2;


//-----------------------------------------------------------------------------------------------
class Explosion : public GameObject
{
public:
	Explosion(const Vector2& position, float orientationDegrees, float radius, float durationSeconds);
	void Update(float deltaSeconds);
	void Render() const;
	bool ShouldDelete() const { return m_shouldDelete; }
	~Explosion();

private:
	SpriteSheet*			m_spriteSheet;
	SpriteAnimation*		m_spriteAnim;
	bool					m_shouldDelete;
	AABB2*					m_aabb;
};