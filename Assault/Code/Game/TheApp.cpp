#include "Game/TheApp.hpp"
#include "Engine/Math/Vector2.hpp"

#include <windows.h>
#include <gl/GL.h>


//-----------------------------------------------------------------------------------------------
TheApp *g_theApp = nullptr;
float g_counter = 0.f;
const float MILLISECONDS_PER_FRAME = 1000.f / 60.f;


//-----------------------------------------------------------------------------------------------
void RenderPolygon(Vector2 center, float radius, float numSides, float degreeOffset);


//-----------------------------------------------------------------------------------------------
TheApp::TheApp()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYS; keyIndex++)
	{
		m_isKeyDown[keyIndex] = 0;
	}
}


//-----------------------------------------------------------------------------------------------
void TheApp::SetKeyDownStatus(unsigned char asKey, bool isNowDown)
{
	m_isKeyDown[asKey] = isNowDown;
}


//-----------------------------------------------------------------------------------------------
void TheApp::Render()
{
	g_counter += .1f;
	Vector2 center(500.f, 500.f);
	SetColors();
	glClear(GL_COLOR_BUFFER_BIT);

	RenderPolygon(center, 200.f, 3.f, g_counter);
}


//-----------------------------------------------------------------------------------------------
void TheApp::SetColors()
{
	if (m_isKeyDown['B'] && m_canObjBeReset)
	{
		m_bgRGB = 1.f;
		GetSystemTime(&m_startTime);
	}
	else if (m_bgRGB > 0.f)
	{
		GetSystemTime(&m_currTime);
		//Had to use abs due to irregularities with the GetSystemTime function
		if (abs(m_currTime.wMilliseconds - m_startTime.wMilliseconds) >= MILLISECONDS_PER_FRAME)
		{
			GetSystemTime(&m_startTime);
			m_bgRGB -= 1.f / 60.f;
		}
	}

	if (m_isKeyDown['A'])
		m_objAlpha = 0.f;
	else
		m_objAlpha = 1.f;

	glClearColor(m_bgRGB, m_bgRGB, m_bgRGB, 1.f);
	glColor4f(0.f, 0.f, 1.f, m_objAlpha);

	SetObjReset(!m_isKeyDown['B']);
}


//-----------------------------------------------------------------------------------------------
void TheApp::SetObjReset(bool canBeReset)
{
	m_canObjBeReset = canBeReset;
}
