#include "Game/Tank.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"

#define STATIC


//-----------------------------------------------------------------------------------------------
STATIC const float Tank::kTankSize = .8f;


//-----------------------------------------------------------------------------------------------
Tank::Tank(const Vector2& position, float orientation, int health)
: GameObject(position, orientation, kTankSize / 2.f, health)
{
	Vector2 mins(-m_radius, -m_radius);
	Vector2 maxs(m_radius, m_radius);	//Bounding box to contain the tank

	m_aabb = new AABB2(mins, maxs);
	//m_texture = Texture::CreateOrGetTexture(m_texturePath);
	//GUARANTEE_OR_DIE(m_texture, Stringf("%s is not a valid texture path!\n", m_texturePath));
}

void Tank::operator=(const Tank& toCopy)
{
	m_aabb = toCopy.m_aabb;
}


//-----------------------------------------------------------------------------------------------
Tank::~Tank()
{
//	delete m_aabb;
}