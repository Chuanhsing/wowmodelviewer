#include <wx/wfstream.h>

#include "modelexport.h"
#include "lwoheader.h"
#include "modelcanvas.h"

#include "CxImage/ximage.h"

//--==Common Functions/Declarations==--

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

inline unsigned short ByteSwap16 (unsigned short nValue) // 16bit
{
   return ((((nValue & 0xFF00)>> 8)) | ((nValue & 0xFF) << 8));
}

//---------------------------------------------
// Scene Writing Functions
//---------------------------------------------

// Writes a single Key for an envelope.
void WriteLWSceneEnvKey(ofstream &fs, unsigned int Chan, float value, float time, unsigned int spline = 0)
{
	fs << "  Key ";				// Announces the start of a Key
	fs << value;				// The Key's Value;
	fs << " " << time;			// Time, in seconds, a float. This can be negative, zero or positive. Keys are listed in the envelope in increasing time order.
	fs << " " << spline;		// The curve type, an integer: 0 - TCB, 1 - Hermite, 2 - 1D Bezier (obsolete, equivalent to Hermite), 3 - Linear, 4 - Stepped, 5 - 2D Bezier
	fs << " 0 0 0 0 0 0 \n";	// Curve Data 1-6, all 0s for now.
}

// Writes an entire channel with only 1 key.
// Use WriteLWSceneEnvArray for writing animations.
void WriteLWSceneEnvChannel(ofstream &fs, unsigned int ChanNum, float value, float time, unsigned int spline = 0)
{
	fs << "Channel " << ChanNum << "\n";	// Channel Number
	fs << "{ Envelope\n";
	fs << "  1\n";						// Number of Keys in this envelope.
	WriteLWSceneEnvKey(fs,ChanNum,value,time,spline);
	fs << "  Behaviors 1 1\n";			// Pre/Post Behaviors. Defaults to 1 - Constant.
	fs << "}\n";
}

// Used for writing the keyframes of an animation.
void WriteLWSceneEnvArray(ofstream &fs, unsigned int count, unsigned int ChanNum, float value[], float time[], unsigned int spline[])
{
	fs << "Channel " << ChanNum << "\n";
	fs << "{ Envelope\n";
	fs << "  1\n";
	for (int n=0;n<count;n++){
		unsigned int thisspline = 0;
		if (spline[n]) {
			thisspline = spline[n];
		}
		WriteLWSceneEnvKey(fs,ChanNum,value[n],time[n],thisspline);
	}

	fs << "  Behaviors 1 1\n";
	fs << "}\n";
}

// Writes the "Plugin" information for a scene object, light, camera &/or bone.
void WriteLWScenePlugin(ofstream &fs, wxString type, int PluginCount, wxString PluginName, wxString Data = "")
{
	fs << "Plugin " << type << " " <<PluginCount << " " << PluginName << "\n" <<Data<< "EndPlugin\n";
}

// Write a scene light, if it's a Model Light
void WriteLWSceneModelLight(ofstream &fs, Model *m, ModelLight *light, int &lcount, Vec3D LPos, int ParentNum = NULL)
{
	bool isParented = false;
	if (ParentNum!=NULL)
		isParented = true;

	fs << "AddLight 2" << wxString::Format("%07x",lcount) << "\n";
	wxString modelname = wxString(m->modelname).AfterLast('\\').BeforeLast('.').MakeLower();
	modelname[0] = toupper(modelname[0]);
	fs << "LightName " << modelname << " Light " << lcount+1 << "\n";
	fs << "ShowLight 1 -1 0.941176 0.376471 0.941176\n";	// Last 3 Numbers are Layout Color for the Light.
	fs << "LightMotion\n";
	fs << "NumChannels 9\n";
	// Position
	WriteLWSceneEnvChannel(fs,0,LPos.x,0);
	WriteLWSceneEnvChannel(fs,1,LPos.y,0);
	WriteLWSceneEnvChannel(fs,2,-LPos.z,0);
	// Rotation
	WriteLWSceneEnvChannel(fs,3,0,0);
	WriteLWSceneEnvChannel(fs,4,0,0);
	WriteLWSceneEnvChannel(fs,5,0,0);
	// Scale
	WriteLWSceneEnvChannel(fs,6,1,0);
	WriteLWSceneEnvChannel(fs,7,1,0);
	WriteLWSceneEnvChannel(fs,8,1,0);

	if (isParented)
		fs << "ParentItem 1" <<wxString::Format("%07x",ParentNum);

	// Light Color Reducer
	// Some lights have a color channel greater than 255. This reduces all the colors, but increases the intensity, which should keep it looking the way Blizzard intended.

	// Model lights have 2 light types: Ambient & Diffused. We'll use Diffused for our Lights.
	Vec3D diffC = light->diffColor.getValue(0,0);

	float Lcolor[] = {diffC.x , diffC.y, diffC.z};
	float Lintensity = light->diffIntensity.getValue(0,0);

	while ((Lcolor[0] > 1)||(Lcolor[1] > 1)||(Lcolor[2] > 1)) {
		Lcolor[0] = Lcolor[0] * 0.99;
		Lcolor[1] = Lcolor[1] * 0.99;
		Lcolor[2] = Lcolor[2] * 0.99;
		Lintensity = Lintensity / 0.99;
	}

	fs << "LightColor " << Lcolor[0] << " " << Lcolor[1] << " " << Lcolor[2] << "\n";
	fs << "LightIntensity " << Lintensity << "\n";

	unsigned int ltype = light->type;
	// Process Light type & output!
	switch (ltype) {
		// Omni Lights
		case 1:
		default:
			// Default to an Omni (Point) light.
			fs << "LightType 1\n";

			if (light->UseAttenuation.getValue(0,0) == 1) {
				// Use Inverse Distance for the default Light Falloff Type. Should better simulate WoW Lights, until I can write a WoW light plugin for Lightwave...
				float attenend = (light->AttenEnd.getValue(0,0)) * 10;
				fs << "LightFalloffType 2\nLightRange " << attenend << "\n";
			}else{
				// Default to these settings, which look pretty close...
				fs << "LightFalloffType 2\nLightRange 25\n";
			}
			fs << "ShadowType 1\nShadowColor 0 0 0\n";
			WriteLWScenePlugin(fs,"LightHandler",1,"PointLight");
	}
	fs << "\n";
	lcount++;
}

// Write a scene light, if it's a WMO Light
void WriteLWSceneWMOLight(ofstream &fs, WMO *m, WMOLight *light, int &lcount, Vec3D LPos)
{
	fs << "AddLight 2" << wxString::Format("%07x",lcount) << "\n";
	wxString modelname = wxString(m->name).AfterLast('\\').BeforeLast('.').MakeLower();
	modelname[0] = toupper(modelname[0]);
	fs << "LightName " << modelname << " Light " << lcount+1 << "\n";
	fs << "ShowLight 1 -1 0.941176 0.376471 0.941176\n";	// Last 3 Numbers are Layout Color for the Light.
	fs << "LightMotion\n";
	fs << "NumChannels 9\n";
	// Position
	WriteLWSceneEnvChannel(fs,0,LPos.x,0);
	WriteLWSceneEnvChannel(fs,1,LPos.y,0);
	WriteLWSceneEnvChannel(fs,2,-LPos.z,0);
	// Rotation
	WriteLWSceneEnvChannel(fs,3,0,0);
	WriteLWSceneEnvChannel(fs,4,0,0);
	WriteLWSceneEnvChannel(fs,5,0,0);
	// Scale
	WriteLWSceneEnvChannel(fs,6,1,0);
	WriteLWSceneEnvChannel(fs,7,1,0);
	WriteLWSceneEnvChannel(fs,8,1,0);

	// Light Color Reducer
	// Some lights have a color channel greater than 255. This reduces all the colors, but increases the intensity, which should keep it looking the way Blizzard intended.
	float Lcolor[] = {light->fcolor.x, light->fcolor.y, light->fcolor.z};
	float Lintensity = light->intensity;

	while ((Lcolor[0] > 1)||(Lcolor[1] > 1)||(Lcolor[2] > 1)) {
		Lcolor[0] = Lcolor[0] * 0.99;
		Lcolor[1] = Lcolor[1] * 0.99;
		Lcolor[2] = Lcolor[2] * 0.99;
		Lintensity = Lintensity / 0.99;
	}

	fs << "LightColor " << Lcolor[0] << " " << Lcolor[1] << " " << Lcolor[2] << "\n";
	fs << "LightIntensity " << Lintensity << "\n";

	unsigned int ltype = light->type;
	// Process Light type & output!
	switch (ltype) {
		// Omni Lights
		case 1:
		default:
			// Default to an Omni (Point) light.
			fs << "LightType 1\n";
			// If the light uses falloff...
			if (light->useatten == 1) {
				// Use Inverse Distance for the default Light Falloff Type. Should better simulate WoW Lights, until I can write a WoW light plugin for Lightwave...
				float attenend = light->attenEnd * 10;
				fs << "LightFalloffType 2\nLightRange " << attenend << "\n";
			}else{
				// Default to these settings, which look pretty close...
				fs << "LightFalloffType 1\nLightRange 12.5\n";
			}
			fs << "ShadowType 1\nShadowColor 0 0 0\n";
			WriteLWScenePlugin(fs,"LightHandler",1,"PointLight");
	}
	fs << "\n";
	lcount++;
}

// Writes a Null Object to the scene file.
void WriteLWSceneNull(ofstream &fs, wxString Name, Vec3D Pos, Vec3D Rot, float Scale, int &ItemNumber, int ParentNum = -1, wxString Label="")
{
	bool isLabeled = false;
	bool isParented = false;
	if (Label!="")
		isLabeled = true;
	if (ParentNum > -1)
		isParented = true;

	fs << "AddNullObject 1" << wxString::Format("%07x",ItemNumber) << " " << Name << "\nChangeObject 0\n";
	if (isLabeled)
		fs << "// " << Label << "\n";
	fs << "ShowObject 7 -1 0.376471 0.878431 0.941176 \nGroup 0\nObjectMotion\nNumChannels 9\n";
	// Position
	WriteLWSceneEnvChannel(fs,0,Pos.x,0);
	WriteLWSceneEnvChannel(fs,1,Pos.y,0);
	WriteLWSceneEnvChannel(fs,2,-Pos.z,0);
	// Rotation
	WriteLWSceneEnvChannel(fs,3,Rot.x,0);
	WriteLWSceneEnvChannel(fs,4,Rot.y,0);
	WriteLWSceneEnvChannel(fs,5,Rot.z,0);
	// Scale
	WriteLWSceneEnvChannel(fs,6,Scale,0);
	WriteLWSceneEnvChannel(fs,7,Scale,0);
	WriteLWSceneEnvChannel(fs,8,Scale,0);

	fs << "PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n";
	if (isParented)
		fs << "ParentItem 1" <<wxString::Format("%07x",ParentNum);
	fs << "IKInitialState 0\nSubPatchLevel 3 3\nShadowOptions 7\n";

	fs << "\n";
	ItemNumber++;
}

// Writes an Object file to a scene





//---------------------------------------------
// --==M2==--
//---------------------------------------------

/* LWO2

Links to helpful documents:
http://gpwiki.org/index.php/LWO
http://home.comcast.net/~erniew/lwsdk/docs/filefmts/lwo2.html


NOTE!!
I've done some research into the LWO2 format. I have a HUGE commented section about it down in the WMO function that details the layout, and some of the byte info too.
I'll update this function once I re-tune the WMO function.
		-Kjasi
*/
void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init)
{
	int i32;
	uint32 u32;
	float f32;
	uint16 u16;
	unsigned char ub;
	int off_t;
	unsigned short numVerts = 0;
	unsigned short numGroups = 0;
	
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.

	unsigned int fileLen = 0;

	// --
	// Formally, a LightWave object file is a single IFF FORM chunk of type LWO2. The first 4 bytes are the characters 'F', 'O', 'R', 'M', 
	// and this is followed by a 4-byte integer containing the chunk size (the size of the file minus 8) and the FORM type (the 
	// characters 'L', 'W', 'O', '2'). As with all numbers in LWO2 files, the chunk size is always written in big-endian (Motorola, 
	// network) byte order.
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);
	f.Write("LWO2", 4);
	fileLen += 4;
	// ================


	// --
	// The TAGS chunk contains an array of strings. Whenever something is identified by name in the file, the ID is often a 
	// 0-based index into the TAGS array. The only named element in this file is its single surface, named "Default".
	f.Write("TAGS", 4);

	uint32 tagsSize = 0;
	wxString TAGS;
	u32 = 0;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	for (unsigned short i=0; i<m->passes.size(); i++) {
		wxString matName(wxString::Format(_T("Material_%i"), i));
		matName.Append(_T('\0'));
		if (fmod((float)matName.length(), 2.0f) > 0)
			matName.Append(_T('\0'));
		f.Write(matName.data(), matName.length());
		tagsSize += matName.length();
	}
	off_t = -4-tagsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	fileLen += tagsSize;
	// ================


	// --
	// The layer header signals the start of a new layer. All geometry elements that appear in the file after this and before 
	// the next LAYR chunk belong to this layer. The layer header contains an index, a flags word, the pivot point of the layer, 
	// the layer's name, and the index of the parent layer. This is the first (and only) layer, so its index is 0 and the optional 
	// parent index is omitted. The bits in the flags word are also 0, and the layer hasn't been given a name.
	// The pivot point is the origin for rotations in this layer and is expressed in world coordinates. Pivots typically differ from 
	// (0, 0, 0) when layers and layer parenting are used to create an object hierarchy.
	f.Write("LAYR", 4);
	u32 = reverse_endian<uint32>(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;
	// ================


	// --
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the vertice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				if ((init == false)&&(m->vertices)) {
					vert.x = reverse_endian<float>(m->vertices[a].x);
					vert.y = reverse_endian<float>(m->vertices[a].y);
					vert.z = reverse_endian<float>(0-m->vertices[a].z);
				} else {
					vert.x = reverse_endian<float>(m->origVertices[a].pos.x);
					vert.y = reverse_endian<float>(m->origVertices[a].pos.y);
					vert.z = reverse_endian<float>(0-m->origVertices[a].pos.z);
				}
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
	fileLen += pointsSize;
	off_t = -4-pointsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


/*
	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write("BBOX", 4);
	u32 = reverse_endian<uint32>(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = reverse_endian<float>(m->header.ps.BoundingBox[0].x);
	vert.y = reverse_endian<float>(m->header.ps.BoundingBox[0].y);
	vert.z = reverse_endian<float>(m->header.ps.BoundingBox[0].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = reverse_endian<float>(m->header.ps.BoundingBox[1].x);
	vert.y = reverse_endian<float>(m->header.ps.BoundingBox[1].y);
	vert.z = reverse_endian<float>(m->header.ps.BoundingBox[1].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	// ================
*/

	// --
	uint32 vmapSize = 0;

	f.Write("VMAP", 4);
	u32 = reverse_endian<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("TXUV", 4);
	uint16 dimension = 2;
	dimension = ByteSwap16(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write("Texture", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	uint16 counter=0;
	int16 surfCounter=0;

	// u16, f32, f32
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				f32 = reverse_endian<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = reverse_endian<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				if (counter == 256)
					counter = 0;
				vmapSize += 10;
			}
		}
	}
	fileLen += vmapSize;

	off_t = -4-vmapSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


	// --
	// POLYGON CHUNK
	// The POLS chunk contains a list of polygons. A "polygon" in this context is anything that can be described using an 
	// ordered list of vertices. A POLS of type FACE contains ordinary polygons, but the POLS type can also be CURV, 
	// PTCH, MBAL or BONE, for example.
	//
	// The high 6 bits of the vertex count for each polygon are reserved for flags, which in effect limits the number of 
	// vertices per polygon to 1023. Don't forget to mask the high bits when reading the vertex count. The flags are 
	// currently only defined for CURVs.
	// 
	// The point indexes following the vertex count refer to the points defined in the most recent PNTS chunk. Each index 
	// can be a 2-byte or a 4-byte integer. If the high order (first) byte of the index is not 0xFF, the index is 2 bytes long. 
	// This allows values up to 65279 to be stored in 2 bytes. If the high order byte is 0xFF, the index is 4 bytes long and 
	// its value is in the low three bytes (index & 0x00FFFFFF). The maximum value for 4-byte indexes is 16,777,215 (224 - 1). 
	// Objects with more than 224 vertices can be stored using multiple pairs of PNTS and POLS chunks.
	// 
	// The cube has 6 square faces each defined by 4 vertices. LightWave polygons are single-sided by default 
	// (double-sidedness is a possible surface property). The vertices are listed in clockwise order as viewed from the 
	// visible side, starting with a convex vertex. (The normal is defined as the cross product of the first and last edges.)
	int32 polySize = 4 + (numVerts / 3) * sizeof(POLYCHUNK2);

	f.Write("POLS", 4);
	i32 = reverse_endian<int32>(polySize);
	f.Write(reinterpret_cast<char *>(&i32), 4);
	fileLen += 8; // an extra 4 bytes for chunk size
	f.Write("FACE", 4);

	counter = 0;
	POLYCHUNK2 tri;
	
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				u16 = 3;
				tri.numVerts = ByteSwap16(u16);
				tri.indice[0] = ByteSwap16(counter);
				counter++;
				tri.indice[2] = ByteSwap16(counter);
				counter++;
				tri.indice[1] = ByteSwap16(counter);
				counter++;
				f.Write(reinterpret_cast<char *>(&tri), sizeof(POLYCHUNK2));
			}
		}
	}
	fileLen += polySize;
	// ========


	// --
	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.
	surfCounter = 0;
	counter=0;

	int32 ptagSize = 4 + (numVerts / 3) * 4;
	f.Write("PTAG", 4);
	u32 = reverse_endian<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("SURF", 4);
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				u16 = ByteSwap16(surfCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				counter++;
			}
			surfCounter++;
		}
	}
	fileLen += ptagSize;
	// ================


	// --
	int32 vmadSize = 0;
	f.Write("VMAD", 4);
	u32 = reverse_endian<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("TXUV", 4);
	dimension = 2;
	dimension = ByteSwap16(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write("Texture", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmadSize += 14;

	counter = 0;
	// u16, u16, f32, f32
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				u16 = ByteSwap16((uint16)(counter/3));
				f.Write(reinterpret_cast<char *>(&u16), 2);
				f32 = reverse_endian<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = reverse_endian<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				vmadSize += 12;
			}
		}
	}
	fileLen += vmadSize;
	off_t = -4-vmadSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


	// --
	uint32 surfaceCounter = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			int clipSize = 0;
			f.Write("CLIP", 4);
			u32 = reverse_endian<uint32>(0);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			u32 = reverse_endian<uint32>(++surfaceCounter);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write("STIL", 4);
			clipSize += 8;

			wxString texName(fn, wxConvUTF8);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << _T("_") << p.tex << _T(".tga") << '\0';
			if (fmod((float)texName.length(), 2.0f) > 0)
				texName.Append(_T('\0'));

			u16 = ByteSwap16(texName.length());
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(texName.data(), texName.length());
			clipSize += (2+texName.length());

			// update the chunks length
			off_t = -4-clipSize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = reverse_endian<uint32>(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			// save texture to file
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);

			fileLen += clipSize;
		}
	}
	// ================


	// --
	wxString surfName;
	surfaceCounter = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			uint32 surfaceDefSize = 0;
			f.Write("SURF", 4);
			u32 = reverse_endian<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			// Surface name
			surfName = _T("Material_");
			surfName << surfaceCounter;
			surfaceCounter++;
			
			surfName.Append(_T('\0'));
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append(_T('\0'));

			surfName.Append(_T('\0')); // ""
			surfName.Append(_T('\0'));
			f.Write(surfName.data(), (int)surfName.length());
			
			surfaceDefSize += (uint32)surfName.length();

			// Surface Attributes
			// COLOUR, size 4, bytes 2
			f.Write("COLR", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			// value
			f32 = reverse_endian<float>(p.ocol.x);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f32 = reverse_endian<float>(p.ocol.y);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f32 = reverse_endian<float>(p.ocol.z);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			surfaceDefSize += 20;

			// LUMI
			f.Write("LUMI", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);


			surfaceDefSize += 12;

			// DIFF
			f.Write("DIFF", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;


			// SPEC
			f.Write("SPEC", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// REFL
			f.Write("REFL", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			ub = '\0';
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);

			surfaceDefSize += 12;

			// TRAN
			f.Write("TRAN", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			ub = '\0';
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);

			surfaceDefSize += 12;

			// GLOSSINESS
			f.Write("GLOS", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			// Value
			// try 50% gloss for 'relfection surfaces
			if (p.useEnvMap)
				f32 = 128.0;
			else
				f32 = 0.0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			surfaceDefSize += 12;

			// RFOP
			f.Write("RFOP", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// TROP
			f.Write("TROP", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// SIDE
			f.Write("SIDE", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			
			// --
			// BLOK
			uint16 blokSize = 0;
			f.Write("BLOK", 4);
			f.Write(reinterpret_cast<char *>(&blokSize), 2);
			surfaceDefSize += 6;

			// IMAP
			f.Write("IMAP", 4);
			u16 = 50-8; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0x80;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CHAN
			f.Write("CHAN", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write("COLR", 4);
			blokSize += 10;

			// OPAC
			f.Write("OPAC", 4);
			u16 = 8; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1.0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 14;

			// ENAB
			f.Write("ENAB", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// NEGA
			f.Write("NEGA", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;
/*
			// AXIS
			f.Write("AXIS", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;
*/
			// TMAP
			f.Write("TMAP", 4);
			u16 = 98; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 6;

			// CNTR
			f.Write("CNTR", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0.0;
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 20;

			// SIZE
			f.Write("SIZE", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1.0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 20;

			// ROTA
			f.Write("ROTA", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0.0;
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 20;

			// FALL
			f.Write("FALL", 4);
			u16 = 16; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0.0;
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 22;

			// OREF
			f.Write("OREF", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CSYS
			f.Write("CSYS", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// end TMAP

			// PROJ
			f.Write("PROJ", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 5;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// AXIS
			f.Write("AXIS", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// IMAG
			f.Write("IMAG", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = surfaceCounter;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// WRAP
			f.Write("WRAP", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 10;

			// WRPW
			f.Write("WRPW", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 12;

			// WRPH
			f.Write("WRPH", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 12;

			// VMAP
			f.Write("VMAP", 4);
			u16 = 8; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			wxString t = _T("Texture");
			t.Append(_T('\0'));
			f.Write(t.data(), t.length());
			blokSize += 14;

			// AAST
			f.Write("AAST", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			blokSize += 12;

			// PIXB
			f.Write("PIXB", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// 
			surfaceDefSize += blokSize;
			off_t = -2-blokSize;
			f.SeekO(off_t, wxFromCurrent);
			u16 = ByteSwap16(blokSize);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.SeekO(0, wxFromEnd);
			// ================
			

			fileLen += surfaceDefSize;
			off_t = -4-surfaceDefSize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = reverse_endian<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);
		}
	}
	// ================


	f.SeekO(4, wxFromStart);
	u32 = reverse_endian<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();
}


//---------------------------------------------
// --==WMOs==--
//---------------------------------------------

/*
	This will export Lights & Doodads (as nulls) into a Lightwave Scene file.
*/
void ExportWMOObjectstoLWO(WMO *m, const char *fn)
{
	if ((m->nLights == 0)&&(m->nDoodads == 0)) {
		return;
	}

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast('.');
	SceneName << ".lws";
	ofstream fs(SceneName.mb_str(), ios_base::out | ios_base::trunc);

	if (!fs.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export the scene."), fn);
		return;
	}
	SceneName = SceneName.AfterLast('\\');

	/*
		Lightwave Scene files are simple text files. New Lines (\n) are need to add a new variable for the scene to understand.
		Lightwave files are pretty sturdy. Variables not already in a scene or model file, will usually be generated when opened.
		As such, we don't need to declare EVERY variable for the scene file, but I'm gonna add what I think is pertinent.
	*/

	// File Top
	fs << "LWSC\n";
	fs << "5\n\n"; // I think this is a version-compatibility number...

	// Scene File Basics
	fs << "RenderRangeType 0\n";
	fs << "FirstFrame 1\n";
	fs << "LastFrame 60\n";
	fs << "FrameStep 1\n";
	fs << "RenderRangeObject 0\n";
	fs << "RenderRangeArbitrary 1-60\n";
	fs << "PreviewFirstFrame 0\n";
	fs << "PreviewLastFrame 60\n";
	fs << "PreviewFrameStep 1\n";
	fs << "CurrentFrame 0\n";
	fs << "FramesPerSecond 30\n";
	fs << "ChangeScene 0\n\n";

	int mcount = 0;	// Model Count
	int lcount = 0; // Light Count

	int DoodadLightArrayID[1000];
	int DoodadLightArrayDDID[1000];
	int DDLArrCount = 0;

	Vec3D ZeroPos(0,0,0);
	Vec3D ZeroRot(0,0,0);

	// Objects/Doodads go here
	// TODO: Add the Exported Object First...
	for (int ds=0;ds<m->nDoodadSets;ds++){
		WMODoodadSet *DDSet = &m->doodadsets[ds];
		wxString DDSetName;
		DDSetName << wxString(m->name).AfterLast('\\').BeforeLast('.') << " DoodadSet " << wxString(DDSet->name);
		int DDSID = mcount;

		WriteLWSceneNull(fs,DDSetName,ZeroPos,ZeroRot,1,mcount);

		for (int dd=DDSet->start;dd<(DDSet->start+DDSet->size);dd++){
			WMOModelInstance *doodad = &m->modelis[dd];
			wxString name = wxString(doodad->filename).AfterLast('\\').BeforeLast('.');
			// Heading, Pitch & Bank.
			// Pitch & Bank are 0s so the item is sitting flat on the ground. Not sure how to convert WoW to Lightwave's rotations yet...
			Vec3D rot(doodad->dir.z,0,0);

			int DDID = mcount;
			WriteLWSceneNull(fs,name,doodad->pos,rot,doodad->sc,mcount,DDSID,doodad->filename);

			// Doodad Lights
			// Apparently, Doodad Lights must be parented to the Doodad for proper placement.
			if ((doodad->model) && (doodad->model->header.nLights > 0)){
				DoodadLightArrayDDID[DDLArrCount] = DDID;
				DoodadLightArrayID[DDLArrCount] = dd;
				DDLArrCount++;
			}
		}
	}

	// Lights
	// Lighting Basics
	fs << "AmbientColor 1 1 1\nAmbientIntensity 0.25\nDoubleSidedAreaLights 1\n\n";

	// Doodad Lights
	for (int i=0;i<DDLArrCount;i++){
		
		WMOModelInstance *doodad = &m->modelis[DoodadLightArrayID[i]];
		ModelLight *light = &doodad->model->lights[i];

		WriteLWSceneModelLight(fs,doodad->model,light,lcount,light->pos,DoodadLightArrayDDID[i]);
	}

	// WMO Lights
	for (int i=0;i<m->nLights;i++){
		WMOLight *light = &m->lights[i];
		WriteLWSceneWMOLight(fs,m,light,lcount,light->pos);
	}

	// Camera data (if we want it) goes here.
	// Yes, we can export flying cameras to Lightwave!
	// Just gotta add them back into the listing...

	// Backdrop Settings
	// Add this if viewing a skybox, or using one as a background?

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << "RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n";

	fs.close();
}


void ExportWMOtoLWO(WMO *m, const char *fn)
{
	int i32;
	uint32 u32;
	float f32;
	uint16 u16;
	unsigned char ub;
	int off_t;
	unsigned short numVerts = 0;
	unsigned short numGroups = 0;
	
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	/* LWO Model Format, as layed out in offical LWO2 files.( I Hex Edited to find most of this information from files I made/saved in Lightwave. -Kjasi)
	FORM	// Format Declaration
	LWO2	// Declares this is the Lightwave Object 2 file format. LWOB is the first format. Doesn't have a lot of the cool stuff LWO2 has...

	TAGS	// Used for various Strings
		Sketch Color Names
		Part Names
		Surface Names
	LAYR		// Specifies the start of a new layer. Probably best to only be on one...
		PNTS		// Points listing & Block Section
			BBOX		// Bounding Box. It's optional, but will probably help.
			VMPA		// Vertex Map Parameters, Always Preceeds a VMAP. 4bytes: Size (2 * 4 bytes).
						// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
						// Sketch Color: 0-12; 6-Default Gray
				VMAP		// Vector Map Section. Always Preceeds the following:
					SPOT	// Aboslute Morph Maps. Used only while modeling. Ignore.
					TXUV	// Defines UV Vector Map. Best not to use these unless the data has no Discontinuous UVs.
					PICK	// Point Selection Sets (2 bytes, then Set Name, then data. (Don't know what kind)
					MORF	// Relative Morph Maps. These are used for non-boned mesh animation.
					RGB		// Point Color Map, no Alpha. Note the space at end of the group!
					RGBA	// Same as above, but with an alpha channel.
					WGHT	// Weight Map. Used to give bones limited areas to effect, or can be used for point-by-point maps for various surfacing tricks.

		POLS		// Declares Polygon section. Next 4 bytes = Number of Polys
			FACE		// The actual Polygons. The maximum number of vertices is 1023 per poly!
				PTAG		// The Poly Tags for this Poly. These usually reference items in the TAGS group.
					COLR	// The Sketch Color Name
					PART	// The Part Name
					SURF	// The Surface Name
				VMPA		// Discontinuous Vertex Map Parameters
					VMAD		// Discontinuous Vector Map Section. Best if used only for UV Maps. Difference between VMAP & VMAD: VMAPs are connected to points, while VMADs are connected to Polys.
						APSL	// Adaptive Pixel Subdivision Level. Only needed for sub-patched models, so just ignore it for our outputs.
						TXUV	// Defines UV Vector Map
			PTCH	// Cat-mull Clarke Patches. Don't need this, but it mirror's FACE's sub-chunks.
			SUBD	// Subdivision Patches. Same as above.
			MBAL	// Metaballs. Don't bother...
			BONE	// Line segments representing the object's skeleton. These are converted to bones for deformation during rendering.

	CLIP (for each Image)
		STIL	// 2 bytes, size of string, followed by image name.extention
		FLAG	// Flags. 2 bytes, size of chunk. Not sure what the flag values are.
		AMOD	// 2 bytes: What's changed, 2 bytes: value. 2-Alphas: 0-Enabled, 1-Disabled, 2-Alpha Only. AMOD is omitted if value is 0.
	XREF	// Calls an instance of a CLIP. We'll avoid this for now.

	SURF	// Starts the surface's data. Not sure about the 4 bytes after it...
		// Until BLOK, this just sets the default values
		COLR	// Color
		LUMI	// Luminosity
		DIFF	// Diffusion
		SPEC	// Specularity
		REFL	// Reflections
		TRAN	// Transparancy
		TRNL	// Translucency
		RIND	// Refractive Index
		BUMP	// Bump Amount
		GLOS	// Glossiness
		GVAL	// Glow
		SHRP	// Diffuse Sharpness

		SMAN	// Smoothing Amount

		RFOP	// Reflection Options: 0-Backdrop Only (default), 1-Raytracing + Backdrop, 2 - Spherical Map, 3 - Raytracing + Spherical Map
		TROP	// Same as FROP, but for Refraction.
		SIDE	// Is it Double-Sided?
		NVSK	// Exclude from VStack

		CMNT // Surface Comment, but I don't seem to be able to get it to show up in LW... 2bytes: Size. Simple Text line for this surface. Make sure it doesn't end on an odd byte!
		VERS // Version Compatibility mode, including what it's compatible with. 2bytes (int16, value 4), 4bytes (int32, value is 850 or 931, depending on if you're compatible with LW8.5 or LW9.3.1)

		BLOK	// First Blok. Bloks hold Surface texture information!
			IMAP	// Declares that this surface texture is an image map.
				CHAN COLR	// Declares that the image map will be applied to the color channel. (Color has a Texture!)
					OPAC	// Opacity of Layer
					ENAB	// Is the layer enabled?
					NEGA	// Is it inverted?
					TMAP	// Texture Map details
						CNTR	// Position
						SIZE	// Scale
						ROTA	// Rotation
						FALL	// Falloff
						OREF	// Object Reference
						CSYS	// Coordinate System: 0-Object's Coordinates, 1-World's Coordinates

						// Image Maps
						PROJ	// Image Projection Mode: 0-Planar (references AXIS), 1-Cylindrical, 2-Spherical, 3-Cubic, 4-Front Projection, 5-UV (IDed in VMAP chunk)
						AXIS	// The axis the image uses: 0-X, 1-Y, or 2-Z;
						IMAG	// The image to use: Use CLIP Index
						WRAP	// Wrapping Mode: 0-Reset, 1-Repeat, 2-Mirror, 3-Edge
						WRPW	// Wrap Count Width (Used for Cylindrical & Spherical projections)
						WRPH	// Wrap Count Height
						VMAP	// Name of the UV Map to use, should PROJ be set to 5!
						AAST	// Antialiasing Strength
						PIXB	// Pixel Blending

			// Node Information
			// We can probably skip this for now. Later, it would be cool to mess with it, but for now, it'll be automatically generated once the file is opened in LW.

			NODS	// Node Block & Size
				NROT
				NLOC
				NZOM
				NSTA	// Activate Nodes
				NVER
				NNDS
				NSRV
					Surface
				NTAG
				NRNM
					Surface
				NNME
					Surface
				NCRD
				NMOD
				NDTA
				NPRW
				NCOM
				NCON

	*/


	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.

	unsigned int fileLen = 0;

	// --
	// Formally, a LightWave object file is a single IFF FORM chunk of type LWO2. The first 4 bytes are the characters 'F', 'O', 'R', 'M', 
	// and this is followed by a 4-byte integer containing the chunk size (the size of the file minus 8) and the FORM type (the 
	// characters 'L', 'W', 'O', '2'). As with all numbers in LWO2 files, the chunk size is always written in big-endian (Motorola, 
	// network) byte order.
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);
	f.Write("LWO2", 4);
	fileLen += 4;
	// ================


	// --
	// The TAGS chunk contains an array of strings. Whenever something is identified by name in the file, the ID is often a 
	// 0-based index into the TAGS array. The only named element in this file is its single surface, named "Default".
	f.Write("TAGS", 4);

	uint16 counter=0;
	int16 TagCounter=0;
	int16 PartCounter=0;
	int16 SurfCounter=0;

	// Declare the Texture Name Array
	// Made it fairly large, just for the really big models.
	// Find a Match for mat->tex and place it into the Texture Name Array.
	wxString texarray[200];
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];
			wxString outname(fn, wxConvUTF8);

			bool nomatch = true;
			for (int t=0;t<=m->nTextures; t++) {
				if (t == mat->tex) {
					texarray[mat->tex] = m->textures[t-1];
					texarray[mat->tex] = texarray[mat->tex].BeforeLast('.');
					nomatch = false;
					break;
				}
			}
			if (nomatch == true){
				texarray[mat->tex] = outname << wxString::Format(_T("_Material_%03i"), mat->tex);
			}
		}
	}

	uint32 tagsSize = 0;
	u32 = 0;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			wxString matName = texarray[mat->tex];
			matName.Append(_T('\0'));
			if (fmod((float)matName.length(), 2.0f) > 0)
				matName.Append(_T('\0'));
			f.Write(matName.data(), matName.length());
			tagsSize += matName.length();
		}
	}

	// Part Names
	for (int i=0; i<m->nGroups; i++) {
		wxString partName = m->groups[i].name;

		partName.Append(_T('\0'));
		if (fmod((float)partName.length(), 2.0f) > 0)
			partName.Append(_T('\0'));
		f.Write(partName.data(), partName.length());
		tagsSize += partName.length();
	}

	off_t = -4-tagsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	fileLen += tagsSize;
	// ================


	// --
	// The layer header signals the start of a new layer. All geometry elements that appear in the file after this and before 
	// the next LAYR chunk belong to this layer. The layer header contains an index, a flags word, the pivot point of the layer, 
	// the layer's name, and the index of the parent layer. This is the first (and only) layer, so its index is 0 and the optional 
	// parent index is omitted. The bits in the flags word are also 0, and the layer hasn't been given a name.
	// The pivot point is the origin for rotations in this layer and is expressed in world coordinates. Pivots typically differ from 
	// (0, 0, 0) when layers and layer parenting are used to create an object hierarchy.
	f.Write("LAYR", 4);
	u32 = reverse_endian<uint32>(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;
	// ================


	// --
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the vertice data
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				Vec3D vert;
				vert.x = reverse_endian<float>(m->groups[i].vertices[a].x);
				vert.y = reverse_endian<float>(m->groups[i].vertices[a].z);
				vert.z = reverse_endian<float>(m->groups[i].vertices[a].y);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
	fileLen += pointsSize;
	off_t = -4-pointsSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


/*
	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write("BBOX", 4);
	u32 = reverse_endian<uint32>(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = reverse_endian<float>(m->header.ps.BoundingBox[0].x);
	vert.y = reverse_endian<float>(m->header.ps.BoundingBox[0].y);
	vert.z = reverse_endian<float>(m->header.ps.BoundingBox[0].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = reverse_endian<float>(m->header.ps.BoundingBox[1].x);
	vert.y = reverse_endian<float>(m->header.ps.BoundingBox[1].y);
	vert.z = reverse_endian<float>(m->header.ps.BoundingBox[1].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	// ================
*/

	// --
	uint32 vmapSize = 0;

	//Vertex Mapping
	f.Write("VMAP", 4);
	u32 = reverse_endian<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write("TXUV", 4);
	uint16 dimension = 2;
	dimension = ByteSwap16(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write("Texture", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	// u16, f32, f32
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				f32 = reverse_endian<float>(m->groups[i].texcoords[a].x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = reverse_endian<float>(1 - m->groups[i].texcoords[a].y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				if (counter == 256)
					counter = 0;
				vmapSize += 10;
			}
		}
	}
	fileLen += vmapSize;

	off_t = -4-vmapSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


	// --
	// POLYGON CHUNK
	// The POLS chunk contains a list of polygons. A "polygon" in this context is anything that can be described using an 
	// ordered list of vertices. A POLS of type FACE contains ordinary polygons, but the POLS type can also be CURV, 
	// PTCH, MBAL or BONE, for example.
	//
	// The high 6 bits of the vertex count for each polygon are reserved for flags, which in effect limits the number of 
	// vertices per polygon to 1023. Don't forget to mask the high bits when reading the vertex count. The flags are 
	// currently only defined for CURVs.
	// 
	// The point indexes following the vertex count refer to the points defined in the most recent PNTS chunk. Each index 
	// can be a 2-byte or a 4-byte integer. If the high order (first) byte of the index is not 0xFF, the index is 2 bytes long. 
	// This allows values up to 65279 to be stored in 2 bytes. If the high order byte is 0xFF, the index is 4 bytes long and 
	// its value is in the low three bytes (index & 0x00FFFFFF). The maximum value for 4-byte indexes is 16,777,215 (224 - 1). 
	// Objects with more than 224 vertices can be stored using multiple pairs of PNTS and POLS chunks.
	// 
	// The cube has 6 square faces each defined by 4 vertices. LightWave polygons are single-sided by default 
	// (double-sidedness is a possible surface property). The vertices are listed in clockwise order as viewed from the 
	// visible side, starting with a convex vertex. (The normal is defined as the cross product of the first and last edges.)
	int32 polySize = 4 + (numVerts / 3) * sizeof(POLYCHUNK2);

	f.Write("POLS", 4);
	i32 = reverse_endian<int32>(polySize);
	f.Write(reinterpret_cast<char *>(&i32), 4);
	fileLen += 8; // an extra 4 bytes for chunk size
	f.Write("FACE", 4);

	counter = 0;
	POLYCHUNK2 tri;
	
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];

			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				u16 = 3;
				tri.numVerts = ByteSwap16(u16);
				tri.indice[0] = ByteSwap16(counter);
				counter++;
				tri.indice[2] = ByteSwap16(counter);
				counter++;
				tri.indice[1] = ByteSwap16(counter);
				counter++;
				f.Write(reinterpret_cast<char *>(&tri), sizeof(POLYCHUNK2));
			}
		}
	}
	fileLen += polySize;
	// ========


	// --
	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.

	// NOTE: Every PTAG type needs a seperate PTAG call!

	TagCounter = 0;
	counter=0;
	int32 ptagSize;

	// Surface PolyTag
	f.Write("PTAG", 4);
	ptagSize = 4 + (numVerts / 3) * 4;		// PTAG Title (always 4) + (Number of Points / PolySides (always 3)) * Number of Bytes
	u32 = reverse_endian<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("SURF", 4);
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];

			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				u16 = ByteSwap16(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				counter++;
			}
			TagCounter++;
			SurfCounter++;
		}
	}
	fileLen += ptagSize;

	// Parts PolyTag
	f.Write("PTAG", 4);
	ptagSize = 4 + (numVerts / 3) * 4;		// PTAG Title (always 4) + (Number of Points / PolySides (always 3)) * Number of Bytes
	counter=0;
	u32 = reverse_endian<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write("PART", 4);
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];

			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				u16 = ByteSwap16(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				counter++;
			}
		}
		TagCounter++;
		PartCounter++;
	}
	fileLen += ptagSize;


	// ================


	// --
	// Discontinuous Vertex Mapping
	int32 vmadSize = 0;
	f.Write("VMAD", 4);
	u32 = reverse_endian<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write("TXUV", 4);
	dimension = 2;
	dimension = ByteSwap16(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write("Texture", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmadSize += 14;

	counter = 0;
	// u16, u16, f32, f32
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];

			for (size_t k=0, b=0; k<batch->indexCount; k++,b++) {
				int a = m->groups[i].indices[batch->indexStart + k];
				u16 = ByteSwap16(counter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				u16 = ByteSwap16((uint16)(counter/3));
				f.Write(reinterpret_cast<char *>(&u16), 2);
				f32 = reverse_endian<float>(m->groups[i].texcoords[a].x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = reverse_endian<float>(1 - m->groups[i].texcoords[a].y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				vmadSize += 12;
			}
		}
	}
	fileLen += vmadSize;
	off_t = -4-vmadSize;
	f.SeekO(off_t, wxFromCurrent);
	u32 = reverse_endian<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================


	// --
	uint32 surfaceCounter = PartCounter;
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			int clipSize = 0;
			f.Write("CLIP", 4);
			u32 = reverse_endian<uint32>(0);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			u32 = reverse_endian<uint32>(++surfaceCounter);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write("STIL", 4);
			clipSize += 8;

			//wxString texName(fn, wxConvUTF8);
			wxString texName = texarray[mat->tex];

			// Remove all paths from our name.
			// Hopefully we can be able to export paths in the future...
			texName = texName.AfterLast('\\');

			//texName << _T("_") << wxString::Format(_T("%03i"),mat->tex) << _T(".tga") << '\0';
			texName << _T(".tga") << '\0';

			if (fmod((float)texName.length(), 2.0f) > 0)
				texName.Append(_T('\0'));

			u16 = ByteSwap16(texName.length());
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(texName.data(), texName.length());
			clipSize += (2+texName.length());

			// update the chunks length
			off_t = -4-clipSize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = reverse_endian<uint32>(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			// save texture to file
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;

			// setup texture
			glBindTexture(GL_TEXTURE_2D, mat->tex);
			SaveTexture(texFilename);

			fileLen += clipSize;
		}
	}
	// ================


	// --
	wxString surfName;
	surfaceCounter = PartCounter;
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			uint32 surfaceDefSize = 0;
			f.Write("SURF", 4);
			u32 = reverse_endian<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			// Surface name
			//surfName = wxString::Format(_T("Material_%03i"),mat->tex);
			surfName = texarray[mat->tex];
			surfaceCounter++;
			
			surfName.Append(_T('\0'));
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append(_T('\0'));

			surfName.Append(_T('\0')); // ""
			surfName.Append(_T('\0'));
			f.Write(surfName.data(), (int)surfName.length());
			
			surfaceDefSize += (uint32)surfName.length();

			// Surface Attributes
			// COLOUR, size 4, bytes 2
			f.Write("COLR", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			// value
			f32 = reverse_endian<float>(1.0);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f32 = reverse_endian<float>(1.0);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f32 = reverse_endian<float>(1.0);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			surfaceDefSize += 20;

			// LUMI
			f.Write("LUMI", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);


			surfaceDefSize += 12;

			// DIFF
			f.Write("DIFF", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;


			// SPEC
			f.Write("SPEC", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// REFL
			f.Write("REFL", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			ub = '\0';
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);

			surfaceDefSize += 12;

			// TRAN
			f.Write("TRAN", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			ub = '\0';
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			f.Write(reinterpret_cast<char *>(&ub), 1);

			surfaceDefSize += 12;

			// GLOSSINESS
			f.Write("GLOS", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			// Value
			// Set to 20%, because that seems right.
			f32 = 0.2;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			surfaceDefSize += 12;

			// RFOP
			f.Write("RFOP", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// TROP
			f.Write("TROP", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// SIDE
			f.Write("SIDE", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			// If double-sided...
			if (mat->flags & WMO_MATERIAL_CULL){
				u16 = 3;
			}
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// SMAN (Smoothing)
			f.Write("SMAN", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			// Smoothing is done in radiens. PI = 180 degree smoothing.
			f32 = 3.141592653589793238462643383279502884197169399375105820974944592307816;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);

			surfaceDefSize += 10;

			// --
			// BLOK
			uint16 blokSize = 0;
			f.Write("BLOK", 4);
			f.Write(reinterpret_cast<char *>(&blokSize), 2);
			surfaceDefSize += 6;

			// IMAP
			f.Write("IMAP", 4);
			u16 = 50-8; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0x80;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CHAN
			f.Write("CHAN", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write("COLR", 4);
			blokSize += 10;

			// OPAC
			f.Write("OPAC", 4);
			u16 = 8; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1.0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 14;

			// ENAB
			f.Write("ENAB", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// NEGA
			f.Write("NEGA", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;
/*
			// AXIS
			f.Write("AXIS", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;
*/
			// TMAP
			f.Write("TMAP", 4);
			u16 = 98; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 6;

			// CNTR
			f.Write("CNTR", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0.0;
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 20;

			// SIZE
			f.Write("SIZE", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1.0;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 20;

			// ROTA
			f.Write("ROTA", 4);
			u16 = 14; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0.0;
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 20;

			// FALL
			f.Write("FALL", 4);
			u16 = 16; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 0.0;
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 22;

			// OREF
			f.Write("OREF", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CSYS
			f.Write("CSYS", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// end TMAP

			// PROJ
			f.Write("PROJ", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 5;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// AXIS
			f.Write("AXIS", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// IMAG
			f.Write("IMAG", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = surfaceCounter;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// WRAP
			f.Write("WRAP", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 10;

			// WRPW
			f.Write("WRPW", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 12;

			// WRPH
			f.Write("WRPH", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 12;

			// VMAP
			f.Write("VMAP", 4);
			u16 = 8; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			wxString t = _T("Texture");
			t.Append(_T('\0'));
			f.Write(t.data(), t.length());
			blokSize += 14;

			// AAST
			f.Write("AAST", 4);
			u16 = 6; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 1;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			blokSize += 12;

			// PIXB
			f.Write("PIXB", 4);
			u16 = 2; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CMNT
			f.Write("CMNT", 4);
			wxString cmnt = texarray[mat->tex];
			cmnt.Append(_T('\0'));
			if (fmod((float)cmnt.length(), 2.0f) > 0)
				cmnt.Append(_T('\0'));
			u16 = cmnt.length(); // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(cmnt.data(), cmnt.length());
			blokSize += 6 + cmnt.length();

			// 
			surfaceDefSize += blokSize;
			off_t = -2-blokSize;
			f.SeekO(off_t, wxFromCurrent);
			u16 = ByteSwap16(blokSize);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.SeekO(0, wxFromEnd);
			// ================
			

			fileLen += surfaceDefSize;
			off_t = -4-surfaceDefSize;
			f.SeekO(off_t, wxFromCurrent);
			u32 = reverse_endian<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);
		}
	}
	// ================


	f.SeekO(4, wxFromStart);
	u32 = reverse_endian<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();

	ExportWMOObjectstoLWO(m,fn);
}