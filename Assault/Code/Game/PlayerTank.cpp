#include "Game/PlayerTank.hpp"
#include "Game/Controller.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/EngineSystemManager.hpp"

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const int PlayerTank::kPlayerTankHealth = 10;


//-----------------------------------------------------------------------------------------------
PlayerTank::PlayerTank(const Vector2& startPos)
: m_spriteSheet(new SpriteSheet("Data/Images/MulticolorTanks.png", 8, 8))
, Tank(startPos, 90.f, kPlayerTankHealth)
{
	m_controller = new Controller(this);
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::Render() const
{
	The.Renderer->PushView();
	The.Renderer->TranslateView(m_position);
	The.Renderer->RotateView(m_orientationDegrees);

	AABB2 texAABB = m_spriteSheet->GetTexCoordsForSpriteCoords(0, 3);
	The.Renderer->DrawTexturedAABB(*m_aabb, m_spriteSheet->GetTexture(), texAABB, WHITE);
	The.Renderer->PopView();
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::Update(float deltaSeconds)
{
	m_controller->Update(deltaSeconds);
}

PlayerTank::~PlayerTank()
{
//	delete m_aabb;
	delete m_controller;
}
