#ifndef WMO_H
#define WMO_H

// STL
#include <vector>
#include <set>
#include <string>

// Our headers
#include "manager.h"
#include "vec3d.h"
#include "mpq.h"
#include "model.h"
#include "video.h"
#include "displayable.h"

class WMO;
class WMOGroup;
class WMOInstance;
class WMOManager;
//class Liquid;


class WMOGroup {
	WMO *wmo;
	int flags;
	int nTriangles, nVertices;
	GLuint dl,dl_light;
	Vec3D center;
	float rad;
	int num;
	int fog;
	int nDoodads;
	short *ddr;
	//Liquid *lq;
public:
	Vec3D v1,v2;
	Vec3D b1,b2;
	Vec3D vmin, vmax;
	bool indoor, hascv;
	bool visible;
	bool ok;

	bool outdoorLights;
	std::string name, desc;

	WMOGroup() : dl(0), ddr(0) {}
	~WMOGroup();
	void init(WMO *wmo, MPQFile &f, int num, char *names);
	void initDisplayList();
	void initLighting(int nLR, short *useLights);
	void draw();
	void drawLiquid();
	void drawDoodads(int doodadset);
	void setupFog();
	void cleanup();

	void updateModels(bool load);
};

struct WMOMaterial {
	int flags;
	int d1;
	int transparent;
	int nameStart;
	unsigned int col1;
	int d3;
	int nameEnd;
	unsigned int col2;
	int d4;
	float f1,f2;
	int dx[5];
	// read up to here -_-
	TextureID tex;
};

struct WMOLight {
	unsigned int flags, color;
	Vec3D pos;
	float intensity;
	float unk[5];
	float r;

	Vec4D fcolor;

	void init(MPQFile &f);
	void setup(GLint light);

	static void setupOnce(GLint light, Vec3D dir, Vec3D lcol);
};

struct WMOPV {
	Vec3D a,b,c,d;
};

struct WMOPR {
	short portal, group, dir, reserved;
};

struct WMODoodadSet {
	char name[0x14];
	int start;
	int size;
	int unused;
};

struct WMOLiquidHeader {
	int X, Y, A, B;
	Vec3D pos;
	short type;
};

struct WMOFog {
	unsigned int flags;
	Vec3D pos;
	float r1, r2, fogend, fogstart;
	unsigned int color1;
	float f2;
	float f3;
	unsigned int color2;
	// read to here (0x30 bytes)
	Vec4D color;
	void init(MPQFile &f);
	void setup();
};

class WMOModelInstance {
public:
	Model *model;
	std::string filename;

	int id;

	Vec3D pos, dir;
	unsigned int d1, scale;

	float frot,w,sc;

	int light;
	Vec3D ldir;
	Vec3D lcol;

	WMOModelInstance() {}
    void init(char *fname, MPQFile &f);
	void draw();

	void loadModel(ModelManager &mm);
	void unloadModel(ModelManager &mm);
};


class WMO: public ManagedItem, public Displayable {
public:
	WMOGroup *groups;
	int nTextures, nGroups, nP, nLights, nModels, nDoodads, nDoodadSets, nX;
	WMOMaterial *mat;
	Vec3D v1,v2;
	bool ok;
	char *groupnames;
	std::vector<std::string> textures;
	std::vector<std::string> models;
	std::vector<WMOModelInstance> modelis;
	ModelManager loadedModels;

	std::vector<WMOLight> lights;
	std::vector<WMOPV> pvs;
	std::vector<WMOPR> prs;

	std::vector<WMOFog> fogs;

	std::vector<WMODoodadSet> doodadsets;

	Model *skybox;
	int sbid;

	WMO(std::string name);
	~WMO();
	
	int doodadset;
	bool includeDefaultDoodads;
	
	void draw();
	void drawSkybox();
	void drawPortals();
	
	void update(int dt);

	void loadGroup(int id);
	void showDoodadSet(int id);
	void updateModels();
};

/*
class WMOManager: public SimpleManager {
public:
	int add(std::string name);
};


class WMOInstance {
	static std::set<int> ids;
public:
	WMO *wmo;
	Vec3D pos;
	Vec3D pos2, pos3, dir;
	int id, d2, d3;
	int doodadset;

	WMOInstance(WMO *wmo, MPQFile &f);
	void draw();
	//void drawPortals();

	static void reset();
};
*/


#endif
