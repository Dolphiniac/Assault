#include "Game/Controller.hpp"
#include "Game/TheGame.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/TheKeyboard.hpp"
#include "Engine/Core/EngineSystemManager.hpp"

#include <math.h>

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const float Controller::kTankMovementSpeed = 1.75f;
STATIC const float Controller::kTankSpinSpeed = 37.5f;
STATIC const int Controller::kBulletsPerSecond = 3;
STATIC const float Controller::kMinimumTimeBetweenFiringSeconds = 1.f / (float)Controller::kBulletsPerSecond;


//-----------------------------------------------------------------------------------------------
Controller::Controller(PlayerTank* tank)
: m_tank(tank)
, m_xinput(g_controllerOne)
, m_secondsSinceLastFire(0.f)
{
}


//-----------------------------------------------------------------------------------------------
void Controller::Update(float deltaSeconds)
{
	m_secondsSinceLastFire += deltaSeconds;
	
	float movSpeedY = AnalyzeForwardMovement() * kTankMovementSpeed * deltaSeconds;
	float movSpeedX = AnalyzeSidewaysMovement() * kTankMovementSpeed * deltaSeconds;
	float spinSpeed = AnalyzeTurn() * kTankSpinSpeed * deltaSeconds;

	m_tank->m_position += Vector2(movSpeedY * cos(m_tank->m_orientationDegrees * DEG2RAD), movSpeedY * sin(m_tank->m_orientationDegrees * DEG2RAD));
	m_tank->m_position += Vector2(movSpeedX * cos((m_tank->m_orientationDegrees - 90.f) * DEG2RAD), movSpeedX * sin((m_tank->m_orientationDegrees - 90.f) * DEG2RAD));
	m_tank->m_orientationDegrees += spinSpeed;

	if (m_xinput->GetTriggerBinary(RIGHT_TRIGGER) || The.Input->GetKey(0x20))
	{
		if (m_secondsSinceLastFire >= kMinimumTimeBetweenFiringSeconds)
		{
			float radius = m_tank->m_radius;
			float orientation = m_tank->m_orientationDegrees * DEG2RAD;
			Vector2 offsetFromCenter(radius * cos(orientation), radius * sin(orientation));

			g_theMap->SpawnBullet(m_tank->m_position + offsetFromCenter, m_tank->m_orientationDegrees, true);

			m_secondsSinceLastFire = 0.f;
		}
	}

	if (m_xinput->GetButtonDown(VB_START) || The.Input->GetKeyDown('P') || m_xinput->GetButtonDown(VB_BACK) || The.Input->GetKeyDown(0x1b))
	{
		g_theGame->TogglePause();
	}
}


//-----------------------------------------------------------------------------------------------
float Controller::AnalyzeForwardMovement()
{
	if (!m_xinput->IsConnected())
	{
		return The.Input->GetVerticalAxisBinary();
	}

	float verticalValue = m_xinput->GetVerticalAxis(LEFT_STICK);
	verticalValue += m_xinput->GetVerticalAxis(RIGHT_STICK);
	return verticalValue;
// 	float agreedVerticalStickValue = 0.f;
// 
// 	float leftY = m_xinput->GetVerticalAxis(LEFT_STICK);
// 	float rightY = m_xinput->GetVerticalAxis(RIGHT_STICK);
// 
// 	if (leftY > 0.f)
// 	{
// 		if (rightY > 0.f)
// 		{
// 
// 			agreedVerticalStickValue = Min(leftY, rightY);
// 			agreedVerticalStickValue = RangeMap(agreedVerticalStickValue, 0.f, .8f, 0.f, 1.f);
// 			agreedVerticalStickValue = Clampf(agreedVerticalStickValue, 0.f, 1.f);
// 		}
// 	}
// 	else if (leftY < 0.f)
// 	{
// 		if (rightY < 0.f)
// 		{
// 			agreedVerticalStickValue = Max(leftY, rightY);
// 			agreedVerticalStickValue = RangeMap(agreedVerticalStickValue, -.8f, 0.f, -1.f, 0.f);
// 			agreedVerticalStickValue = Clampf(agreedVerticalStickValue, -1.f, 0.f);
// 		}
// 	}
// 
// 	return agreedVerticalStickValue;
}


//-----------------------------------------------------------------------------------------------
float Controller::AnalyzeSidewaysMovement()
{
	if (!m_xinput->IsConnected())
	{
		return The.Input->GetHorizontalAxisBinary();
	}

	float horizontalValue = m_xinput->GetHorizontalAxis(LEFT_STICK);
	horizontalValue += m_xinput->GetHorizontalAxis(RIGHT_STICK);
	return horizontalValue;
// 	float agreedHorizontalStickValue = 0.f;
// 
// 	float leftX = m_xinput->GetHorizontalAxis(LEFT_STICK);
// 	float rightX = m_xinput->GetHorizontalAxis(RIGHT_STICK);
// 
// 	if (leftX > 0.f)
// 	{
// 		if (rightX > 0.f)
// 		{
// 
// 			agreedHorizontalStickValue = Min(leftX, rightX);
// 			agreedHorizontalStickValue = RangeMap(agreedHorizontalStickValue, 0.f, .8f, 0.f, 1.f);
// 			agreedHorizontalStickValue = Clampf(agreedHorizontalStickValue, 0.f, 1.f);
// 		}
// 	}
// 	else if (leftX < 0.f)
// 	{
// 		if (rightX < 0.f)
// 		{
// 			agreedHorizontalStickValue = Max(leftX, rightX);
// 			agreedHorizontalStickValue = RangeMap(agreedHorizontalStickValue, -.8f, 0.f, -1.f, 0.f);
// 			agreedHorizontalStickValue = Clampf(agreedHorizontalStickValue, -1.f, 0.f);
// 		}
// 	}
// 
// 	return agreedHorizontalStickValue;
}


//-----------------------------------------------------------------------------------------------
float Controller::AnalyzeTurn()
{
	if (!m_xinput->IsConnected())
	{
		return -The.Input->GetHorizontalAxisBinary();
	}
	return m_xinput->GetVerticalAxis(RIGHT_STICK) - m_xinput->GetVerticalAxis(LEFT_STICK);

// 	float rightY = m_xinput->GetVerticalAxis(RIGHT_STICK);
// 	float leftY = m_xinput->GetVerticalAxis(LEFT_STICK);
// 	float spinModifier = 0.f;
// 
// 	if (rightY != 0.f && leftY != 0.f)
// 	{
// 		spinModifier = m_xinput->GetVerticalAxis(RIGHT_STICK) - m_xinput->GetVerticalAxis(LEFT_STICK);
// 	}
// 
// 	return spinModifier; 
}

Controller::~Controller()
{
}
