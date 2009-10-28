#include "particle.h"
#include "util.h"


#define MAX_PARTICLES 5000

Vec4D fromARGB(uint32 color)
{
	const float a = ((color & 0xFF000000) >> 24) / 255.0f;
	const float r = ((color & 0x00FF0000) >> 16) / 255.0f;
	const float g = ((color & 0x0000FF00) >>  8) / 255.0f;
	const float b = ((color & 0x000000FF)      ) / 255.0f;
    return Vec4D(r,g,b,a);
}

template<class T>
T lifeRamp(float life, float mid, const T &a, const T &b, const T &c)
{
	if (life<=mid)
		return interpolate<T>(life / mid,a,b);
	else 
		return interpolate<T>((life-mid) / (1.0f-mid),b,c);
}


void ParticleSystem::init(MPQFile &f, ModelParticleEmitterDef &mta, int *globals)
{
	m_speed.init		(mta.params[0], f, globals);
	m_variation.init	(mta.params[1], f, globals);
	m_spread.init		(mta.params[2], f, globals);
	m_lat.init			(mta.params[3], f, globals);
	m_gravity.init		(mta.params[4], f, globals);
	m_lifespan.init		(mta.params[5], f, globals);
	m_rate.init			(mta.params[6], f, globals);
	m_areal.init		(mta.params[7], f, globals);
	m_areaw.init		(mta.params[8], f, globals);
	m_deacceleration.init(mta.params[9],f, globals);
	m_enabled.init		(mta.en,        f, globals);

	for (size_t i=0; i<3; i++) {
		m_colors[i] = fromARGB(mta.p.colors[i]);
		m_sizes[i] = mta.p.sizes[i] * mta.p.scales[i];
	}
	m_midLife = mta.p.mid;
	m_slowdown = mta.p.slowdown;
	m_rotation = mta.p.rotation;
	m_pos = fixCoordSystem(mta.pos);

	texture = model->textures[mta.texture];
	m_blend = mta.blend;
	m_rows = mta.rows;
	m_cols = mta.cols;
	m_type = mta.s1;
	//order = mta.s2;
	m_order = mta.s1>0 ? -1 : 0;
	parent = model->bones + mta.bone;

	m_anim = 0;
	m_time = 0;
	rem = 0;
    
	switch (mta.type) {
	case 1:
		emitter = new PlaneParticleEmitter(this);
		break;
	case 2:
		emitter = new SphereParticleEmitter(this);
		break;
	}

	//transform = mta.flags & 1024;

	// Flags && Type 1
	// 4121 = water ele
	// 4097 = water elemental
	// 1041 = Transcendance Halo
	// 1039 = water ele
	// 569 = Faith shoulders (angel wings)
	// 313 = Faith halo (centre flame?)
	// 49 = particle moving up?
	// 29 = particle being static
	// 25 = flame on weapon - move up/along the weapon
	// 17 = glow on weapon - static, random direction
	// 17 = aurastone - ?
	// 1 = perdition blade && water elemental

	// Type 2
	// 3145 = water ele
	// 1305 = water ele
	// 1049 = water elemental
	// 1033 = water elemental
	// 281 = water ele
	// 256 = Water elemental
	// 57 = Faith halo, ring?
	// 9 = water elemental

	billboard = !(mta.flags & 0x1000);

	// diagnosis test info
	pType = mta.type;
	flags = mta.flags;

	tofs = frand();

	// init tiles
	for (int i=0; i<m_rows*m_cols; i++) {
		TexCoordSet tc;
		initTile(tc.tc,i);
		tiles.push_back(tc);
	}

	m_accumulatedTime = 0;
}

void ParticleSystem::initTile(Vec2D *tc, int num)
{
	Vec2D otc[4];
	Vec2D a,b;
	int x = num % m_cols;
	int y = num / m_cols;
	a.x = x * (1.0f / m_cols);
	b.x = (x+1) * (1.0f / m_cols);
	a.y = y * (1.0f / m_rows);
	b.y = (y+1) * (1.0f / m_rows);

	otc[0] = a;
	otc[2] = b;
	otc[1].x = b.x;
	otc[1].y = a.y;
	otc[3].x = a.x;
	otc[3].y = b.y;

	for (int i=0; i<4; i++) {
		tc[(i+4-m_order) & 3] = otc[i];
	}
}


void ParticleSystem::update(float dt)
{
	// Check to make sure this particle system / emitter is enabled, if not, exit.
	bool en = m_enabled.getValue(m_anim, m_time) != 0;
	if (!en)
		return;

	m_accumulatedTime += dt;
	
	// Particle rate
	float fRate = m_rate.getValue(m_anim, m_time);
	// Particle life
	float fLife = m_lifespan.getValue(m_anim, m_time);

	// Make sure the emitter exists
	if (emitter) {
		
		// If the time elapse since our last particle is equal to or greater than our particle rate, create a new one.
		if (m_accumulatedTime >= (1.0f / fRate)) {
			m_accumulatedTime = 0;

			Particle p = emitter->newParticle(m_anim,m_time,0,0,0,0,0,0); //manim, mtime, w, l, spd, var, spr, spr2);
			// sanity check:
			//if (particles.size() < MAX_PARTICLES)
				particles.push_back(p);
		}
	}

	float grav = m_gravity.getValue(m_anim, m_time);
	float deaccel = m_deacceleration.getValue(m_anim, m_time);

	// Update all the particles in the list
	for (ParticleList::iterator it = particles.begin(); it != particles.end();) {
		Particle &p = *it;

		// Age the particle
		p.m_curLife += dt;

		// Store the old position
		p.m_prevPos = p.m_pos;

		// Modify the particles vector based on acceration and gravity
		//p.m_velocity += p.m_acceleration + p.m_weight;
		/*
		p.speed += p.down * grav * dt - p.dir * deaccel * dt;
		if (slowdown>0) {
			mspeed = expf(-1.0f * slowdown * p.life);
		}
		p.pos += p.speed * mspeed * dt;
		*/

		// Move the particle based on the new vector
		p.m_pos += (p.m_velocity * dt);
		p.m_pos.y += p.m_weight;


		// calculate size and color based on lifetime
		float rlife = p.m_curLife / p.m_life;
		p.m_size = lifeRamp<float>(rlife, m_midLife, m_sizes[0], m_sizes[1], m_sizes[2]);
		p.m_color = lifeRamp<Vec4D>(rlife, m_midLife, m_colors[0], m_colors[1], m_colors[2]);
		

		// If the particle has 'expired' then remove it.
		if (p.m_curLife >= p.m_life) 
			particles.erase(it++);
		else 
			++it;
	}
}

void ParticleSystem::setup(int anim, int time)
{
	m_anim = anim;
	m_time = time;

	/*
	if (transform) {
		// transform every particle by the parent trans matrix   - apparently this isn't needed
		Matrix m = parent->mat;
		for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
			it->tpos = m * it->pos;
		}
	} else {
		for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
			it->tpos = it->pos;
		}
	}
	*/
}

void ParticleSystem::draw()
{
	Vec3D bv0,bv1,bv2,bv3;

	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);

	// setup blend mode
	switch (m_blend) {
	case 0:	// Opaque (no blending)
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		break;
	case 1:	// Additive blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glDisable(GL_ALPHA_TEST);
		break;
	case 2:	// Standard blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_ALPHA_TEST);
		break;
	case 3:	// Alpha Keying
		glDisable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		break;
	case 4:	// Modulative blending
		glEnable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		break;
	}
	
	//glDisable(GL_LIGHTING);
	//glDisable(GL_CULL_FACE);
	//glDepthMask(GL_FALSE);

	/*
	if (!video.supportPointSprites) {// && m_rows==1 && m_cols==1) {
		// This is how will our point sprite's size will be modified by 
		// distance from the viewer
		float quadratic[] = {0.1f, 0.0f, 0.5f};
		//float quadratic[] = {0.88f, 0.001f, 0.000004f};
		glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

		// Query for the max point size supported by the hardware
		float maxSize = 512.0f;
		glGetFloatv(GL_POINT_SIZE_MAX_ARB, &maxSize );

		// Clamp size to 100.0f or the sprites could get a little too big on some  
		// of the newer graphic cards. My ATI card at home supports a max point 
		// size of 1024.0f!
		//if( maxSize > 100.0f )
		//	maxSize = 100.0f;

		glPointSize(maxSize);

		// The alpha of a point is calculated to allow the fading of points 
		// instead of shrinking them past a defined threshold size. The threshold 
		// is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
		// the minimum and maximum point sizes.
		glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);

		glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f );
		glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, maxSize );

		// Specify point sprite texture coordinate replacement mode for each texture unit
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
		// Render point sprites...
		glEnable(GL_POINT_SPRITE_ARB);

		glBegin(GL_POINTS);
		{
			for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
				glPointSize(it->m_size);
				//glTexCoord2fv(tiles[it->m_tile].tc[0]);
				glColor4fv(it->m_color);
				glVertex3fv(it->m_pos);
			}
		}
		glEnd();

		glDisable(GL_POINT_SPRITE_ARB);
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);

	} else { // Old slow method */

		Vec3D vRight;
		Vec3D vUp;
		
		if (billboard) {
			/*
			Matrix mbb;
			// get a billboard matrix
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mbb.m[0][0]));

			mbb.m[0][0] = 1;
			mbb.m[1][0] = 0;
			mbb.m[2][0] = 0;

			//mbb.m[0][1] = 0;
			//mbb.m[1][1] = 1;
			//mbb.m[2][1] = 0;
			
			mbb.m[0][2] = 0;
			mbb.m[1][2] = 0;
			mbb.m[2][2] = 1;
			
			glPushMatrix();
			glLoadMatrixf(&mbb.m[0][0]);
			*/
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

			vRight = Vec3D(modelview[0], modelview[4], modelview[8]);
			vUp = Vec3D(modelview[1], modelview[5], modelview[9]);

			/*
			a = center - (right + up) * size;
			b = center + (right - up) * size;
			c = center + (right + up) * size;
			d = center - (right - up) * size;
			*/
		}

		// position stuff
		float f = 1;//0.707106781f; // sqrt(2)/2
		bv0 = Vec3D(-f,+f,0);
		bv1 = Vec3D(+f,+f,0);
		bv2 = Vec3D(+f,-f,0);
		bv3 = Vec3D(-f,-f,0);

		glBegin(GL_QUADS);
		// TODO: per-particle rotation in a non-expensive way?? :|
		for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
			glColor4fv(it->m_color);

			glTexCoord2fv(tiles[it->m_tile].tc[0]);
			//glVertex3fv(it->m_pos + bv0 * it->m_size);
			glVertex3fv(it->m_pos - (vRight + vUp) * it->m_size);

			glTexCoord2fv(tiles[it->m_tile].tc[1]);
			//glVertex3fv(it->m_pos + bv1 * it->m_size);
			glVertex3fv(it->m_pos + (vRight - vUp) * it->m_size);

			glTexCoord2fv(tiles[it->m_tile].tc[2]);
			//glVertex3fv(it->m_pos + bv2 * it->m_size);
			glVertex3fv(it->m_pos + (vRight + vUp) * it->m_size);

			glTexCoord2fv(tiles[it->m_tile].tc[3]);
			//glVertex3fv(it->m_pos + bv3 * it->m_size);
			glVertex3fv(it->m_pos - (vRight - vUp) * it->m_size);
		}
		glEnd();

		if (billboard) {
			//glPopMatrix();
		}

	//	if (type==0 || type==2 ) {
	//		// TODO: figure out type 2 (deeprun tram subway sign)
	//		// - doesn't seem to be any different from 0 -_-
	//		// regular particles
	//		float f = 1;//0.707106781f; // sqrt(2)/2
	//		
	//		if (billboard) {
	//			glPushMatrix();
	//			float mat[] = {1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,-1.000000,0.000000,0.000000,0.000000,0.000000,1.000000};
	//			//glLoadMatrixf(mat);
	//		}

	//		if (billboard) {
	//			bv0 = mbb * Vec3D(-f,+f,0);
	//			bv1 = mbb * Vec3D(+f,+f,0);
	//			bv2 = mbb * Vec3D(+f,-f,0);
	//			bv3 = mbb * Vec3D(-f,-f,0);
	//		
	//			// TODO: per-particle rotation in a non-expensive way?? :|
	//			glBegin(GL_QUADS);
	//			for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
	//				glColor4fv(it->m_color);

	//				glTexCoord2fv(tiles[it->tile].tc[0]);
	//				glVertex3fv(it->pos + bv0 * it->size);

	//				glTexCoord2fv(tiles[it->tile].tc[1]);
	//				glVertex3fv(it->pos + bv1 * it->size);

	//				glTexCoord2fv(tiles[it->tile].tc[2]);
	//				glVertex3fv(it->pos + bv2 * it->size);

	//				glTexCoord2fv(tiles[it->tile].tc[3]);
	//				glVertex3fv(it->pos + bv3 * it->size);
	//			}
	//			glEnd();
	//		} else {

	//			glBegin(GL_QUADS);
	//			for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
	//				glColor4fv(it->m_color);

	//				glTexCoord2fv(tiles[it->tile].tc[0]);
	//				glVertex3fv(it->pos + it->corners[0] * it->size);

	//				glTexCoord2fv(tiles[it->tile].tc[1]);
	//				glVertex3fv(it->pos + it->corners[1] * it->size);

	//				glTexCoord2fv(tiles[it->tile].tc[2]);
	//				glVertex3fv(it->pos + it->corners[2] * it->size);

	//				glTexCoord2fv(tiles[it->tile].tc[3]);
	//				glVertex3fv(it->pos + it->corners[3] * it->size);
	//			}
	//			glEnd();

	//		}

	//		if (billboard)
	//			glPopMatrix();

	//	} else if (type==1) { // Sphere particles
	//		// particles from origin to position
	//		/*
	//		bv0 = mbb * Vec3D(0,-1.0f,0);
	//		bv1 = mbb * Vec3D(0,+1.0f,0);
	//		*/

	//		bv0 = mbb * Vec3D(-1.0f,0,0);
	//		bv1 = mbb * Vec3D(+1.0f,0,0);

	//		glBegin(GL_QUADS);
	//		for (ParticleList::iterator it = particles.begin(); it != particles.end(); ++it) {
	//			/*
	//			// What are these being used for?
	//			// From what I can tell, this is a huge bottle neck and is causing models to "disappear"
	//			Vec3D P,O;
	//			P=it->pos;
	//			O=it->origin;
	//			*/

	//			glColor4fv(it->m_color);

	//			glTexCoord2fv(tiles[it->tile].tc[0]);
	//			glVertex3fv(it->pos + bv0 * it->size);

	//			glTexCoord2fv(tiles[it->tile].tc[1]);
	//			glVertex3fv(it->pos + bv1 * it->size);

	//			glTexCoord2fv(tiles[it->tile].tc[2]);
	//			glVertex3fv(it->origin + bv1 * it->size);

	//			glTexCoord2fv(tiles[it->tile].tc[3]);
	//			glVertex3fv(it->origin + bv0 * it->size);
	//		}
	//		glEnd();
	//		
	//	}
	//}

	//glEnable(GL_LIGHTING);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_TRUE);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


Particle PlaneParticleEmitter::newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2)
{
    Particle p;
	
	// Set the particles life span
	p.m_curLife = 0;
	p.m_life = sys->m_lifespan.getValue(anim, time);

	// Set the particles size
	p.m_size = sys->m_sizes[0];

	// Get the particles start position
	w = sys->m_areaw.getValue(anim, time);
	l = sys->m_areal.getValue(anim, time);
	p.m_pos = sys->m_pos; //+ Vec3D(randfloat(-l,l), 0, randfloat(-w,w));
	p.m_prevPos = p.m_pos;
	
	// Get the particles vector
	var = sys->m_variation.getValue(anim,time);
	spd = sys->m_speed.getValue(anim, time);
	spr = sys->m_spread.getValue(anim, time);
	p.m_velocity = Vec3D(0,1,0) * spd * (1.0f+randfloat(-var,var));
	p.m_weight = 0.0f;
	// Vec3D dir = mrot * Vec3D(0,1,0);
	//p.m_acceleration

	// Randomly pick a texture 'tile' to start on.
	p.m_tile = randint(0, sys->m_rows * sys->m_cols - 1);

	/*
	//Spread Calculation
	Matrix mrot;

	CalcSpreadMatrix(spr,spr,1.0f,1.0f);
	mrot = sys->parent->mrot * SpreadMat;
	
	if (sys->flags == 1041) { // Trans Halo
		p.pos = sys->parent->mat * (sys->pos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w)));

		float t = randfloat(0,2*PI);

		p.pos = Vec3D(0.0f, sys->pos.y + 0.15f, sys->pos.z) + Vec3D(cos(t)/8, 0.0f, sin(t)/8); // Need to manually correct for the halo - why?
		
		// var isn't being used, which is set to 1.0f,  whats the importance of this?
		// why does this set of values differ from other particles

		Vec3D dir(0.0f, 1.0f, 0.0f);
		p.dir = dir;

		p.speed = dir.normalize() * spd * randfloat(0, var);
	} else if (sys->flags == 25 && sys->parent->parent<1) { // Weapon Flame
		p.pos = sys->parent->pivot * (sys->pos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
		Vec3D dir = mrot * Vec3D(0.0f, 1.0f, 0.0f);
		p.dir = dir.normalize();
		//Vec3D dir = sys->model->bones[sys->parent->parent].mrot * sys->parent->mrot * Vec3D(0.0f, 1.0f, 0.0f);
		//p.speed = dir.normalize() * spd;

	} else if (sys->flags == 25 && sys->parent->parent > 0) { // Weapon with built-in Flame (Avenger lightsaber!)
		p.pos = sys->parent->mat * (sys->pos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
		Vec3D dir = Vec3D(sys->parent->mat.m[1][0],sys->parent->mat.m[1][1], sys->parent->mat.m[1][2]) * Vec3D(0.0f, 1.0f, 0.0f);
		p.speed = dir.normalize() * spd * randfloat(0, var*2);

	} else if (sys->flags == 17 && sys->parent->parent<1) { // Weapon Glow
		p.pos = sys->parent->pivot * (sys->pos + Vec3D(randfloat(-l,l), randfloat(-l,l), randfloat(-w,w)));
		Vec3D dir = mrot * Vec3D(0,1,0);
		p.dir = dir.normalize();		
	} else {
		p.pos = sys->pos + Vec3D(randfloat(-l,l), 0, randfloat(-w,w));
		p.pos = sys->parent->mat * p.pos;

		Vec3D dir = mrot * Vec3D(0,1,0);
		p.dir = dir.normalize();
		p.down = Vec3D(0,-1.0f,0); // dir * -1.0f;
		p.speed = dir.normalize() * spd * (1.0f+randfloat(-var,var));
	}

	if(!sys->billboard)	{
		p.corners[0] = mrot * Vec3D(-1,0,+1);
		p.corners[1] = mrot * Vec3D(+1,0,+1);
		p.corners[2] = mrot * Vec3D(+1,0,-1);
		p.corners[3] = mrot * Vec3D(-1,0,-1);
	}

	p.life = 0;
	p.maxlife = sys->lifespan.getValue(anim, time);
	*/
	return p;
}

Particle SphereParticleEmitter::newParticle(int anim, int time, float w, float l, float spd, float var, float spr, float spr2)
{
	Particle p;

	/*
	// Sphere Particle Types and flag Info
	0x021D = Gas Clouds

	// Model Flags - *shrug* gotta write this down somewhere.
	// 0x1 =
	// 0x2 =
	// 0x4 =
	// 0x8 = 
	// 0x10 = 
	// 19 = 0x13 = blue ball in thunderfury = should be billboarded?

	// Planar Particle Flags
	// 0x1 =
	// 0x2 =
	// 0x4 =
	// 0x8 = 
	// 0x10 = 
	// 1041 = 0x411 = Halo
	// 49 = 0x31 = particle moving up?
	// 29 = 0x1D = particle being static
	// 25 = 0x19 = flame on weapon - move up/along the weapon
	// 17 = 0x11 = glow on weapon - static, random direction
	// 17 = 0x11 = aurastone - ?
	// 1 = 0x1 = perdition blade

	*/

	// Set the particles life span
	p.m_curLife = 0;
	p.m_life = sys->m_lifespan.getValue(anim, time);

	// Set the particles size
	p.m_size = sys->m_sizes[0];

	// Get the particles start position
	w = sys->m_areaw.getValue(anim, time);
	l = sys->m_areal.getValue(anim, time);

	// Get the particles vector
	var = sys->m_variation.getValue(anim,time);
	spd = sys->m_speed.getValue(anim, time);

	p.m_weight = 0.0f; //Vec3D(0,-1.0f,0);
	
	// Vec3D dir = mrot * Vec3D(0,1,0);
	//p.m_acceleration

	// Randomly pick a texture 'tile' to start on.
	p.m_tile = randint(0, sys->m_rows * sys->m_cols - 1);

	// TODO: fix shpere emitters to work properly
	float radius = randfloat(0,1);
	
	// New
	// Spread should never be zero for sphere particles ?
	float t;
	if (spr == 0)
		t = randfloat(-PI,PI);
	else
		t = randfloat(-spr,spr);

	// New
	// Length should never technically be zero ?
	//if (l==0)
	//	l = w;

	//Spread Calculation
	//CalcSpreadMatrix(spr*2,spr2*2,w,l);
	//Matrix mrot = sys->parent->mrot * SpreadMat;
	Matrix mrot = sys->parent->mrot;
	Vec3D bdir = mrot * Vec3D(0,1,0) * radius;

	//float temp = bdir.z;
	//bdir.z = bdir.y;
	//bdir.y = temp;

	Vec3D dir;

	/*
	if ((bdir.lengthSquared()==0) && ((sys->flags&0x100)!=0x100)) {
		p.m_velocity = Vec3D(0,0,0);
		dir = sys->parent->mrot * Vec3D(0,1,0);
	} else {
		if(sys->flags & 0x100)
			dir = sys->parent->mrot * Vec3D(0,1,0);
		else
			dir = bdir.normalize();
	*/

		p.m_velocity = bdir * spd * (1.0f+randfloat(-var,var));   // ?
	//}

	//p.m_velocity = Vec3D(0,1,0) * spd * (1.0f+randfloat(-var,var));
	//p.m_pos = sys->m_pos; //+ Vec3D(randfloat(-l,l), 0, randfloat(-w,w));
	p.m_pos = sys->parent->mat * sys->m_pos + bdir;
	p.m_prevPos = p.m_pos;

	//p.dir =  dir.normalize();//mrot * Vec3D(0, 1.0f,0);

	//if (sys->flags == 57 || sys->flags == 313) { // Faith Halo
	//	Vec3D bdir(w*cosf(t)*1.6, 0.0f, l*sinf(t)*1.6);

	//	p.pos = sys->pos + bdir;
	//	p.pos = sys->parent->mat * p.pos;

	//	if (bdir.lengthSquared()==0) 
	//		p.speed = Vec3D(0,0,0);
	//	else {
	//		dir = sys->parent->mrot * (bdir.normalize());//mrot * Vec3D(0, 1.0f,0);
	//		p.speed = dir.normalize() * spd * (1.0f+randfloat(-var,var));   // ?
	//	}
	//}	

	return p;
}




void RibbonEmitter::init(MPQFile &f, ModelRibbonEmitterDef &mta, int *globals)
{
	color.init(mta.color, f, globals);
	opacity.init(mta.opacity, f, globals);
	above.init(mta.above, f, globals);
	below.init(mta.below, f, globals);

	parent = model->bones + mta.bone;
	int *texlist = (int*)(f.getBuffer() + mta.ofsTextures);
	// just use the first texture for now; most models I've checked only had one
	texture = model->textures[texlist[0]];

	tpos = pos = fixCoordSystem(mta.pos);

	// TODO: figure out actual correct way to calculate length
	// in BFD, res is 60 and len is 0.6, the trails are very short (too long here)
	// in CoT, res and len are like 10 but the trails are supposed to be much longer (too short here)
	numsegs = (int)mta.res;
	seglen = mta.length;
	length = mta.res * seglen;

	// create first segment
	RibbonSegment rs;
	rs.pos = tpos;
	rs.len = 0;
	segs.push_back(rs);
}

void RibbonEmitter::setup(int anim, int time)
{
	Vec3D ntpos = parent->mat * pos;
	Vec3D ntup = parent->mat * (pos + Vec3D(0,0,1));
	ntup -= ntpos;
	ntup.normalize();
	float dlen = (ntpos-tpos).length();

	manim = anim;
	mtime = time;

	// move first segment
	RibbonSegment &first = *segs.begin();
	if (first.len > seglen) {
		// add new segment
		first.back = (tpos-ntpos).normalize();
		first.len0 = first.len;
		RibbonSegment newseg;
		newseg.pos = ntpos;
		newseg.up = ntup;
		newseg.len = dlen;
		segs.push_front(newseg);
	} else {
		first.up = ntup;
		first.pos = ntpos;
		first.len += dlen;
	}

	// kill stuff from the end
	float l = 0;
	bool erasemode = false;
	for (std::list<RibbonSegment>::iterator it = segs.begin(); it != segs.end(); ) {
		if (!erasemode) {
			l += it->len;
			if (l > length) {
				it->len = l - length;
				erasemode = true;
			}
			++it;
		} else {
			segs.erase(it++);
		}
	}

	tpos = ntpos;
	tcolor = Vec4D(color.getValue(anim, time), opacity.getValue(anim, time));
	
	tabove = above.getValue(anim, time);
	tbelow = below.getValue(anim, time);
}

void RibbonEmitter::draw()
{
	/*
	// placeholders
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor4f(1,1,1,1);
	glBegin(GL_TRIANGLES);
	glVertex3fv(tpos);
	glVertex3fv(tpos + Vec3D(1,1,0));
	glVertex3fv(tpos + Vec3D(-1,1,0));
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	*/

	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4fv(tcolor);

	glBegin(GL_QUAD_STRIP);
	std::list<RibbonSegment>::iterator it = segs.begin();
	float l = 0;
	for (; it != segs.end(); ++it) {
        float u = l/length;

		glTexCoord2f(u,0);
		glVertex3fv(it->pos + tabove * it->up);
		glTexCoord2f(u,1);
		glVertex3fv(it->pos - tbelow * it->up);

		l += it->len;
	}
	
	if (segs.size() > 1) {
		// last segment...?
		--it;
		glTexCoord2f(1,0);
		glVertex3fv(it->pos + tabove * it->up + (it->len/it->len0) * it->back);
		glTexCoord2f(1,1);
		glVertex3fv(it->pos - tbelow * it->up + (it->len/it->len0) * it->back);
	}
	glEnd();

	glColor4f(1,1,1,1);
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}





