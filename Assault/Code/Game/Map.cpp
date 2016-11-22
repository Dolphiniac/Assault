#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/Bullet.hpp"
#include "Game/Turret.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/Explosion.hpp"
#include "Game/Camera2D.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Audio.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Core/FileUtils.hpp"

#define STBI_HEADER_FILE_ONLY
#include "ThirdParty/stb/stb_image.c"

#define STATIC


//-----------------------------------------------------------------------------------------------
Map* g_theMap = nullptr;


//-----------------------------------------------------------------------------------------------
const Vector2 BOTTOMLEFT(0.f, 0.f);
const Vector2 TOPRIGHT(20.f, 11.25f);
STATIC const float Map::kRespawnDelay = 5.f;


//-----------------------------------------------------------------------------------------------
Map::Map()
: m_timePlayerDead(0.f)
, m_currentMapIndex(1)
, m_groundTiles(new SpriteSheet("Data/Images/ardentryst_spritesheet.png", 12, 11))
{
	//Initialize tile types
	{
		m_stoneTile = new Tile();
		m_stoneTile->m_isSolid = true;
		m_stoneTile->m_texture = m_groundTiles->GetTexture();
		m_stoneTile->m_texCoords = m_groundTiles->GetTexCoordsForSpriteCoords(0, 2);

		m_grassTile = new Tile();
		m_grassTile->m_isSolid = false;
		m_grassTile->m_texture = m_groundTiles->GetTexture();
		m_grassTile->m_texCoords = m_groundTiles->GetTexCoordsForSpriteCoords(5, 10);
	}

	std::string toLoad = Stringf("Data/Maps/Map%i.png", m_currentMapIndex++);
	//Initialize map
	MapGen(toLoad);

	m_camera = new Camera2D(m_player->GetPosition(), m_player->GetOrientation(), TOPRIGHT.y);

// 	//Initialize map tiles
// 	for (int i = 0; i < m_size.y; i++)
// 	{
// 		for (int j = 0; j < m_size.x; j++)
// 		{
// 			TileCoords currCoords(j, i);
// 
// 			Tile currTile(currCoords);
// 			bool isStone = false;
// 			currTile.SetTypeFromTile(*m_grassTile);
// 
// 			if (currCoords.x == 0 || currCoords.x == m_size.x - 1 || currCoords.y == 0 || currCoords.y == m_size.y - 1) //Edges in stone
// 			{
// 				currTile.SetTypeFromTile(*m_stoneTile);
// 				isStone = true;
// 			}
// 
// 			if (rand() % 10 == 0 && currTile.m_tileCoords != TileCoords(1, 1)) //Random tiles (excluding start space) stone
// 			{
// 				currTile.SetTypeFromTile(*m_stoneTile);
// 				isStone = true;
// 			}
// 			m_tiles.push_back(currTile);
// 
// 			if (!isStone)
// 			{
// 				int rng = rand() % 180;
// 				if (currTile.m_tileCoords != TileCoords(1, 1))
// 				{
// 					if (rng == 0)
// 					{
// 						Turret* turret = new Turret(GetWorldCoordsForTileCenter(currTile));
// 						m_enemies.insert(turret);
// 						m_gameObjects.insert(turret);
// 					}
// 					else if (rng == 1)
// 					{
// 						EnemyTank* et = new EnemyTank(GetWorldCoordsForTileCenter(currTile));
// 						m_enemies.insert(et);
// 						m_gameObjects.insert(et);
// 						m_tanks.insert(et);
// 					}
// 				}
// 
// 			}
// 		}
// 	}
// 
// 	m_gameObjects.insert(m_player);
// 	m_tanks.insert(m_player);

	//m_camera = new Camera2D(m_player->GetPosition(), m_player->GetOrientation(), TOPRIGHT.y);
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsForTile(const Tile& tile) const
{
	return tile.m_tileCoords;
}


//-----------------------------------------------------------------------------------------------
Tile* Map::GetTileAtTileCoords(const TileCoords& tileCoords) const
{
	int index = GetIndexForTileCoords(tileCoords);
	if (index >= (int)m_tiles.size())
	{
		return nullptr;
	}

	return &GetTileAtIndex(index);
}


//-----------------------------------------------------------------------------------------------
Tile& Map::GetTileAtIndex(TileIndex tileIndex) const
{
	return (Tile&)m_tiles.at(tileIndex);
}


//-----------------------------------------------------------------------------------------------
Tile* Map::GetTileAtWorldCoords(const WorldCoords& worldCoords) const
{
	return GetTileAtTileCoords(GetTileCoordsForWorldCoords(worldCoords));
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsForWorldCoords(const WorldCoords& worldCoords) const
{
	int x = (int)worldCoords.x;
	int y = (int)worldCoords.y;

	return TileCoords(x, y);
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsForIndex(TileIndex tileIndex) const
{
	if (tileIndex == 0)
	{
		return TileCoords(0, 0);
	}

	int x = tileIndex % m_size.x;
	int y = tileIndex / m_size.x;

	return TileCoords(x, y);
}


//-----------------------------------------------------------------------------------------------
TileIndex Map::GetIndexForTileCoords(const TileCoords& tileCoords) const
{
	int result = m_size.x * tileCoords.y;
	result += tileCoords.x;
	return result;
}


//-----------------------------------------------------------------------------------------------
WorldCoords Map::GetWorldCoordsForTileCenter(const Tile& tile) const
{
	return GetWorldCoordsForTileCenter(tile.m_tileCoords);
}


//-----------------------------------------------------------------------------------------------
WorldCoords Map::GetWorldCoordsForTileCenter(const TileCoords& tileCoords) const
{
	float x = (float)tileCoords.x + .5f;
	float y = (float)tileCoords.y + .5f;

	return WorldCoords(x, y);
}


//-----------------------------------------------------------------------------------------------
void Map::Update(float deltaSeconds)
{
	for/*each*/ (GameObject* go : m_gameObjects)
	{
		go->Update(deltaSeconds);
	}
	EntityVsEntity();
	for (Tank* t : m_tanks)
	{
		PlayerVsTileCollide(t);
	}
	if (!m_player)
	{
		m_timePlayerDead += deltaSeconds;
	}
	BulletVsTileCollide();
	BulletVsEntityCollide();
	ExplosionExpire();

	if (m_enemies.size() == 0)
	{
		if (m_player)
		{
			delete m_player;
			m_tanks.erase(m_player);
			m_gameObjects.erase(m_player);
			m_collidables.erase(m_player);
			m_movables.erase(m_player);
		}
		m_playerSpawnPoint = Vector2::Zero;
		std::string toLoad = Stringf("Data/Maps/Map%i.png", m_currentMapIndex++);
		if (DoesFileExist(toLoad))
		{
			m_tiles.clear();
			m_enemies.clear();
			m_enemyBullets.clear();
			m_playerBullets.clear();
			m_tanks.clear();
			m_gameObjects.clear();

			MapGen(toLoad);
		}
		else
		{
			g_theGame->m_isQuitting = true;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	Vector2 cameraCenteredOnScreenPosition(TOPRIGHT.x * .5f, TOPRIGHT.y * .2f);
	if (m_player)
	{
		m_camera->SetPos(m_player->GetPosition());
		m_camera->SetOrientation(m_player->GetOrientation() - 90.f);
	}

	The.Renderer->SetOrtho(BOTTOMLEFT, TOPRIGHT);
	The.Renderer->Clear(BLACK);

	The.Renderer->TranslateView(cameraCenteredOnScreenPosition);
	The.Renderer->RotateView(-m_camera->GetOrientation());
	The.Renderer->TranslateView(-m_camera->GetPos());
	
	for/*each*/ (Tile t : m_tiles)
	{
		Rgba color = WHITE;
		The.Renderer->DrawTexturedAABB((t.m_aabb), t.m_texture, t.m_texCoords, color);
	}

// 	//Draw vertical grid lines
// 	for (int i = 0; i < m_size.x; i++)
// 	{
// 		Vector2 startVec((float)i, 0.f);
// 		Vector2 endVec((float)i, (float)m_size.y);
// 		g_theRenderer->DrawLine(startVec, endVec, WHITE, WHITE, 1.f);
// 	}
// 
// 	//Draw horizontal grid lines
// 	for (int i = 0; i < m_size.y; i++)
// 	{
// 		Vector2 startVec(0.f, (float)i);
// 		Vector2 endVec((float)m_size.x, (float)i);
// 		g_theRenderer->DrawLine(startVec, endVec, WHITE, WHITE, 1.f);
// 	}

	for/*each*/ (GameObject* go : m_gameObjects)
	{
		go->Render();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnBullet(const Vector2& position, float orientation, bool isPlayer)
{
	Bullet* bullet = new Bullet(position, orientation, isPlayer);
	if (isPlayer)
	{
		m_playerBullets.insert(bullet);
	}
	else
	{
		m_enemyBullets.insert(bullet);
	}

	m_gameObjects.insert(bullet);

	The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/fireBullet.wav"));
}


//-----------------------------------------------------------------------------------------------
void Map::PlayerVsTileCollide(Tank* tank)
{
	CollideCardinally(tank);
	CollideDiagonally(tank);
}


//-----------------------------------------------------------------------------------------------
void Map::CollideCardinally(Tank* tank)
{
	WorldCoords position = tank->GetPosition();
	float radius = tank->GetRadius();

	Tile* north = GetTileAtWorldCoords(WorldCoords(position.x, position.y + 1.f));
	Tile* south = GetTileAtWorldCoords(WorldCoords(position.x, position.y - 1.f));
	Tile* east = GetTileAtWorldCoords(WorldCoords(position.x + 1.f, position.y));
	Tile* west = GetTileAtWorldCoords(WorldCoords(position.x - 1.f, position.y));

	if (north->IsSolid())
	{
		float amountOverlapped = ((float)north->m_tileCoords.y - position.y);
		if (amountOverlapped < radius)
		{
			position.y -= (radius - amountOverlapped);
			tank->SetPosition(position);
		}
	}

	if (south->IsSolid())
	{
		float amountOverlapped = (position.y - ((float)south->m_tileCoords.y + 1.f));
		if (amountOverlapped < radius)
		{
			position.y += (radius - amountOverlapped);
			tank->SetPosition(position);
		}
	}

	if (east->IsSolid())
	{
		float amountOverlapped = ((float)east->m_tileCoords.x - position.x);
		if (amountOverlapped < radius)
		{
			position.x -= (radius - amountOverlapped);
			tank->SetPosition(position);
		}
	}

	if (west->IsSolid())
	{
		float amountOverlapped = (position.x - ((float)west->m_tileCoords.x + 1.f));
		if (amountOverlapped < radius)
		{
			position.x += (radius - amountOverlapped);
			tank->SetPosition(position);
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CollideDiagonally(Tank* tank)
{
	WorldCoords position = tank->GetPosition();
	float radius = tank->GetRadius();

	Tile* nw = GetTileAtWorldCoords(WorldCoords(position.x - 1.f, position.y + 1.f));
	Tile* sw = GetTileAtWorldCoords(WorldCoords(position.x - 1.f, position.y - 1.f));
	Tile* ne = GetTileAtWorldCoords(WorldCoords(position.x + 1.f, position.y + 1.f));
	Tile* se = GetTileAtWorldCoords(WorldCoords(position.x + 1.f, position.y - 1.f));

	if (nw->IsSolid())
	{
		Vector2 corner((float)nw->m_tileCoords.x + 1.f, (float)nw->m_tileCoords.y);
		Vector2 displacement = corner - position;

		float displacementLength = displacement.Length();

		if (displacementLength < radius)
		{
			float lengthFactor = (radius - displacementLength);
			displacement.Normalize();
			displacement *= lengthFactor;
			position -= displacement;
			tank->SetPosition(position);
		}
	}
	if (sw->IsSolid())
	{
		Vector2 corner((float)sw->m_tileCoords.x + 1.f, (float)sw->m_tileCoords.y + 1.f);
		Vector2 displacement = corner - position;

		float displacementLength = displacement.Length();

		if (displacementLength < radius)
		{
			float lengthFactor = (radius - displacementLength);
			displacement.Normalize();
			displacement *= lengthFactor;
			position -= displacement;
			tank->SetPosition(position);
		}
	}
	if (ne->IsSolid())
	{
		Vector2 corner((float)ne->m_tileCoords.x, (float)ne->m_tileCoords.y);
		Vector2 displacement = corner - position;

		float displacementLength = displacement.Length();

		if (displacementLength < radius)
		{
			float lengthFactor = (radius - displacementLength);
			displacement.Normalize();
			displacement *= lengthFactor;
			position -= displacement;
			tank->SetPosition(position);
		}
	}
	if (se->IsSolid())
	{
		Vector2 corner((float)se->m_tileCoords.x, (float)se->m_tileCoords.y + 1.f);
		Vector2 displacement = corner - position;

		float displacementLength = displacement.Length();

		if (displacementLength < radius)
		{
			float lengthFactor = (radius - displacementLength);
			displacement.Normalize();
			displacement *= lengthFactor;
			position -= displacement;
			tank->SetPosition(position);
		}
	}
}


//Amanatides-Woo---------------------------------------------------------------------------------
RaycastResult2D Map::Raycast2D(const WorldCoords& startPosition, const WorldCoords& endPosition) const
{
	TileCoords currTileCoords = GetTileCoordsForWorldCoords(startPosition);
	RaycastResult2D result;

	if (GetTileAtTileCoords(currTileCoords)->IsSolid())
	{
		result.didImpact = true;
		result.impactFraction = 0.f;
		result.impactPosition = startPosition;
		result.impactSurfaceNormal = Vector2::Zero;
		result.impactTileCoords = currTileCoords;
		return result;
	}

	Vector2 displacement(endPosition - startPosition);
	
	float tDeltaX = 1.f / displacement.x;
	if (displacement.x == 0.f)
	{
		tDeltaX = 10001.f;
	}
	int tileStepX = (displacement.x > 0.f) ? 1 : -1;
	int offsetToLeadingEdgeX = (tileStepX + 1) / 2;
	float firstVerticalIntersectionX = (float)(currTileCoords.x + offsetToLeadingEdgeX);
	float tOfNextXCrossing = abs(firstVerticalIntersectionX - startPosition.x) * tDeltaX;

	float tDeltaY = 1.f / displacement.y;
	if (displacement.y == 0.f)
	{
		tDeltaY = 10000.f;
	}
	int tileStepY = (displacement.y > 0.f) ? 1 : -1;
	int offsetToLeadingEdgeY = (tileStepY + 1) / 2;
	float firstVerticalIntersectionY = (float)(currTileCoords.y + offsetToLeadingEdgeY);
	float tOfNextYCrossing = abs(firstVerticalIntersectionY - startPosition.y) * tDeltaY;

	for (;;)
	{
		if (abs(tOfNextXCrossing) < abs(tOfNextYCrossing)) //I seemed to need this.  My visual testing had trouble without testing abs of these values
		{
			if (abs(tOfNextXCrossing) > 1.f)
			{
				result.didImpact = false;
				return result;
			}

			currTileCoords.x += tileStepX;
			if (GetTileAtTileCoords(currTileCoords)->IsSolid())
			{
				result.didImpact = true;
				result.impactFraction = tOfNextXCrossing;
				result.impactPosition = startPosition + (displacement * tOfNextXCrossing);
				result.impactSurfaceNormal = Vector2((float)-tileStepX, 0.f);
				result.impactTileCoords = currTileCoords;
				return result;
			}

			tOfNextXCrossing += tDeltaX;
		}
		else
		{
			if (abs(tOfNextYCrossing) > 1.f)
			{
				result.didImpact = false;
				return result;
			}

			currTileCoords.y += tileStepY;
			if (GetTileAtTileCoords(currTileCoords)->IsSolid())
			{
				result.didImpact = true;
				result.impactFraction = tOfNextYCrossing;
				result.impactPosition = startPosition + (displacement * tOfNextYCrossing);
				result.impactSurfaceNormal = Vector2(0.f, (float)-tileStepY);
				result.impactTileCoords = currTileCoords;
				return result;
			}

			tOfNextYCrossing += tDeltaY;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::BulletVsTileCollide()
{
	for (auto iterator = m_enemyBullets.begin(); iterator != m_enemyBullets.end();)
	{
		Bullet* bullet = *iterator;
		if (GetTileAtWorldCoords(bullet->GetPosition())->IsSolid())
		{
			BulletExplosion(bullet);
			delete bullet;
			m_gameObjects.erase(bullet);
			m_enemyBullets.erase(iterator++);
		}
		else
		{
			iterator++;
		}
	}

	for (auto iterator = m_playerBullets.begin(); iterator != m_playerBullets.end();)
	{
		Bullet* bullet = *iterator;
		if (GetTileAtWorldCoords(bullet->GetPosition())->IsSolid())
		{
			BulletExplosion(bullet);
			delete bullet;
			m_gameObjects.erase(bullet);
			m_playerBullets.erase(iterator++);
		}
		else
		{
			iterator++;
		}
	}
}

void Map::BulletVsEntityCollide()
{
	if (m_player)
	{
		for (auto iterator = m_enemyBullets.begin(); iterator != m_enemyBullets.end();)
		{
			Bullet* bullet = *iterator;
			if (DoDiscsOverlap(bullet->GetPosition(), bullet->GetRadius(), m_player->GetPosition(), m_player->GetRadius()))
			{
				BulletExplosion(bullet);
				delete bullet;
				m_gameObjects.erase(bullet);
				m_enemyBullets.erase(iterator++);
				m_player->TakeDamage();
				if (m_player->IsDead())
				{
					PlayerDeathExplosion();
					g_theGame->InitGameOver();
					delete m_player;
					m_gameObjects.erase(m_player);
					m_tanks.erase(m_player);
					m_collidables.erase(m_player);
					m_movables.erase(m_player);
					m_player = nullptr;
				}
				break;
			}
			else
			{
				iterator++;
			}
		}
	}

	for (auto bulletIterator = m_playerBullets.begin(); bulletIterator != m_playerBullets.end();)
	{
		Bullet* bullet = *bulletIterator;
		bool collided = false;

		for (auto enemyIterator = m_enemies.begin(); enemyIterator != m_enemies.end();)
		{
			GameObject* enemy = *enemyIterator;

			if (DoDiscsOverlap(bullet->GetPosition(), bullet->GetRadius(), enemy->GetPosition(), enemy->GetRadius()))
			{
				BulletExplosion(bullet);
				delete bullet;
				m_gameObjects.erase(bullet);
				m_playerBullets.erase(bulletIterator++);
				enemy->TakeDamage();
				if (enemy->IsDead())
				{
					EnemyDeathExplosion(enemy);
					delete enemy;
					m_gameObjects.erase(enemy);
					m_collidables.erase(enemy);
					m_movables.erase(enemy);
					m_tanks.erase((Tank*)enemy);
					m_enemies.erase(enemyIterator++);
				}
				collided = true;
				break;
			}
			else
			{
				enemyIterator++;
			}
		}

		if (!collided)
		{
			bulletIterator++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DestroyBullet(Bullet* toDestroy)
{
	if (toDestroy->IsPlayer())
	{
		m_playerBullets.erase(toDestroy);
	}
	else
	{
		m_enemyBullets.erase(toDestroy);
	}
	delete toDestroy;
	m_gameObjects.erase(toDestroy);
}


//-----------------------------------------------------------------------------------------------
void Map::DestroyPlayer()
{
	delete m_player;
	m_gameObjects.erase(m_player);
	m_collidables.erase(m_player);
	m_movables.erase(m_player);
	m_tanks.erase(m_player);
	m_player = nullptr;
}

void Map::DestroyEnemy(GameObject* toDestroy)
{
	delete toDestroy;
	m_enemies.erase(toDestroy);
	m_gameObjects.erase(toDestroy);
	m_collidables.erase(toDestroy);
	m_movables.erase(toDestroy);
}


//-----------------------------------------------------------------------------------------------
Vector2 Map::GetPlayerPosition() const
{
	if (m_player)
	{
		return m_player->GetPosition();
	}
	else
	{
		return Vector2::Zero;
	}
}


//-----------------------------------------------------------------------------------------------
void Map::MapGen(const std::string& mapFilePath)
{
	IntVector2 texelSize;
	int numComponents = 0;
	int numComponentsRequested = 0;
	unsigned char* imageData = stbi_load(mapFilePath.c_str(), &texelSize.x, &texelSize.y, &numComponents, numComponentsRequested);

	m_size = TileCoords(texelSize.x, texelSize.y);

	int rowLength = texelSize.x * numComponents;
	int startingIndex = texelSize.x * texelSize.y * numComponents;
	int tileIndex = 0;

	for (startingIndex -= rowLength; startingIndex >= 0; startingIndex -= rowLength)
	{
		for (int currentIndex = startingIndex; currentIndex < startingIndex + rowLength; currentIndex += numComponents)
		{
			Tile newTile(GetTileCoordsForIndex(tileIndex));
			tileIndex++;

			int red = imageData[currentIndex];
			int green = imageData[currentIndex + 1];
			int blue = imageData[currentIndex + 2];

			int compositeColor = 0;
			compositeColor += (red << 16);
			compositeColor += (green << 8);
			compositeColor += blue;

			Turret* turret = nullptr;
			EnemyTank* enemyTank = nullptr;

			switch (compositeColor)
			{
			case HEX_GRASS:
				newTile.SetTypeFromTile(*m_grassTile);
				break;
			case HEX_STONE:
				newTile.SetTypeFromTile(*m_stoneTile);
				break;
			case HEX_PLAYER:
				newTile.SetTypeFromTile(*m_grassTile);
				ASSERT_OR_DIE(m_playerSpawnPoint == Vector2::Zero, Stringf("Error at tileIndex %i: Player spawn point already encountered\n", tileIndex));

				m_playerSpawnPoint = GetWorldCoordsForTileCenter(newTile);
				m_player = new PlayerTank(m_playerSpawnPoint);
				m_gameObjects.insert(m_player);
				m_collidables.insert(m_player);
				m_movables.insert(m_player);
				m_tanks.insert(m_player);
				break;
			case HEX_ENEMY_TANK:
				newTile.SetTypeFromTile(*m_grassTile);
				enemyTank = new EnemyTank(GetWorldCoordsForTileCenter(newTile));
				m_gameObjects.insert(enemyTank);
				m_collidables.insert(enemyTank);
				m_movables.insert(enemyTank);
				m_enemies.insert(enemyTank);
				m_tanks.insert(enemyTank);
				break;
			case HEX_ENEMY_TURRET:
				newTile.SetTypeFromTile(*m_grassTile);
				turret = new Turret(GetWorldCoordsForTileCenter(newTile));
				m_gameObjects.insert(turret);
				m_collidables.insert(turret);
				m_enemies.insert(turret);
				break;
			default:
				ERROR_AND_DIE(Stringf("Error at tileIndex %i: Unrecognized color 0x%06x\n", tileIndex, compositeColor));
			}

			m_tiles.push_back(newTile);
		}
	}

	ASSERT_OR_DIE(m_playerSpawnPoint != Vector2::Zero, "No player spawn point determined!\n");
}


//-----------------------------------------------------------------------------------------------
void Map::ExplosionExpire()
{
	for (auto explosionIter = m_explosions.begin(); explosionIter != m_explosions.end();)
	{
		Explosion* explosion = *explosionIter;
		
		if (explosion->ShouldDelete())
		{
			delete explosion;
			explosionIter++;
			m_gameObjects.erase(explosion);
			m_explosions.erase(explosion);
		}
		else
		{
			explosionIter++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::BulletExplosion(Bullet* bullet)
{
	float orientation = bullet->GetOrientation();
	Vector2 pos = bullet->GetPosition();
	float size = bullet->GetRadius();

	Explosion* explosion = new Explosion(pos, orientation, size, .5f);
	m_gameObjects.insert(explosion);
	m_explosions.insert(explosion);
	The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/espload.wav"));
}


//-----------------------------------------------------------------------------------------------
void Map::EnemyDeathExplosion(GameObject* enemy)
{
	float orientation = enemy->GetOrientation();
	Vector2 pos = enemy->GetPosition();
	float size = enemy->GetRadius();

	Explosion* explosion = new Explosion(pos, orientation, size, 1.5f);
	m_gameObjects.insert(explosion);
	m_explosions.insert(explosion);
	if (m_tanks.find((Tank*)enemy) != m_tanks.end())
	{
		The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/tankEspload.wav"));
	}
	else
	{
		The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/turretEspload.wav"));
	}
}


//-----------------------------------------------------------------------------------------------
void Map::PlayerDeathExplosion()
{
	float orientation = m_player->GetOrientation();
	Vector2 pos = m_player->GetPosition();
	float size = m_player->GetRadius() * 2.f;

	Explosion* explosion = new Explosion(pos, orientation, size, 2.f);
	m_gameObjects.insert(explosion);
	m_explosions.insert(explosion);
	The.Audio->PlaySound(The.Audio->CreateOrGetSound("Data/Audio/playerEspload.wav"));
}


void Map::EntityVsEntity()
{
	for (auto firstIter = m_collidables.begin(); firstIter != m_collidables.end(); firstIter++)
	{
		GameObject* firstEntity = *firstIter;
		auto secondIter = firstIter;
		secondIter++;

		for (; secondIter != m_collidables.end(); secondIter++)
		{
			GameObject* secondEntity = *secondIter;

			if (!DoDiscsOverlap(firstEntity->GetPosition(), firstEntity->GetRadius(), secondEntity->GetPosition(), secondEntity->GetRadius()))
			{
				continue;
			}

			Vector2 displacement(secondEntity->GetPosition() - firstEntity->GetPosition());
			float displacementLength = displacement.Length();

			float pushLength = firstEntity->GetRadius() + secondEntity->GetRadius() - displacementLength;

			displacement.Normalize();

			displacement *= pushLength * 0.5f;

			bool moveFirst;
			bool moveSecond;

			moveFirst = (m_movables.find(firstEntity) == m_movables.end()) ? false : true;
			moveSecond = (m_movables.find(secondEntity) == m_movables.end()) ? false : true;

			if (moveFirst)
			{
				firstEntity->SetPosition(firstEntity->GetPosition() - displacement);
			}
			if (moveSecond)
			{
				secondEntity->SetPosition(secondEntity->GetPosition() + displacement);
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::Respawn()
{
	m_timePlayerDead = 0.f;
	m_player = new PlayerTank(m_playerSpawnPoint);
	m_gameObjects.insert(m_player);
	m_collidables.insert(m_player);
	m_movables.insert(m_player);
	m_tanks.insert(m_player);
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	delete m_camera;
	delete m_groundTiles;
	delete m_grassTile;
	delete m_stoneTile;
	for (GameObject* go : m_gameObjects)
		delete go;
}