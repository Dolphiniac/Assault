#pragma once

#include "Game/GameCommon.hpp"

#include <vector>
#include <set>


//-----------------------------------------------------------------------------------------------
const int HEX_GRASS = 0x02ff41;
const int HEX_STONE = 0x8d9eae;
const int HEX_PLAYER = 0x0213ff;
const int HEX_ENEMY_TANK = 0xff0303;
const int HEX_ENEMY_TURRET = 0xeeff03;


//-----------------------------------------------------------------------------------------------
class Tile;
class GameObject;
class PlayerTank;
class Camera2D;
class SpriteSheet;
class Bullet;
class Map;
class Tank;
class Explosion;


//-----------------------------------------------------------------------------------------------
extern Map* g_theMap;


//-----------------------------------------------------------------------------------------------
struct RaycastResult2D
{
	bool didImpact;
	Vector2 impactPosition;
	TileCoords impactTileCoords;
	float impactFraction;
	Vector2 impactSurfaceNormal;
};


//-----------------------------------------------------------------------------------------------
class Map
{
	static const float kRespawnDelay;
public:
	Map();
	void Update(float deltaSeconds);
	void Render() const;
	void SpawnBullet(const Vector2& position, float orientation, bool isPlayer);
	Vector2 GetPlayerPosition() const;
	RaycastResult2D Raycast2D(const WorldCoords& startPosition, const WorldCoords& endPosition) const;
	void Respawn();
	~Map();

private:
	std::vector<Tile>			m_tiles;
	TileCoords					m_size;
	std::set<GameObject*>		m_gameObjects;
	std::set<Bullet*>			m_enemyBullets;
	std::set<Bullet*>			m_playerBullets;
	std::set<GameObject*>		m_enemies; 
	std::set<Tank*>				m_tanks;
	std::set<Explosion*>		m_explosions;
	std::set<GameObject*>		m_collidables;
	std::set<GameObject*>		m_movables;
	PlayerTank*					m_player;
	Camera2D*					m_camera;
	SpriteSheet*				m_groundTiles;
	Tile*						m_grassTile;
	Tile*						m_stoneTile;
	float						m_timePlayerDead;
	Vector2						m_playerSpawnPoint;
	int							m_currentMapIndex;

private:
	TileCoords GetTileCoordsForTile(const Tile& tile) const;
	Tile* GetTileAtTileCoords(const TileCoords& tileCoords) const;
	Tile& GetTileAtIndex(TileIndex tileIndex) const;
	Tile* GetTileAtWorldCoords(const WorldCoords& worldCoords) const;
	TileCoords GetTileCoordsForWorldCoords(const WorldCoords& worldCoords) const;
	TileCoords GetTileCoordsForIndex(TileIndex tileIndex) const;
	TileIndex GetIndexForTileCoords(const TileCoords& tileCoords) const;
	WorldCoords GetWorldCoordsForTileCenter(const Tile& tile) const;
	WorldCoords GetWorldCoordsForTileCenter(const TileCoords& tileCoords) const;
	void PlayerVsTileCollide(Tank* tank);
	void CollideCardinally(Tank* tank);
	void CollideDiagonally(Tank* tank);
	void BulletVsTileCollide();
	void BulletVsEntityCollide();
	void DestroyBullet(Bullet* toDestroy);
	void DestroyPlayer();
	void DestroyEnemy(GameObject* toDestroy);
	void MapGen(const std::string& mapFilePath);
	void ExplosionExpire();
	void BulletExplosion(Bullet* bullet);
	void EnemyDeathExplosion(GameObject* enemy);
	void PlayerDeathExplosion();
	void EntityVsEntity();
};