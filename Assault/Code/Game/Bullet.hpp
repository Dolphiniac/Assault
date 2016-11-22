#pragma once

#include "Game/GameObject.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;
class AABB2;
class SpriteSheet;


//-----------------------------------------------------------------------------------------------
class Bullet : public GameObject
{
	static const float kBulletRadius;
	static const float kBulletSpeedEnemy;
	static const float kBulletSpeedPlayer;

public:
	Bullet(const Vector2& startPosition, float orientation, bool isPlayer);
	void Update(float deltaSeconds);
	void Render() const;
	bool IsPlayer() const { return m_isPlayer; }
	~Bullet();

private:
	Vector2				m_movementDirection;
	SpriteSheet*		m_spriteSheet;
	bool				m_isPlayer;
	AABB2*				m_aabb;
};

