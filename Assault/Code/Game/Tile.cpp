#include "Game/Tile.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile()
: m_tileCoords(TileCoords(0, 0))
, m_aabb(0.f, 0.f, 0.f, 0.f)
, m_texture(nullptr)
, m_texCoords(0.f, 0.f, 0.f, 0.f)
, m_isSolid(false)
{
}


//-----------------------------------------------------------------------------------------------
Tile::Tile(const TileCoords& tileCoords)
: m_tileCoords(tileCoords)
, m_texture(nullptr)
, m_texCoords(0.f, 0.f, 0.f, 0.f)
, m_isSolid(false)
{
	TileCoords maxs(m_tileCoords.x + 1, m_tileCoords.y + 1);
	m_aabb = AABB2(m_tileCoords, maxs);
}


//-----------------------------------------------------------------------------------------------
void Tile::SetTypeFromTile(const Tile& prototype)
{
	TileCoords myCoords = m_tileCoords;
	AABB2 myAABB = m_aabb;
	*this = prototype;
	m_tileCoords = myCoords;
	m_aabb = myAABB;
}


//-----------------------------------------------------------------------------------------------
Tile::~Tile()
{
//	delete m_aabb;
//	delete m_texCoords;
}