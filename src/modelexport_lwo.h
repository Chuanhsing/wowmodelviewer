#ifndef MODELEXPORT_LWO_H
#define MODELEXPORT_LWO_H

#define MAX_POINTS_PER_POLYGON 1023

// -----------------------------------------
// New LW Header Stuff
// -----------------------------------------

int i32;
uint32 u32;
float f32;
uint16 u16;
unsigned char ub;

enum LWItemType {
	LW_ITEMTYPE_OBJECT = 1,
	LW_ITEMTYPE_LIGHT,
	LW_ITEMTYPE_CAMERA,
	LW_ITEMTYPE_BONE,
};
enum LWLightType {
	LW_LIGHTTYPE_DISTANT = 0,
	LW_LIGHTTYPE_POINT,
	LW_LIGHTTYPE_SPOTLIGHT,
	LW_LIGHTTYPE_LINEAR,
	LW_LIGHTTYPE_AREA,

	LW_LIGHTTYPE_PLUGIN = 100,
};
enum LWBoneType {
	LW_BONETYPE_BONE = 0,
	LW_BONETYPE_JOINT,
};

// Writing Functions
void LW_WriteVX(wxFFileOutputStream &f, uint32 p, uint32 &Size); // Write Lightwave VX Data
void LW_WriteSurface(wxFFileOutputStream &f, wxString surfName, Vec4D Color, float reflect, bool cull, uint32 surfID, wxString comment, uint32 &fileSize);

// Low Polychunk
struct PolyChunk16 {
	uint16 numVerts;
	uint16 indice[3];
};

// High Polychunk. (Used if the indice is greater than 0xFF)
struct PolyChunk32 {
	uint16 numVerts;
	uint32 indice[3];
};

// Animation Data
struct AnimationData {
	std::vector<Vec3D> Position;
	std::vector<Vec3D> Rotation;
	std::vector<Vec3D> Scale;
	std::vector<uint32> Time;

	void Push(Vec3D position, Vec3D rotation, Vec3D scale, uint32 time){
		Position.push_back(position);
		Rotation.push_back(rotation);
		Scale.push_back(scale);
		Time.push_back(time);
	}
	uint32 Size(){
		return (uint32)Time.size();
	}
};

// --== Scene Formats ==--
struct LWBones{
	uint32 BoneID;
	AnimationData AnimData;
	wxString Name;
	uint8 BoneType;
	wxString ParentID;
	uint8 ParentType;
};

// Scene Object
struct LWSceneObj{
	uint32 ObjectID;
	AnimationData AnimData;
	wxString Name;
	std::vector<LWBones> Bones;
	bool isNull;
	wxString ParentID;
	uint8 ParentType;
};

struct LWLight{
	uint32 LightID;
	AnimationData AnimData;
	wxString Name;
	RGB_Color Color;
	uint8 LightType;
	float FalloffRadius;
	wxString ParentID;
	uint8 ParentType;
};

struct LWCamera{
	uint32 CameraID;
	AnimationData AnimData;
	wxString Name;
	float FieldOfView;
	wxString TargetObjectID;
	wxString ParentID;
	uint8 ParentType;
};

// Master Scene
struct LWScene{
	std::vector<LWSceneObj> Object;
	std::vector<LWLight> Light;
	std::vector<LWCamera> Camera;

	wxString FileName;
	wxString FilePath;
};

// --== Object Formats ==--
// Weight Data
struct LWWeight{
	uint16 WeightID;
	float Value;
};

// Vertex Color Data
struct LWVertexColor{
	uint8 r, g, b, a;

	LWVertexColor(uint8 R=255, uint8 G=255, uint8 B=255, uint8 A=255){
		r = R;
		g = G;
		b = B;
		a = A;
	}
};

// Point Chunk Data
struct LWPoint {
	Vec3D PointData;
	Vec2D UVData;
	std::vector<LWWeight> WeightData;
	LWVertexColor VertexColors;
};

// Poly Chunk Data
struct LWPoly {
	PolyChunk32 PolyData;
	uint32 PartTagID;
	uint32 SurfTagID;
};

// -= Lightwave Chunk Structures =-

// Layer Data
struct LWLayer {
	// Layer Data
	wxString Name;					// Name of the Layer, Optional
	int ParentLayer;				// 0-based number of parent layer. -1 or omitted for no parent.

	// Points Block
	std::vector<LWPoint>Points;		// Various Point Blocks used by this layer.
	bool HasVectorColors;			// Is True if the layer has a Vector Color map
	Vec3D BoundingBox1;				// First Corner of the Layer's Bounding Box
	Vec3D BoundingBox2;				// Second Corner of the Layer's Bounding Box
	// Point UV Data

	// Poly Block
	std::vector<LWPoly> Polys;
	// Poly UV Data

	LWLayer(){
		HasVectorColors = false;
	}

};

// Clip Data
struct LWClip {
	wxString Filename;	// The Path & Filename of the image to be used in Lightwave
	wxString Source;	// The Source Path & Filename, as used in WoW.
	uint32 TagID;		// = Number of Parts + Texture number
};

// Surface Chunk Data
struct LWSurface {
	wxString Name;			// The Surface's Name
	wxString Comment;		// Comment for the surface.

	bool isDoubleSided;		// Should it show the same surface on both sides of a poly.

	// Images
	uint32 Image_ColorID;	//Tag ID for the Color Image
	uint32 Image_SpecID;	//Tag ID for the Specular Image
	uint32 Image_TransID;	//Tag ID for the Transparancy Image

	// Constructors
	LWSurface(wxString name, wxString comment, uint32 ColorID, uint32 SpecID=-1, uint32 TransID=-1, bool doublesided = false){
		Name = name;
		Comment = comment;

		Image_ColorID = ColorID;
		Image_SpecID = SpecID;
		Image_TransID = TransID;

		isDoubleSided = doublesided;
	}
};

// The Master Structure for each individual LWO file.
struct LWObject {
	wxArrayString PartNames;	// List of names for all the Parts;
	std::vector<LWLayer> Layers;		// List of Layers (usually 1) that make up the Geometery.
	std::vector<LWClip> Images;			// List of all the Unique Images used in the model.
	std::vector<LWSurface> Surfaces;	// List of the Surfaces used in the model.

	wxString SourceType;			// M2, WMO or ADT
};

#endif