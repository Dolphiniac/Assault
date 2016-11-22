#pragma once

#include "Engine/Math/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
typedef Vector2 WorldPosition;


//-----------------------------------------------------------------------------------------------
class Camera2D
{
private:
	WorldPosition		m_worldPosition;
	float				m_orientationDegrees;
	float				m_numTilesInViewVertically;

public:
	Camera2D(const WorldPosition& wp, float orientation, float numTilesInViewVertically);

	WorldPosition GetPos() const { return m_worldPosition; }
	float GetOrientation() const { return m_orientationDegrees; }
	float GetNumTilesInViewVertically() const { return m_numTilesInViewVertically; }

	void SetPos(const WorldPosition& wp) { m_worldPosition = wp; }
	void SetOrientation(float orientation) { m_orientationDegrees = orientation; }
	void SetNumTilesInViewVertically(float ntivw) { m_numTilesInViewVertically = ntivw; }
};