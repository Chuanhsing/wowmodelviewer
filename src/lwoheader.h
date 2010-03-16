#ifndef LWOHEADER_H
#define LWOHEADER_H

#define MAX_POINTS_PER_POLYGON 1023

// -----------------------------------------
// New LW Header Stuff
// -----------------------------------------

int i32;
uint32 u32;
float f32;
uint16 u16;
unsigned char ub;

#if defined _MSWIN || defined _WINDOWS
	#define MSB2			_SwapTwoBytes
	#define MSB4			_SwapFourBytes
	#define LSB2(w)			(w)
	#define LSB4(w)			(w)
#else
	#define MSB2(w)			(w)
	#define MSB4(w)			(w)
	#define LSB2			_SwapTwoBytes
	#define LSB4			_SwapFourBytes 
#endif

unsigned short _SwapTwoBytes (unsigned short w)
{
	unsigned short tmp;
	tmp =  (w & 0x00ff);
	tmp = ((w & 0xff00) >> 0x08) | (tmp << 0x08);
	return tmp;
}

template <typename T>
inline T _SwapFourBytes (T w)
{
	T a;
	unsigned char *src = (unsigned char*)&w;
	unsigned char *dst = (unsigned char*)&a;

	dst[0] = src[3];
	dst[1] = src[2];
	dst[2] = src[1];
	dst[3] = src[0];

	return a;
}

struct PolyChunk16 {
	uint16 numVerts;
	uint16 indice[3];
};

// With this, we gotta mask out the un-needed indice bytes if it's less than 0xFF
struct PolyChunk32 {
	uint16 numVerts;
	uint32 indice[3];
};


// -=Usable Structures=-
// Weight Data
struct LWWeight{
	Vec3D Point;
	float Value;
};

// Layer Data
struct LWLayer {
	// Layer Data
	wxString Name;

	// Points Block
	unsigned long PointCount;
	Vec3D Points[1];
	Vec3D BoundingBox1;
	Vec3D BoundingBox2;
	Vec2D UVData[1];
	
	// Poly Block
	unsigned long PolyCount;
	PolyChunk32 Polys[1];
};

#endif // LWOHEADER_H