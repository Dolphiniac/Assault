#pragma once
#include "Game/Tank.hpp"

#include <string>


//-----------------------------------------------------------------------------------------------
class Controller;
class SpriteSheet;


//-----------------------------------------------------------------------------------------------
class PlayerTank : public Tank
{
	friend class Controller;
	static const int kPlayerTankHealth;
public:
	PlayerTank(const Vector2& startPos);
	void Render() const;
	void Update(float deltaSeconds);
	~PlayerTank();

private:
	Controller*				m_controller;
	SpriteSheet*			m_spriteSheet;
};