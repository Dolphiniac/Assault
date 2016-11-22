#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/TheKeyboard.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Renderer/GLRenderer.hpp"



//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
: m_isPaused(false)
, m_gameBegun(false)
, m_gameOver(false)
, m_alphaBlack(0.f)
, m_gameOverTime(0.f)
, m_timeRemainingToRespawn(0.f)
, m_drawGameOver(false)
, m_isQuitting(false)
{
	g_controllerOne = new XboxController(1);
	g_theMap = new Map();
	m_attractChannel = The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/attract.mp3"));
	m_attractChannel->setMode(FMOD_LOOP_NORMAL);
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update(float deltaSeconds)
{
	g_controllerOne->Update();

	if (!m_gameBegun)
	{
		if (g_controllerOne->GetButton(VB_START) || The.Input->GetKey('P'))
		{
			m_gameBegun = true;
			m_isPaused = false;
			m_attractChannel->stop();
			m_mainMusicChannel = The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/mainMusic.mp3"));
			m_mainMusicChannel->setMode(FMOD_LOOP_NORMAL);
		}

		if (g_controllerOne->GetButtonDown(VB_BACK) || The.Input->GetKeyDown(0x1b))
		{
			m_isQuitting = true;
		}
		return;
	}

	if (m_isPaused)
	{
		deltaSeconds = 0.f;

		if (g_controllerOne->GetButtonDown(VB_BACK) || The.Input->GetKeyDown(0x1b))
		{
			m_gameBegun = false;
			m_attractChannel = The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/attract.mp3"));
			m_attractChannel->setMode(FMOD_LOOP_NORMAL);
			delete g_theMap;
			g_theMap = new Map();
			return;
		}
	}

	g_theMap->Update(deltaSeconds);

	if (m_gameOver)
	{
		GameOver(deltaSeconds);
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	Vector2 bottomLeft(0.f, 0.f);
	Vector2 topRight(1600.f, 900.f);
	AABB2 aabb(bottomLeft, topRight);

	if (!m_gameBegun)
	{
		The.Renderer->SetOrtho(bottomLeft, topRight);
		The.Renderer->Clear(BLACK);
		The.Renderer->DrawTexturedAABB(aabb, Texture::CreateOrGetTexture("Data/Images/titleScreen.png"));
		return;
	}

	g_theMap->Render();

	if (m_isPaused)
	{
		The.Renderer->SetOrtho(bottomLeft, topRight);
		The.Renderer->DrawAABB(aabb, { 0.f, 0.f, 0.f, .3f });
		The.Renderer->DrawTexturedAABB(aabb, Texture::CreateOrGetTexture("Data/Images/Pause.png"));
	}

	if (m_gameOver)
	{
		The.Renderer->SetOrtho(bottomLeft, topRight);
		Rgba goColor = { 0.f, 0.f, 0.f, m_alphaBlack };
		The.Renderer->DrawAABB(aabb, goColor);

		if (m_drawGameOver)
		{
			The.Renderer->DrawTexturedAABB(aabb, Texture::CreateOrGetTexture("Data/Images/respawn.png"));
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::TogglePause()
{
	m_isPaused = !m_isPaused;
	The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/pause.wav"));
	m_mainMusicChannel->setPaused(m_isPaused);
}


//-----------------------------------------------------------------------------------------------
TheGame::~TheGame()
{
	delete g_controllerOne;
	delete g_theMap;
}


//-----------------------------------------------------------------------------------------------
void TheGame::InitGameOver()
{
	m_gameOver = true;
	m_alphaBlack = 0.f;
	m_timeRemainingToRespawn = 10.f;
	m_drawGameOver = false;
	m_gameOverTime = 0.f;
	m_mainMusicChannel->setPaused(true);
}


//-----------------------------------------------------------------------------------------------
void TheGame::GameOver(float deltaSeconds)
{
	float maxAlphaBlack = .3f;
	m_alphaBlack = Lerp(0.f, maxAlphaBlack, m_gameOverTime);

	if (m_alphaBlack >= maxAlphaBlack)
	{
		m_timeRemainingToRespawn -= deltaSeconds;
		m_drawGameOver = true;

		if (The.Input->GetKey('P') || g_controllerOne->GetButton(VB_START))
		{
			m_gameOver = false;
			g_theMap->Respawn();
			m_mainMusicChannel->setPaused(false);
		}

		if (m_timeRemainingToRespawn <= 0.f)
		{
			delete g_theMap;
			g_theMap = new Map();

			m_gameBegun = false;
			m_gameOver = false;
			m_attractChannel = The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/attract.mp3"));
			m_attractChannel->setMode(FMOD_LOOP_NORMAL);
		}
	}

	if (g_controllerOne->GetButtonDown(VB_BACK) || The.Input->GetKeyDown(0x1b))
	{
		delete g_theMap;
		g_theMap = new Map();

		m_gameBegun = false;
		m_gameOver = false;
		m_attractChannel = The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/attract.mp3"));
		m_attractChannel->setMode(FMOD_LOOP_NORMAL);
	}

	m_gameOverTime += deltaSeconds;
}