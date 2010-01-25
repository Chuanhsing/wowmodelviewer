#ifndef LWOHEADER_H
#define LWOHEADER_H

#define MAX_POINTS_PER_POLYGON 1023

// -----------------------------------------
// New LW Header Stuff
//
// One I finish my work, this will replace the above file.
// -----------------------------------------

int i32;
uint32 u32;
float f32;
uint16 u16;
unsigned char ub;

template <typename T>
inline T reverse_endian(T n) // for 32bits
{
#ifdef _MSC_VER
	_asm
	{
		mov EAX, n;
		bswap EAX;
		mov n, EAX;
	}

	return n;
#else
	uint32 m = *reinterpret_cast<uint32 *>(&n);
	T temp = ((m & 0xFF000000) >> 24) | ((m & 0x00FF0000) >> 8) |
			 ((m & 0x0000FF00) << 8)  | ((m & 0x000000FF) << 24);
	return *reinterpret_cast<T *>(&temp);
#endif
}

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

unsigned int _SwapFourBytes (unsigned int w)
{
	unsigned int tmp;
	tmp =  (w & 0x000000ff);
	tmp = ((w & 0x0000ff00) >> 0x08) | (tmp << 0x08);
	tmp = ((w & 0x00ff0000) >> 0x10) | (tmp << 0x08);
	tmp = ((w & 0xff000000) >> 0x18) | (tmp << 0x08);
	return tmp;
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

/*
// Return a Vertex
inline char get_VX(unsigned int Value)
{
	unsigned short indice16;
	unsigned int indice32;
	unsigned short byte = 0;
	bool is16 = false;
	char rthis[256];

	uint16 counter16 = (Value & 0x0000FFFF);
	uint32 counter32 = Value + 0xFF000000;

	if (Value < 0xFF00){
		rthis = MSB2(counter16);
		byte = 2;
	}else{
		rthis = MSB4(counter32);
		byte = 4;
	}

	return reinterpret_cast<char *>rthis, byte;
}
*/


#endif // LWOHEADER_H