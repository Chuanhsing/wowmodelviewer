#ifndef MODELEXPORT_M3_H
#define MODELEXPORT_M3_H

/* http://code.google.com/p/libm3/wiki/Structs */

// Size = 16 byte / 0x10 byte
struct QUAT
{
    float x, y, z, w;
};

// Size = 32 byte / 0x20 byte
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
	Reference name;
	uint32 type;
	Reference mSEQS;		// sequenceHeader
	Reference mSTC;			// sequenceData
	Reference mSTG;			// sequenceLookup
	Vec3D v3d1;
	uint32 d10;
	Reference mSTS;
	Reference mBone;
	uint32 nSkinnedBones;
	uint32 vertFlags;
	Reference mVert;		// vertexData
	Reference mDIV;			// views
	Reference mBoneLU;		// boneLookup
	sphere boundSphere;
	int d2[15];
	Reference mAttach;
	Reference mAttachLU;	// attachLookup
	Reference mLite;		// Lights
	Reference mSHBX;
	Reference mCam;			// Camera
	Reference D;
	Reference mMatLU;		// materialLookup
	Reference mMat;			// material
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

// Size = 160 byte / 0xA0 byte
// Incomplete
struct BONE
{
    /*0x00*/ int32 d1; // Keybone?
    /*0x04*/ Reference name; // bonename
    /*0x10*/ uint32 flags; //2560 = Weighted vertices rendered, 512 = not rendered
    /*0x14*/ int16 parent; // boneparent
    /*0x16*/ uint16 s1; // always 0
    /*0x1A*/ AnimRef transid; //unique animation ID ref
    /*0x20*/ Vec3D pos; //bone position is relative to parent bone and its rotation
    /*0x2C*/ Vec3D pos2;
    /*0x38*/ float f1;
    /*0x3C*/ AnimRef rotid;
    /*0x44*/ QUAT rot; //initial bone rotation
    /*0x54*/ QUAT rot2;
    /*0x64*/ float f2;
    /*0x68*/ AnimRef scaleid;
    /*0x70*/ Vec3D scale; //initial scale
    /*0x7C*/ Vec3D scale2;
    /*0x88*/ int32 d3;
    /*0x90*/ AnimRef unk2;
    /*0x94*/ int32 d4[3];
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

// Size = 52 byte / 0x34 byte
// Incomplete
struct DIV
{
    /*0x00*/ Reference faces;
    /*0x0C*/ Reference REGN;	// submesh
    /*0x18*/ Reference BAT;
    /*0x24*/ Reference MSEC;
    /*0x30*/ uint32 unk;
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

//Size = 26 byte / 0x24 byte
// Incomplete
struct REGN
{
    /*0x00*/ uint32 d1[2]; //always 0?
    /*0x08*/ uint32 indVert;
    /*0x0C*/ uint32 numVert;
    /*0x10*/ uint32 indFaces;
    /*0x14*/ uint32 numFaces;
    /*0x18*/ uint16 boneCount; //boneLookup total count (redundant?)
    /*0x1A*/ uint16 indBone; //indice into boneLookup
    /*0x1C*/ uint16 numBone; //number of bones used from boneLookup
    /*0x1E*/ uint16 s2; //flags? vital for sc2 engine rendering the geometry
    /*0x20*/ unsigned char b1[2];
    /*0x22*/ uint16 s3;
};

// Size = 72 byte / 0x48 byte
// Incomplete
struct MSEC
{
    /*0x00*/ uint32 d1; //always 0?
    /*0x04*/ AnimRef bounds; //Bounding box animation ref in STC
    /*0x0C*/ sphere ext1; //Some kind of mesh extent? TODO
    /*0x2C*/ uint32 d2[7];
};

// Size = 176 byte / 0xB0 byte
// Incomplete
struct CAM
{
    /*0x00*/ int32 d1;
    /*0x04*/ Reference name;
    /*0x10*/ uint16 flags1;
    /*0x12*/ uint16 flags2;
};

// Size = 388 byte / 0x184 byte
// Incomplete
struct PROJ
{
};

// Size = 100 byte/ 0x64 byte
// Incomplete
struct EVNT
{
    /*0x00*/ Reference name;
    /*0x0C*/ int16 unk1[4];
    /*0x14*/ float matrix[4][4];
    /*0x54*/ int32 unk2[4];
};

// Size = 20 byte / 0x14 byte
// Incomplete
struct ATT
{
    /*0x00*/ int32 flag;
    /*0x04*/ Reference name;
    /*0x10*/ int32 bone;
};

// Size = 32 byte / 0x20 byte
// Complete, Animationblock, SD3V, SD4Q
struct SD
{
    /*0x00*/ Reference timeline;
    /*0x0C*/ uint32 flags;
    /*0x10*/ uint32 length;
    /*0x14*/ Reference data;
};

// Size = 96 byte / 0x60 byte
// Incomplete
struct SEQS
{
    /*0x00*/ int32 d1;
    /*0x04*/ int32 d2;
    /*0x08*/ Reference name;
    /*0x14*/ int32 animStart;
    /*0x18*/ int32 length;			// animLength
    /*0x1C*/ float moveSpeed;
    /*0x20*/ uint32 flags;
    /*0x24*/ uint32 frequency;
    /*0x28*/ uint32 ReplayStart;
    /*0x2C*/ int32 d4[3];
    /*0x38*/ sphere boundSphere;
    /*0x58*/ int32 d5[2];
};

// Size = 4 byte / 0x04 byte
// Incomplete
struct AnimationIndex
{
    /*0x00*/ uint16 aind; //anim ind in seq data
    /*0x02*/ uint16 sdind; //seq data array index
};

// Size = 204 byte / 0xCC byte
// Incomplete
/*
These blocks works like some sort of headers for sequences, referencing Sequence Data 
blocks and some other stuff.

Seems there can be more than one STC block per sequence if there are several versions 
of an animation sequence. STG represents an STC lookup table for each SEQS entry. 
Example: Cover Full, Cover Shield

Bones, among other structures, reference the Sequence Data located in these structs for animation.
*/
struct STC
{
    /*0x00*/ Reference name;
    /*0x0C*/ uint32 d1;
    /*0x10*/ uint16 indSEQ[2]; //points to animation in SEQS chunk, twice for some reason
    /*0x14*/ Reference animid; //list of unique uint32s used in chunks with animation. The index of these correspond with the data in the next reference.
    /*0x20*/ Reference animindex; //lookup table, connects animid with it's animation data, nEntries of AnimationIndex reference using U32_ id
    /*0x2C*/ uint32 d2;
    /*0x30*/ Reference Events;
    /*0x3C*/ Reference r1;
    /*0x48*/ Reference Trans; // SD3V - Trans
    /*0x54*/ Reference Rot; // SD4Q - Rotation
    /*0x60*/ Reference r2;
    /*0x6C*/ Reference r3[6]; // SDR3 - Scale?, SDFG - Flags, SDMB - Bounding Boxes?
    /*0xB4*/ Reference Flags;
    /*0xC0*/ Reference bounds;
};

// Size = 28 byte / 0x1C byte
// Incomplete
struct STS
{
    /*0x00*/ Reference animid; // uint32
    /*0x0C*/ int32 unk[3];
    /*0x18*/ int16 s1;
    /*0x1A*/ int16 s2;
};

// Size = 24 byte / 0x18 byte
// Complete
/*
The number of STG blocks in a .m3 file equals the number of SEQS blocks, and tells 
where in the STC list the animations of the corresponding SEQS block starts.
*/
struct STG
{
    /*0x00*/ Reference name;
    /*0x0C*/ Reference stcID;
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

/*
In the .m3 files, the vertex data seems to be contained within a uint8 block.

MODL-> flags defines some vertex stuff:

0x20000 = has vertices
0x40000 = has 1 extra UV coords
0x80000 = has 2 extra UV coords
0x100000 = has 3 extra UV coords

Each UV texture coordinate must be divided by 2046.0 to get its true float value. In 3ds 
max, the Y-UV coord must be flipped (1 - UV.y) for textures to be displayed on the mesh 
properly.

Vertex Weighting‘ø

Each vertex boneIndex is not an index into the global bone entries found in MODL but 
rather a reference into the bonelookup. However, it's not just an index into the 
bonelookup entries either. In order to find the correct bone to weight the vertice 
to, the boneIndex value uses submesh information found in the REGN indBone value to 
grab the right bone in the bonelookup entries. So in order to calculate the correct 
boneIndex:

1. Find which REGN entry the vertex belongs to
2. Add the REGN.indBone to the vertex.boneIndex value
3. Grab the bonelookup value your new index points to
4. Get the bone the bonelookup value refers to
*/
struct Vertex32 // 32 byte
{
    Vec3D pos;
    char weBone[4]; // fltByte
    unsigned char weIndice[4]; //index in boneLookup of vertex submesh
    char normal[4];  // fltNormal, x, y, z, w (w is the scale)
    uint16 uv[2]; // uvShort
    char tangents[4];
};

struct Vertex36 // 36 byte
{
    Vec3D pos;
    char boneWeight[4];
    char boneIndex[4]; //index in boneLookup of vertex submesh
    char normal[4];  // x, y, z, w (w is the scale)
    uint16 uv1[2];
    uint16 uv2[2];
    char tangents[4];
};

struct Vertex40 // 40 byte
{
    Vec3D pos;
    char boneWeight[4];
    char boneIndex[4]; //index in boneLookup of vertex submesh
    char normal[4];  // x, y, z, w (w is the scale)
    uint16 uv1[2];
    uint16 uv2[2];
    uint16 uv3[2];
    char tangents[4];
};

struct Vertex44 // 44 byte
{
    Vec3D pos;
    char boneWeight[4];
    char boneIndex[4]; //index in boneLookup of vertex submesh
    char normal[4];  // x, y, z, w (w is the scale)
    uint16 uv1[2];
    uint16 uv2[2];
    uint16 uv3[2];
    uint16 uv4[2];
    char tangents[4];
};

#endif