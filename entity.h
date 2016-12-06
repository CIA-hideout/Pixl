// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 entity.h v1.0

#ifndef _ENTITY_H               // Prevent multiple definitions if this 
#define _ENTITY_H               // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "image.h"
#include "input.h"
#include "game.h"
#include <map>

namespace entityNS {
	enum COLLISION_TYPE { NONE, CIRCLE, BOX, ROTATED_BOX };
	const float GRAVITY = 6.67428e-11f;         // gravitational constant
}

// types of object
enum ObjectType {
	OBJECT_TYPE_PLAYER,
	OBJECT_TYPE_TRIANGLE,
	OBJECT_TYPE_SQUARES,
	OBJECT_TYPE_CIRCLE,
	OBJECT_TYPE_BOSS,
	OBJECT_TYPE_PICKUP,
	OBJECT_TYPE_BLACKHOLE,
	OBJECT_TYPE_GUI_HEART,
	OBJECT_TYPE_MISSILE,
	OBJECT_TYPE_EXPLOSION
};

// types of effects that an object may have
enum EffectType {
	EFFECT_SLOW,
	EFFECT_STUN,
	EFFECT_INVINCIBLE,
	EFFECT_INVERTED,
	EFFECT_INVULNERABLE,
	EFFECT_FROZEN,
	EFFECT_ENLARGED,
	EFFECT_CANNOT_PICKUP
};

class Entity : public Image {
protected:

	entityNS::COLLISION_TYPE	collisionType;
	VECTOR2						center;										// center of entity
	VECTOR2						distSquared;								// used for calculating circle collision
	float						radius;										// radius of collision circle
	float						sumRadiiSquared;

	RECT						edge;										// for BOX and ROTATED_BOX collision detection
	VECTOR2						corners[4];									// for ROTATED_BOX collision detection
	VECTOR2						edge01, edge03;								// edges used for projection
	VECTOR2						velocity;									// velocity
	VECTOR2						deltaV;										// added to velocity during next call to update()
	float						edge01Min, edge01Max, edge03Min, edge03Max;	// min and max projections
	float						mass;										// Mass of entity
	float						health;										// health 0 to 100
	float						rr;											// Radius squared variable
	float						force;										// Force of gravity
	float						gravity;									// gravitational constant of the game universe
	bool						active;										// only active entities may collide
	bool						rotatedBoxReady;							// true when rotated collision box is ready
	Input						*input;										// pointer to the input system
	HRESULT						hr;											// standard return type

	std::string					name;
	int							id;
	ObjectType					objectType;

	std::map<EffectType, float> effectTimers;								// effect timers of the effects that an entity might have


	// --- The following functions are protected because they are not intended to be
	// --- called from outside the class.
	// Circular collision detection 
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideCircle(Entity &ent, VECTOR2 &collisionVector);
	// Axis aligned box collision detection
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideBox(Entity &ent, VECTOR2 &collisionVector);
	// Separating axis collision detection between boxes
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideRotatedBox(Entity &ent, VECTOR2 &collisionVector);
	// Separating axis collision detection between box and circle
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideRotatedBoxCircle(Entity &ent, VECTOR2 &collisionVector);
	// Separating axis collision detection helper functions
	void computeRotatedBox();
	bool projectionsOverlap(Entity &ent);
	bool collideCornerCircle(VECTOR2 corner, Entity &ent, VECTOR2 &collisionVector);

public:
	// Constructor
	Entity();
	// Destructor
	virtual ~Entity() {}

	virtual const VECTOR2* getCenter()
	{
		center = VECTOR2(getCenterX(), getCenterY());
		return &center;
	}

	void setHeight(int height) { this->spriteData.height = height; }
	void setWidth(int width) { this->spriteData.width = width; }

	// Return radius of collision circle.
	virtual float getRadius() const     { return radius; }

	// Return RECT structure used for BOX and ROTATED_BOX collision detection.
	virtual const RECT& getEdge() const { return edge; }

	// Return corner c of ROTATED_BOX
	virtual const VECTOR2* getCorner(UINT c) const
	{
		if (c >= 4)
			c = 0;
		return &corners[c];
	}

	// Return velocity vector.
	virtual const VECTOR2 getVelocity() const { return velocity; }

	// Return active.
	virtual bool  getActive()         const { return active; }

	// Return mass.
	virtual float getMass()           const { return mass; }

	// Return gravitational constant.
	virtual float getGravity()        const { return gravity; }

	// Return health;
	virtual float getHealth()         const { return health; }

	// Return collision type (NONE, CIRCLE, BOX, ROTATED_BOX)
	virtual entityNS::COLLISION_TYPE getCollisionType() { return collisionType; }

	ObjectType getObjectType() { return objectType; }
	void setObjectType(ObjectType objectType) { this->objectType = objectType; }

	int getObjectId() { return id; }
	void setObjectId(int id) { this->id = id; }

	// Set velocity.
	virtual void  setVelocity(VECTOR2 v)    { velocity = v; }
	virtual void  setVelocity(float x, float y) { velocity.x = x; velocity.y = y; }

	// Set delta velocity. Added to velocity in update().
	virtual void  setDeltaV(VECTOR2 dv)     { deltaV = dv; }

	// Set active.
	virtual void  setActive(bool a)         { active = a; }

	// Set health.
	virtual void setHealth(float h)         { health = h; }

	// Set mass.
	virtual void  setMass(float m)          { mass = m; }

	// Set gravitational constant. Default is 6.67428e-11
	virtual void  setGravity(float g)       { gravity = g; }

	// Set radius of collision circle.
	virtual void setCollisionRadius(float r)    { radius = r; }

	// Update Entity.
	// typically called once per frame
	// deltaTime is used to regulate the speed of movement and animation
	virtual void update(float deltaTime);

	// Initialize Entity
	// Pre: *gamePtr = pointer to Game object
	//      width = width of Image in pixels  (0 = use full texture width)
	//      height = height of Image in pixels (0 = use full texture height)
	//      ncols = number of columns in texture (1 to n) (0 same as 1)
	//      *textureM = pointer to TextureManager object
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols,
							TextureManager *textureM);
	// Activate Entity.
	virtual void activate();

	// Empty ai function to allow Entity objects to be instantiated.
	virtual void ai(float deltaTime, Entity &ent);

	// Is this entity outside the specified rectangle?
	virtual bool outsideRect(RECT rect);

	// Does this entity collide with ent?
	virtual bool collidesWith(Entity &ent, VECTOR2 &collisionVector);

	// Damage this Entity with weapon.
	virtual void damage(int weapon);

	// Entity bounces after collision with other Entity
	void bounce(VECTOR2 &collisionVector, Entity &ent);

	// Adds the gravitational force to the velocity vector of this entity
	void gravityForce(Entity *other, float deltaTime);

	std::map<EffectType, float>* getEffectTimers() { return &this->effectTimers; };

	// returns effect true if timer for effect > 0.0f
	bool hasEffect(EffectType effectType) { return this->effectTimers.at(effectType) > 0.0f; }
};

#endif
