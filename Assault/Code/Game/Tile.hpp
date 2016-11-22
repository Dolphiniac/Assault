#pragma once

#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
class AABB2;
class Texture;


//-----------------------------------------------------------------------------------------------
class Tile
{
	friend class Map;
public:
	Tile();
	Tile(const TileCoords& tileCoords);
	bool IsSolid() const { return m_isSolid; }
	void SetTypeFromTile(const Tile& prototype);
	~Tile();

private:
	TileCoords		m_tileCoords;
	AABB2			m_aabb;
	Texture*		m_texture;
	AABB2			m_texCoords;
	bool			m_isSolid;
};