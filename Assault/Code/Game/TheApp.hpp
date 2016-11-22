#pragma once
#include <windows.h>


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
class TheApp;
class Vector2;
void RenderPolygon(Vector2 center, float radius, float numSides, float degreeOffset);


//-----------------------------------------------------------------------------------------------
extern TheApp *g_theApp;


//-----------------------------------------------------------------------------------------------
class TheApp
{
public:
	TheApp();
	void SetKeyDownStatus(unsigned char asKey, bool isNowDown);
	void Render();
	void SetColors();
	void SetObjReset(bool canBeReset);
protected:
	bool		m_isKeyDown[NUM_KEYS];
	float		m_bgRGB = 0.f;
	float		m_objAlpha;
	bool		m_canObjBeReset = true;
	SYSTEMTIME	m_startTime;
	SYSTEMTIME	m_currTime;
};

