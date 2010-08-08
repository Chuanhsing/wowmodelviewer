#ifndef MODELEXPORT_M3_H
#define MODELEXPORT_M3_H

/* http://code.google.com/p/libm3/wiki/Structs */

struct VEC2
{
    float x, y;
};

struct VEC3
{
    float x, y, z;
};

struct VEC4
{
    float x, y, z, w;
};

struct QUAT
{
    float x, y, z, w;
};

//Size = 32 byte / 0x20 byte
struct sphere
{
	Vec3D min;
	Vec3D max;
	float radius;
	uint32 flags;
};

struct matrix
{
	Vec4D a;
	Vec4D b;
	Vec4D c;
	Vec4D d;
	uint32 flags;
};

// Size = 12 byte / 0x0C byte
// Complete, struct HeadRef
struct Reference 
{
    /*0x00*/ uint32 nEntries; // nData
    /*0x04*/ uint32 ref; // indData
	/*0x08*/ uint32 flags;
};

// Size = 16 byte / 0x10 byte
// Incomplete, struct Tag
struct ReferenceEntry 
{ 
    /*0x00*/ char id[4]; // blockID
    /*0x04*/ uint32 offset; // ofs
    /*0x08*/ uint32 nEntries; // counts
    /*0x0C*/ uint32 vers; // Possibly nReferences;
};

// Size = 8 Byte / 0x08 byte
// Incomplete
struct AnimRef
{
    /*0x00*/ uint16 flags; //usually 1
    /*0x02*/ uint16 animflag; //6 indicates animation data present
    /*0x04*/ uint32 animid; //a unique uint32 value referenced in STC.animid and STS.animid
};

// Size = 24 byte / 0x18 byte
// Complete
struct MD34
{ 
    /*0x00*/ char id[4]; 
    /*0x04*/ uint32 ofsRefs;
    /*0x08*/ uint32 nRefs;
	/*0x0C*/ Reference mref;
	/*0x18*/ char padding[8];
};

// Size = 784 byte / 0x310 byte
struct MODL
{
	Reference Modelname;
	uint32 type;
	Reference mSEQS;
	Reference mSTC;
	Reference mSTG;
	Vec3D v3d1;
	uint32 d10;
	Reference mSTS;
	Reference mBone;
	uint32 nSkinnedBones;
	uint32 vertFlags;
	Reference mVert;
	Reference mDIV;
	Reference mBoneLU;
	sphere boundSphere;
	int d2[15];
	Reference mAttach;
	Reference mAttachLU;
	Reference mLite;
	Reference mSHBX;
	Reference mCam;
	Reference D;
	Reference mMatLU;
	Reference mMat;
	Reference mDIS;
	Reference mCMP;
	Reference mTER;
	Reference mVOL;
	Reference r1;
	Reference mCREP;
	Reference mPar;
	Reference mParc;
	Reference mRibbon;
	Reference mPROJ;
	Reference mFOR;
	Reference mWRP;
	Reference r2;
	Reference mPHRB;
	Reference r3[3];
	Reference mIKJT;
	Reference r4;
	Reference mPATU;
	Reference mTRGD;
	Reference mIREF;
	int d7[2];
	matrix mat;
	sphere ext2;
	Reference mSGSS;
	Reference mATVL;
	Reference F;
	Reference G;
	Reference mBBSC;
	Reference mTMD;
	uint32 d9[4];
};

// Size = 156 byte / 0x9C byte
// Incomplete
struct BONE
{
    /*0x00*/ int32 d1; // Keybone?
    /*0x04*/ Reference name;
    /*0x0C*/ uint32 flags; //2560 = Weighted vertices rendered, 512 = not rendered
    /*0x10*/ int16 parent;
    /*0x12*/ uint16 s1;
    /*0x14*/ AnimRef transid; //unique animation ID ref
    /*0x1C*/ VEC3 pos; //bone position is relative to parent bone and its rotation
    /*0x28*/ int32 d2[4];
    /*0x38*/ AnimRef rotid;
    /*0x40*/ QUAT rot; //initial bone rotation
    /*0x50*/ int32 d3[3];
    /*0x5C*/ float f1;
    /*0x60*/ int32 d4;
    /*0x64*/ AnimRef scaleid;
    /*0x6C*/ VEC3 scale; //initial scale
    /*0x78*/ VEC3 v1;
    /*0x84*/ int32 d5[6];
    //appears to have another animation ref at 0x8C but not sure what for
};

// Size = 8 byte / 0x08 byte
// Complete
struct MATM
{
    /*0x00*/ uint32 nmat; //usually only 1
    /*0x04*/ uint32 matind; //MAT index
};

// Size = 212 bytes / 0xD4 bytes
// Incomplete
struct MAT
{
    Reference name;
    int32 d1[8];
    float x, y;
    Reference layers[13];//0 - Diffuse, 1 - Decal, 2 - Specular, 3 - Emissive, 9 - Normal
    int32 d2[15];
};

// Size = 352 bytes / 0x160 bytes
// Incomplete
struct LAYR
{
    uint32 d1;
    Reference name;
    AnimRef ar1;
    uint32 d2[6]; //d2[5] determines team colour
    AnimRef ar2;
    float f1[2];
    uint32 d3;
    AnimRef ar3;
    uint32 d4[4];
    int32 d5;
    uint32 d6[5];
    AnimRef ar4;
    uint32 d7[3];
    AnimRef ar5;
    uint32 d8[5];
    AnimRef ar6;
    uint32 d9[2];
    AnimRef ar7;
    float f2[2];
    uint32 d10[3];
    AnimRef ar8;
    float f3;
    uint32 d11;
    float f4;
    uint32 d12[4];
    AnimRef ar9;
    float f5[4];
    uint32 d13;
    AnimRef ar10;
    uint32 d14[3];
    AnimRef ar11;
    float f6[2];
    uint32 d15;
    AnimRef ar12;
    float f7[2];
    uint32 d16;
    int32 d17;
    uint32 d18;
    float f8;
    uint32 d19;
    float f9[2];
};

// Size = 32 byte / 0x20 byte
// Incomplete
struct DIV
{
    /*0x00*/ Reference faces;
    /*0x08*/ Reference REGN;
    /*0x10*/ Reference BAT;
    /*0x18*/ Reference MSEC;
};

// Size = 14 byte / 0x0E byte
// Incomplete
struct BAT
{
    /*0x00*/ uint32 d1;
    /*0x04*/ uint16 subid; //REGN index
    /*0x06*/ uint16 s1[2];
    /*0x0A*/ uint16 matid; //MATM index (MATM is a material lookup table)
    /*0x0C*/ int16 s2; //usually -1
};

//Size = 28 byte / 0x1C byte
// Incomplete
struct REGN
{
    /*0x00*/ uint32 d1; //always 0?
    /*0x04*/ uint16 indVert;
    /*0x06*/ uint16 numVert;
    /*0x08*/ uint32 indFaces;
    /*0x0C*/ uint32 numFaces;
    /*0x10*/ uint16 boneCount; //boneLookup total count (redundant?)
    /*0x12*/ uint16 indBone; //indice into boneLookup
    /*0x14*/ uint16 numBone; //number of bones used from boneLookup
    /*0x16*/ uint16 s1[3]; //flags? vital for sc2 engine rendering the geometry
};

// Size = 72 byte / 0x48 byte
// Incomplete
struct MSEC
{
    /*0x00*/ uint32 d1; //always 0?
    /*0x04*/ AnimRef bounds; //Bounding box animation ref in STC
    /*0x0C*/ //extent ext1; //Some kind of mesh extent? TODO
    /*0x28*/ uint32 d2[8];
};

// Size = 176 byte / 0xB0 byte
// Incomplete
struct CAM
{
    /*0x00*/ int32 d1;
    /*0x04*/ Reference name;
    /*0x0C*/ uint16 flags1;
    /*0x0E*/ uint16 flags2;
};

// Size = 388 byte / 0x184 byte
// Incomplete
struct PROJ
{
};

// Size = 96 byte/ 0x60 byte
// Incomplete
struct EVNT
{
    /*0x00*/ Reference name;
    /*0x08*/ int16 unk1[4];
    /*0x10*/ float matrix[4][4];
    /*0x50*/ int32 unk2[4];
};

// Size = 16 byte / 0x10 byte
// Incomplete
struct ATT
{
    /*0x00*/ int32 unk;
    /*0x04*/ Reference name;
    /*0x0C*/ int32 bone;
};

// Size = 24 byte / 0x18 byte
// Complete
struct SD
{
    /*0x00*/ Reference timeline;
    /*0x08*/ uint32 flags;
    /*0x0C*/ uint32 length;
    /*0x08*/ Reference data;
};

// Size = 96 byte / 0x60 byte
// Incomplete
struct SEQS
{
    /*0x00*/ int32 d1;
    /*0x04*/ int32 d2;
    /*0x08*/ Reference name;
    /*0x14*/ int32 d3;
    /*0x18*/ int32 length;
    /*0x1C*/ float moveSpeed;
    /*0x20*/ uint32 flags;
    /*0x24*/ uint32 frequency;
    /*0x28*/ uint32 ReplayStart;
    /*0x2C*/ int32 unk[3];
    /*0x38*/ sphere boundSphere;
    /*0x58*/ int32 d5;
    /*0x5C*/ int32 d6;
};

// Size = 4 byte / 0x04 byte
// Incomplete
struct AnimationIndex
{
    /*0x00*/ uint16 aind; //anim ind in seq data
    /*0x02*/ uint16 sdind; //seq data array index
};

// Size = 140 byte / 0x8C byte
// Incomplete
struct STC
{
    /*0x00*/ Reference name;
    /*0x08*/ uint32 d1;
    /*0x0C*/ uint16 indSEQ[2]; //points to animation in SEQS chunk, twice for some reason
    /*0x10*/ Reference animid; //list of unique uint32s used in chunks with animation. The index of these correspond with the data in the next reference.
    /*0x18*/ Reference animindex; //lookup table, connects animid with it's animation data, nEntries of AnimationIndex reference using U32_ id
    /*0x20*/ uint32 d2;
    /*0x24*/ Reference SeqData[13]; //SD3V - Trans, SD4Q - Rotation, SDR3 - Scale?, SDFG - Flags, SDMB - Bounding Boxes?
};

// Size = 24 byte / 0x18 byte
// Incomplete
struct STS
{
    /*0x00*/ Reference animid; // uint32
    /*0x08*/ int32 unk[3];
    /*0x14*/ int16 s1;
    /*0x16*/ int16 s2;
};

// Size = 16 byte / 0x10 byte
// Complete
struct STG
{
    /*0x00*/ Reference name;
    /*0x08*/ Reference stcID;
};

// Size = 28 byte / 0x1C byte
// Incomplete
struct BNDS
{
    /*0x00*/ Vec3D extents1[2];
    /*0x18*/ float radius1;
};

// Size = 64 byte / 0x40 byte
// Complete
struct IREF
{
    float matrix[4][4];
};



#endif