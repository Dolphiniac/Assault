#pragma once

#include "Engine/Core/Audio.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;
class Map;
class Texture;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	TheGame();
	void Render() const;
	void Update(float deltaSeconds);
	void TogglePause();
	~TheGame();
	void GameOver(float deltaSeconds);
	void InitGameOver();
	bool m_isQuitting;

private:
	bool			m_isPaused;
	bool			m_gameBegun;
	bool			m_gameOver;

	float			m_alphaBlack;
	float			m_gameOverTime;
	float			m_timeRemainingToRespawn;
	bool			m_drawGameOver;
	FMOD::Channel*	m_attractChannel;
	FMOD::Channel*	m_mainMusicChannel;
};