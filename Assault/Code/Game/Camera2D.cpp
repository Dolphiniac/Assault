#include "Game/Camera2D.hpp"


//-----------------------------------------------------------------------------------------------
Camera2D::Camera2D(const WorldPosition& wp, float orientation, float numTilesInViewVertically)
: m_worldPosition(wp)
, m_orientationDegrees(orientation)
, m_numTilesInViewVertically(numTilesInViewVertically)
{

}