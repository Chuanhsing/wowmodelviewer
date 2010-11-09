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
struct LWSurface;

// Writing Functions
void LW_WriteVX(wxFFileOutputStream &f, uint32 p, uint32 &Size); // Write Lightwave VX Data
void LW_WriteSurface(wxFFileOutputStream &f, LWSurface Surface, uint32 &fileSize);

// Lightwave Type Numbers
// These numbers identify object, light and bone types in the Lightwave Program.
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

// Polygon Chunk
struct PolyChunk {
	uint16 numVerts;		// The Number of Indices that make up the poly.
	uint32 indice[3];		// The IDs of the 3 Indices that make up each poly. In reality, this should be indice[MAX_POINTS_PER_POLYGON], but WoW will never go above 3.
};

// Polygon Normal
struct PolyNormal {
	wxString NormalMapName;
	uint32 indice[3];
	uint32 polygon;
	Vec3D direction[3];		// This is the direction the polygon's normal should point towards.	
};

// Animation Data
struct AnimationData {
	std::vector<Vec3D> Position;
	std::vector<Vec3D> Rotation;
	std::vector<Vec3D> Scale;
	std::vector<uint32> Time;

	// Push keyframe data into the Animation Data structure.
	void Push(Vec3D position, Vec3D rotation, Vec3D scale, uint32 time){
		Position.push_back(position);
		Rotation.push_back(rotation);
		Scale.push_back(scale);
		Time.push_back(time);
	}
	// Returns the number of keyframes in this animation.
	size_t Size(){
		return Time.size();
	}
};

// --== Scene Formats ==--
struct LWBones{
	uint32 BoneID;
	AnimationData AnimData;
	wxString Name;
	uint8 BoneType;
	int32 ParentID;
	uint8 ParentType;
};

// Scene Object
struct LWSceneObj{
	uint32 ObjectID;
	int32 LayerID;
	AnimationData AnimData;
	wxString Name;
	std::vector<LWBones> Bones;
	bool isNull;
	int32 ParentID;
	int8 ParentType;

	LWSceneObj(wxString name, uint32 id, int32 parentID, int8 parentType = LW_ITEMTYPE_OBJECT, bool IsNull = false, int32 layerID = 1){
		Name = name;
		ObjectID = id;
		ParentType = parentType;
		ParentID = parentID;
		isNull = IsNull;
		LayerID = layerID;
	}
};

struct LWLight{
	uint32 LightID;
	AnimationData AnimData;
	wxString Name;
	Vec3D Color;
	float Intensity;
	uint8 LightType;
	float FalloffRadius;
	int32 ParentID;
	uint8 ParentType;
};

struct LWCamera{
	uint32 CameraID;
	AnimationData AnimData;
	wxString Name;
	float FieldOfView;
	int32 TargetObjectID;
	int32 ParentID;
	uint8 ParentType;
};

// Master Scene
struct LWScene{
	std::vector<LWSceneObj> Objects;
	std::vector<LWLight> Lights;
	std::vector<LWCamera> Cameras;

	wxString FileName;
	wxString FilePath;
	float AmbientIntensity;

	LWScene(wxString file, wxString path, float ambint = 0.5){
		FileName = file;
		FilePath = path;

		AmbientIntensity = ambint;
	}

	~LWScene(){
		Objects.clear();
		Lights.clear();
		Cameras.clear();
		FileName.Clear();
		FilePath.Clear();
		//free(&AmbientIntensity);
	}
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
	LWVertexColor VertexColors;		// In reality, this should be a std::vector, but WoW doesn't use more than 1.
};

// Poly Chunk Data
struct LWPoly {
	PolyChunk PolyData;
	PolyNormal Normals;
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

	// Poly Block
	std::vector<LWPoly> Polys;

	LWLayer(){
		Name = _T("(unnamed)");
		HasVectorColors = false;
		ParentLayer = -1;
	};
};

// Clip Data
struct LWClip {
	wxString Filename;		// The Path & Filename of the image to be used in Lightwave
	wxString Source;		// The Source Path & Filename, as used in WoW.
	uint32 TagID;			// = Number of Parts + Texture number
};

struct LWSurf_Image {
	uint32 ID;				// Tag ID for the Image
	float UVRate_U;			// Rate to move the U with UV Animation
	float UVRate_V;			// Rate to move the V with UV Animation

	// Contructor
	LWSurf_Image(uint32 idtag=-1, float UVAnimRate_U=0.0f, float UVAnimRate_V=0.0f){
		ID = idtag;
		UVRate_U = UVAnimRate_U;
		UVRate_V = UVAnimRate_V;
	}
};

// Surface Chunk Data
struct LWSurface {
	wxString Name;			// The Surface's Name
	wxString Comment;		// Comment for the surface.

	bool isDoubleSided;		// Should it show the same surface on both sides of a poly.
	bool hasVertColors;

	// Base Attributes
	Vec3D Surf_Color;			// Surface Color as floats
	float Surf_Diff;			// Diffusion Amount
	float Surf_Lum;				// Luminosity Amount
	float Surf_Spec;			// Specularity Amount
	float Surf_Reflect;			// Reflection Amount
	float Surf_Trans;			// Transparancy Amount

	// Images
	LWSurf_Image Image_Color;	// Color Image data
	LWSurf_Image Image_Spec;	// Specular Image data
	LWSurf_Image Image_Trans;	// Transparancy Image data

	// Constructors
	LWSurface(wxString name, wxString comment, LWSurf_Image ColorID, LWSurf_Image SpecID, LWSurf_Image TransID, Vec3D SurfColor=Vec3D(0.75,0.75,0.75), float Diffusion=1.0f, float Luminosity = 0.0f, bool doublesided = false, bool hasVC = false){
		Name = name;
		Comment = comment;

		Surf_Color = SurfColor;
		Surf_Diff = Diffusion;
		Surf_Lum = Luminosity;
		Surf_Spec = 0.0f;
		Surf_Reflect = 0.0f;
		Surf_Trans = 0.0f;

		Image_Color = ColorID;
		Image_Spec = SpecID;
		Image_Trans = TransID;

		isDoubleSided = doublesided;
		hasVertColors = hasVC;
	}
};

// The Master Structure for each individual LWO file.
struct LWObject {
	wxArrayString PartNames;			// List of names for all the Parts;
	std::vector<LWLayer> Layers;		// List of Layers (usually 1) that make up the Geometery.
	std::vector<LWClip> Images;			// List of all the Unique Images used in the model.
	std::vector<LWSurface> Surfaces;	// List of the Surfaces used in the model.

	wxString SourceType;				// M2, WMO or ADT

	LWObject(){
		SourceType = wxEmptyString;
	}

	void Plus(LWObject o, int LayerNum=0,wxString PartNamePrefix = _T("")){
		//wxLogMessage(_T("Running LW Plus Function, Num Layers: %i, into Layer %i."),o.Layers.size(),LayerNum);
		// Add layers if nessicary...
		while (Layers.size() < LayerNum+1){
			LWLayer a;
			Layers.push_back(a);
		}

		uint32 OldPartNum = (uint32)PartNames.size();
		uint32 OldSurfNum = (uint32)Surfaces.size();
		uint32 OldTagNum =  OldPartNum + OldSurfNum;
		std::vector<uint32> OldPointNum;

		// Parts
		for (size_t i=0;i<o.PartNames.size();i++){
			wxString a = PartNamePrefix + o.PartNames[i];
			PartNames.push_back(a);
		}
		// Surfaces
		for (size_t i=0;i<o.Surfaces.size();i++){
			LWSurface s = o.Surfaces[i];
			s.Image_Color.ID += OldTagNum;
			Surfaces.push_back(s);
		}
		//Images
		for (size_t i=0;i<o.Images.size();i++){
			LWClip a = o.Images[i];
			a.TagID += OldTagNum;
			Images.push_back(a);
		}

		// Parts Difference
		uint32 PartDiff = (uint32)PartNames.size() - OldPartNum;

		// --== Layers ==--
		// Process Old Layers
		for (size_t i=0;i<Layers.size();i++){
			OldPointNum.push_back((uint32)Layers[i].Points.size());
			for (size_t x=0;x<Layers[i].Polys.size();x++){
				Layers[i].Polys[x].SurfTagID += PartDiff;
			}
		}
		// Proccess New Layers
		for (size_t i=0;i<o.Layers.size();i++){
			LWLayer a = o.Layers[i];

			// Vector Colors
			if (a.HasVectorColors == true)
				Layers[LayerNum].HasVectorColors = true;

			// Bounding Box
			if (a.BoundingBox1.x > Layers[LayerNum].BoundingBox1.x)
				Layers[LayerNum].BoundingBox1.x = a.BoundingBox1.x;
			if (a.BoundingBox1.y > Layers[LayerNum].BoundingBox1.y)
				Layers[LayerNum].BoundingBox1.y = a.BoundingBox1.y;
			if (a.BoundingBox1.z > Layers[LayerNum].BoundingBox1.z)
				Layers[LayerNum].BoundingBox1.z = a.BoundingBox1.z;
			if (a.BoundingBox2.x < Layers[LayerNum].BoundingBox2.x)
				Layers[LayerNum].BoundingBox2.x = a.BoundingBox2.x;
			if (a.BoundingBox2.y < Layers[LayerNum].BoundingBox2.y)
				Layers[LayerNum].BoundingBox2.y = a.BoundingBox2.y;
			if (a.BoundingBox2.z < Layers[LayerNum].BoundingBox2.z)
				Layers[LayerNum].BoundingBox2.z = a.BoundingBox2.z;

			// Points
			for (size_t x=0;x<a.Points.size();x++){
				Layers[LayerNum].Points.push_back(a.Points[x]);
			}
			// Polys
			for (size_t x=0;x<a.Polys.size();x++){
				for (uint16 j=0;j<a.Polys[x].PolyData.numVerts;j++){
					a.Polys[x].PolyData.indice[j] += OldPointNum[LayerNum];
				}
				a.Polys[x].PartTagID += OldPartNum;
				a.Polys[x].SurfTagID += OldTagNum;
				Layers[LayerNum].Polys.push_back(a.Polys[x]);
			}
		}
	}
	LWObject operator= (LWObject o){
		PartNames = o.PartNames;
		Layers = o.Layers;
		Images = o.Images;
		Surfaces = o.Surfaces;

		SourceType = o.SourceType;

		// return LWObject?
		return o;
	}
	~LWObject(){
		PartNames.Clear();
		Layers.clear();
		Images.clear();
		Surfaces.clear();
		SourceType.Clear();
	}
};

// Gather Functions
LWObject GatherM2forLWO(Attachment *att, Model *m, bool init, const char *fn, LWScene &scene, bool announce = true);
LWObject GatherWMOforLWO(WMO *m, const char *fn, LWScene &scene);

#endif