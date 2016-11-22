#pragma once

#include "Game/GameObject.hpp"

#include <string>


//-----------------------------------------------------------------------------------------------
class AABB2;
class Texture;


//-----------------------------------------------------------------------------------------------
class Tank : public GameObject
{
	static const float kTankSize;
public:
	Tank(const Vector2& position, float orientation, int health);
	void operator=(const Tank& toCopy);
	virtual void Render() const = 0;
	virtual void Update(float deltaSeconds) = 0;
	~Tank();

protected:
	AABB2*					m_aabb;
// 	const std::string		m_texturePath = /*"Data/Images/Tank.png"*/"Data/Images/Tank.png";
// 	Texture*				m_texture;
};