#pragma once


//-----------------------------------------------------------------------------------------------
class PlayerTank;
class XboxController;


//-----------------------------------------------------------------------------------------------
class Controller
{
public:
	Controller(PlayerTank* tank);
	void Update(float deltaSeconds);
	float AnalyzeForwardMovement();
	float AnalyzeSidewaysMovement(); 
	float AnalyzeTurn();
	~Controller();

private:
	PlayerTank*			m_tank;
	XboxController*		m_xinput;

private:
	float				m_secondsSinceLastFire;

	static const float kTankMovementSpeed;
	static const float kTankSpinSpeed;
	static const int kBulletsPerSecond;
	static const float kMinimumTimeBetweenFiringSeconds;
};