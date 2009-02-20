#ifndef PARTICLE_H
#define PARTICLE_H

class ParticleSystem;
class RibbonEmitter;

#include "model.h"
#include "animated.h"

#include <list>

struct Particle
{
	// Position
	Vec3D m_pos;           // current position of the particle
	Vec3D m_prevPos;       // last position of the particle
  
	// Forces / Speeds / Vectors
	Vec3D m_velocity;      // direction and speed
	Vec3D m_acceleration;  // acceleration
	float m_weight;           // determines how gravity affects the particle
	//float   m_weightDelta;      // change over time

	float m_life;           // determines how long the particle is alive
	float m_curLife;		// says how long particle has been alive for
	
	// Size / colour info
	float m_size;             // size of particle
	//float   m_sizeDelta;        // amount to change the size over time
	Vec4D m_color;         // current color of the particle
	//Vec4D   m_colorDelta;    // how the color changes with time
	
	// Texture tile info
	int m_tile;
};

/*
struct Particle {
	Vec3D pos, speed, down, origin, dir;
	Vec3D	corners[4];
	//Vec3D tpos;
	float size, life, maxlife;
	int tile;
	Vec4D color;
};
*/

typedef std::list<Particle> ParticleList;

class ParticleEmitter {
protected:
	ParticleSystem *sys;
public:
	ParticleEmitter(ParticleSystem *sys): sys(sys) {}
	virtual Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2) = 0;
};

class PlaneParticleEmitter: public ParticleEmitter {
public:
	PlaneParticleEmitter(ParticleSystem *sys): ParticleEmitter(sys) {}
	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
};

class SphereParticleEmitter: public ParticleEmitter {
public:
	SphereParticleEmitter(ParticleSystem *sys): ParticleEmitter(sys) {}
	Particle newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2);
};

struct TexCoordSet {
    Vec2D tc[4];
};

class ParticleSystem {
	ParticleEmitter *emitter;
	ParticleList particles;

	Animated<float> m_speed, m_variation, m_spread, m_lat, m_gravity, m_lifespan, m_rate, m_areal, m_areaw, m_deacceleration;
	Animated<uint8> m_enabled;

	float m_accumulatedTime;  // used to track how long since the last particle was emitted
	float m_midLife, m_slowdown, m_rotation;
	Vec3D m_pos;

	Vec4D m_colors[3];
	float m_sizes[3];
	
	GLuint texture;
	

	int m_blend, m_order, m_type;
	int m_anim, m_time;
	int m_rows, m_cols;

	std::vector<TexCoordSet> tiles;
	void initTile(Vec2D *tc, int num);
	bool billboard;

	float rem;

	// unknown parameters omitted for now ...
	int32 flags;
	int16 pType;

	Bone *parent;

public:
	Model *model;
	float tofs;

	ParticleSystem(): emitter(0)
	{
		m_blend = 0;
		m_order = 0;
		m_type = 0;
		m_anim = 0;
		m_time = 0;
		m_rows = 0;
		m_cols = 0;

		m_midLife = 0;
		m_slowdown = 0;
		m_rotation = 0;

		m_accumulatedTime = 0;

		model = 0;
		parent = 0;
		texture = 0;

		rem = 0;
		tofs = 0;
	}
	~ParticleSystem() { delete emitter; }

	void init(MPQFile &f, ModelParticleEmitterDef &mta, int *globals);
	void update(float dt);

	void setup(int anim, int time);
	void draw();

	friend class PlaneParticleEmitter;
	friend class SphereParticleEmitter;
};


struct RibbonSegment {
	Vec3D pos, up, back;
	float len,len0;
};

class RibbonEmitter {
	Animated<Vec3D> color;
	AnimatedShort opacity;
	Animated<float> above, below;

	Bone *parent;
	float f1, f2;

	Vec3D pos;

	int manim, mtime;
	float length, seglen;
	int numsegs;
	
	Vec3D tpos;
	Vec4D tcolor;
	float tabove, tbelow;

	GLuint texture;

	std::list<RibbonSegment> segs;

public:
	Model *model;

	void init(MPQFile &f, ModelRibbonEmitterDef &mta, int *globals);
	void setup(int anim, int time);
	void draw();
};



#endif
