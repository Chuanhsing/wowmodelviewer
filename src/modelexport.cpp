
#include <wx/wfstream.h>
#include <math.h>

#include "modelexport.h"
#include "ms3dheader.h"
#include "3dsheader.h"
#include "modelcanvas.h"

#include "CxImage/ximage.h"

unsigned short numVerts = 0;
unsigned short numFaces = 0;
unsigned short numGroups = 0;

unsigned short vertIndex = 0;
unsigned short grpIndex = 0;

struct ModelData {
	Vertex3f vertex;
	float tu;
	float tv;
	Vertex3f normal;
	unsigned short groupIndex;
	char boneid;
};

struct GroupData {
	ModelRenderPass p;
	Model *m;
};

ModelData *verts = NULL;
GroupData *groups = NULL;

// 2 methods to go,  just export the entire m2 model.
// or use our "drawing" routine to export only whats being drawn.

// SaveTexture
// Used to save composite textures, such as a character's face & body.
void SaveTexture(wxString fn)
{
	unsigned char *pixels = NULL;

	GLint width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	pixels = new unsigned char[width * height * 4];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

	CxImage *newImage = new CxImage(0);
	newImage->CreateFromArray(pixels, width, height, 32, (width*4), true);


	if (fn.Last() == 'g')
		newImage->Save(fn.mb_str(), CXIMAGE_FORMAT_PNG);
	else
		newImage->Save(fn.mb_str(), CXIMAGE_FORMAT_TGA);

	newImage->Destroy();
	wxDELETE(newImage);
	wxDELETEA(pixels);
}

// SaveTexture2 Function
// Used to export images that are filenames. For composited images, such as a character's face & body texture, use SaveTexture.
// ExportID identifies the exporting function. This is used in the path-generating section.
// Suffixes currently supported: "tga" & "png". Defaults to tga if omitted by exporter.
void SaveTexture2(wxString file, wxString outdir, wxString ExportID, wxString suffix = "tga")
{
	// Check to see if we have all our data...
	if (file == _T(""))
		return;
	if (outdir == _T(""))
		return;

	wxFileName fn(file);
	if (fn.GetExt().Lower() != _T("blp"))
		return;
	TextureID temptex = texturemanager.add(std::string(file.mb_str()));
	Texture &tex = *((Texture*)texturemanager.items[temptex]);
	if (tex.w == 0 || tex.h == 0)
		return;

	wxString temp;

	unsigned char *tempbuf = (unsigned char*)malloc(tex.w*tex.h*4);
	tex.getPixels(tempbuf, GL_BGRA_EXT);

	CxImage *newImage = new CxImage(0);
	newImage->AlphaCreate();	// Create the alpha layer
	newImage->IncreaseBpp(32);	// set image to 32bit 
	newImage->CreateFromArray(tempbuf, tex.w, tex.h, 32, (tex.w*4), true);

	wxString fileName = file.AfterLast('\\').BeforeLast('.');
	wxString filePath = file.BeforeLast('\\');

	// -= Pre-Path Directories =-
	// Add any directories inbetween the target directory and the preserved directory go here.

	// Lightwave
	if (ExportID == "LWO"){
		if (modelExport_LW_PreserveDir == true){
			MakeDirs(outdir,"Images");
			outdir += "/Images/";
		}
	// Wavefront OBJ
	}else if (ExportID == "OBJ"){
	}

	// Restore WoW's content directories for this image.
	if (modelExport_PreserveDir == true){
		MakeDirs(outdir,filePath);
		outdir += filePath;
		outdir << "\\";
		outdir.Replace("\\","/");
	}

	// Final Filename
	temp = outdir+fileName+wxT(".")+suffix;

	//wxLogMessage(_T("Info: Exporting texture to %s..."), temp.c_str());

	// Save image!
	if (suffix == _T("tga"))
		newImage->Save(temp.mb_str(), CXIMAGE_FORMAT_TGA);
	else
		newImage->Save(temp.mb_str(), CXIMAGE_FORMAT_PNG);

	// Clear data we don't need anymore
	free(tempbuf);
	newImage->Destroy();
	wxDELETE(newImage);
	
}

// Limit a value by a min & a max. The Inc controls by how much to reduce for every run.
double Clamp(double value, float min, float max, float inc = PI){
	if (value > 0){
		while (value >= max){
			value -= inc;
		}
	}else if (value < 0){
		while (value <= min){
			value += inc;
		}
	}
	
	return value;
}

// Converts WoW's Radians into a 3D friendly Heading, Pitch, Bank system.
// Tested only with Lightwave. Might have to make minor direction changes here to make it more univsersal.
Vec3D QuaternionToXYZ(Vec3D Dir, float W){
	//-dir.z,dir.x,dir.y	WoW Direction...
	Vec3D vdir(Dir.x,Dir.z,-Dir.y);
	float c,angle_x,angle_y,angle_z,tempx,tempy;

	Vec3D XYZ;
	Matrix m;
	Quaternion q(vdir, W);
	m.quaternionRotate(q);

	angle_y = asin(m.m[0][2]);
	c = cos(angle_y);
	if (fabs(c) > 0.005){		// If not Gimble-locked
		tempx = m.m[2][2];
		tempy = -m.m[1][2];

		angle_x = atan2(tempy,tempx);

		tempx = m.m[0][0] / c;
		tempy = -m.m[0][1] / c;

		angle_z = atan2(tempy,tempx);
	}else{						// If Gimble-lock occured...
		wxString ays, cs;
		ays << angle_y;
		cs << c;
		wxLogMessage("Gimbal Lock happened! angle_y=%s, c=%s", ays, cs);

		angle_x  = 0;
		tempx = m.m[1][1];
		tempy = m.m[1][0];

		angle_z  = atan2(tempy, tempx);
	}

	if (angle_y < 0)
		angle_y = -angle_y;

	// 1.0 = 1 Radian, or 57.295779513082320876798154814114 degrees (180/PI)
	float d180 = PI;	// 180 degrees
	int accuracy = 6;	// to the 6th decimal point...
	XYZ.x = Clamp(round(angle_x+d180,accuracy),-PI*2,PI*2);			// Heading
	XYZ.y = Clamp(round(-angle_y,accuracy),-PI*2,PI*2);				// Pitch
	XYZ.z = -Clamp(round(angle_z+d180,accuracy),-PI*2,PI*2);		// Bank

	return XYZ;
}

void AddCount(Model *m)
{
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			numGroups++;
			
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				numVerts++;
			}
		}
	}
}

void AddVertices(Model *m, Attachment *att, bool init)
{
	int boneID = -1;
	Model *mParent = NULL;

	if (att->parent) {
		mParent = static_cast<Model*>(att->parent->model);
		if (mParent)
			boneID = mParent->attLookup[att->id];
	}

	Vec3D pos(0,0,0);
	Vec3D scale(1,1,1);
	if (boneID>-1) {
		// Note: We still need to rotate the items!!
		pos = mParent->atts[boneID].pos;
		Bone cbone = mParent->bones[mParent->atts[boneID].bone];
		Matrix mat = cbone.mat;
		if (init == true){
			// InitPose is a reference to the HandsClosed animation (#15), which is the closest to the Initial pose.
			// By using this animation, we'll get the proper scale for the items when in Init mode.
			int InitPose = 15;
			scale = cbone.scale.getValue(InitPose,0);
			if (scale.x == 0 && scale.y == 0 && scale.z == 0){
				scale.x = 1;
				scale.y = 1;
				scale.z = 1;
			}
		}else{
			// Scale takes into consideration only the final size of an object. This means that if a staff it rotated 90 degrees,
			// the final scale will be as if the staff is REALLY short. This should solve itself after we get rotations working.
			scale.x = mat.m[0][0];
			scale.y = mat.m[1][1];
			scale.z = mat.m[2][2];

			// Moves the item to the proper position.
			mat.translation(cbone.transPivot);
			pos.x = mat.m[0][3];
			pos.y = mat.m[1][3];
			pos.z = mat.m[2][3];
		}
	}

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {			
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				
				if ((m->vertices) && (init == false)) {
					verts[vertIndex].vertex.x = ((m->vertices[a].x * scale.x) + pos.x);
					verts[vertIndex].vertex.y = ((m->vertices[a].y * scale.y) + pos.y);
					verts[vertIndex].vertex.z = ((m->vertices[a].z * scale.z) + pos.z);

					if (video.supportVBO) {
						verts[vertIndex].normal.x = (m->vertices[m->header.nVertices + a].x + pos.x);
						verts[vertIndex].normal.y = (m->vertices[m->header.nVertices + a].y + pos.y);
						verts[vertIndex].normal.z = (m->vertices[m->header.nVertices + a].z + pos.z);
					} else {
						verts[vertIndex].normal.x = m->normals[a].x;
						verts[vertIndex].normal.y = m->normals[a].y;
						verts[vertIndex].normal.z = m->normals[a].z;
					}
				} else {
					verts[vertIndex].vertex.x = ((m->origVertices[a].pos.x * scale.x) + pos.x);
					verts[vertIndex].vertex.y = ((m->origVertices[a].pos.y * scale.y) + pos.y);
					verts[vertIndex].vertex.z = ((m->origVertices[a].pos.z * scale.z) + pos.z);

					verts[vertIndex].normal.x = m->origVertices[a].normal.x;
					verts[vertIndex].normal.y = m->origVertices[a].normal.y;
					verts[vertIndex].normal.z = m->origVertices[a].normal.z;
				}

				verts[vertIndex].tu = m->origVertices[a].texcoords.x;
				verts[vertIndex].tv = m->origVertices[a].texcoords.y;

				verts[vertIndex].groupIndex = grpIndex;
				verts[vertIndex].boneid = m->origVertices[a].bones[0];

				vertIndex++;
			}
			groups[grpIndex].p = p;
			groups[grpIndex].m = m;
			grpIndex++;
		}
	}
}

void InitCommon(Attachment *att, bool init)
{
	if (verts)
		wxDELETEA(verts);
	if (groups)
		wxDELETEA(groups);

	numVerts = 0;
	numGroups = 0;
	numFaces = 0;
	vertIndex = 0;
	grpIndex = 0;

	if (!att)
		return;

	Model *m = NULL;
	if (att->model) {
		m = static_cast<Model*>(att->model);
		if (!m)
			return;

		AddCount(m);
	}

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddCount(mAtt);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddCount(mAttChild);
		}
	}

	numFaces = numVerts / 3;

	verts = new ModelData[numVerts];
	//indices = new float[numVerts];
	groups = new GroupData[numGroups];

	if (m)
		AddVertices(m, att, init);

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddVertices(mAtt, att->children[i], init);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddVertices(mAttChild, att2->children[j], init);
		}
	}
}

// 3D Studio Max
// http://gpwiki.org/index.php/Loading_3ds_files
// http://www.martinreddy.net/gfx/3d/3DS.spec
// http://www.martinreddy.net/gfx/3d/MLI.spec
// http://gpwiki.org/index.php/C:Load3DS
void ExportM2to3DS(Attachment *att, Model *m, const char *fn, bool init)
{
	if (!m)
		return;

	//ofstream f(fn, ios::out | ios::binary | ios::trunc);
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	// ------------------------------------

	InitCommon(att, init);

	// Primary chunk
	MAX3DS_HEADER header;

	// EDIT3DS chunk
	MAX3DS_CHUNK chunk1;
	chunk1.id = EDIT3DS; // 0x3D3D

	// Now write the data to file
	f.Write(&header, sizeof(MAX3DS_HEADER));
	f.Write(&chunk1, sizeof(MAX3DS_CHUNK)); // EDIT3DS
	chunk1.size = sizeof(MAX3DS_CHUNK);

	// EDIT_CONFIG2 chunk
	MAX3DS_CHUNK chunk2_1;
	chunk2_1.id = MESH_VERSION; // 0x3D3E
	
	int meshVersion = 3;
	chunk2_1.size += sizeof(int);

	f.Write(&chunk2_1, sizeof(MAX3DS_CHUNK)); // MESH_VERSION
	f.Write(&meshVersion, sizeof(int));
	chunk1.size += chunk2_1.size;

	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			// EDIT_MATERIAL (0xAFFF)
			MAX3DS_CHUNK chunk2_2;
			chunk2_2.id = EDIT_MATERIAL; // 0xAFFF
			f.Write(&chunk2_2, sizeof(MAX3DS_CHUNK)); // EDIT_MATERIAL
			
			// MATNAME (0xA000) (See mli Doc) 
			// http://www.martinreddy.net/gfx/3d/MLI.spec
			MAX3DS_CHUNK chunk2_2_1;
			chunk2_2_1.id = MATNAME; // 0xA000
			wxString matName = wxString::Format(_T("Material_%i"), i);
			matName.Append('\0');
			chunk2_2_1.size = sizeof(MAX3DS_CHUNK) + matName.length();
			chunk2_2.size = sizeof(MAX3DS_CHUNK) + chunk2_2_1.size;
			f.Write(&chunk2_2_1, sizeof(MAX3DS_CHUNK)); // MATNAME
			f.Write(matName.data(), matName.length());

			//COL_RGB  
			//chunk2_2_2.size += 18;
			MAX3DS_CHUNK chunk2_2_2;
			chunk2_2_2.id = MATAMBIENT; // 0xA010
			MAX3DS_CHUNK chunk2_2_2_1;
			chunk2_2_2_1.id = RGB1;
			ColorRGBs rgb2_2_2_1;
			rgb2_2_2_1.r = 178;
			rgb2_2_2_1.g = 178;
			rgb2_2_2_1.b = 178;
			chunk2_2_2_1.size += sizeof(ColorRGBs);
			MAX3DS_CHUNK chunk2_2_2_2;
			chunk2_2_2_2.id = RGB2;
			ColorRGBs rgb2_2_2_2;
			rgb2_2_2_2.r = 178;
			rgb2_2_2_2.g = 178;
			rgb2_2_2_2.b = 178;
			chunk2_2_2_2.size += sizeof(ColorRGBs);
			chunk2_2_2.size += chunk2_2_2_1.size;
			chunk2_2_2.size += chunk2_2_2_2.size;
			chunk2_2.size += chunk2_2_2.size;
			f.Write(&chunk2_2_2, sizeof(MAX3DS_CHUNK)); // MATAMBIENT
			f.Write(&chunk2_2_2_1, sizeof(MAX3DS_CHUNK)); // RGB1
			f.Write(&rgb2_2_2_1, sizeof(ColorRGBs));
			f.Write(&chunk2_2_2_2, sizeof(MAX3DS_CHUNK)); // RGB2
			f.Write(&rgb2_2_2_2, sizeof(ColorRGBs));

			//chunk2_2_3.size += 18;
			MAX3DS_CHUNK chunk2_2_3;
			chunk2_2_3.id = MATDIFFUSE; // 0xA020
			MAX3DS_CHUNK chunk2_2_3_1;
			chunk2_2_3_1.id = RGB1; // 0x0011
			ColorRGBs rgb2_2_3_1;
			rgb2_2_3_1.r = 255;
			rgb2_2_3_1.g = 255;
			rgb2_2_3_1.b = 255;
			chunk2_2_3_1.size += sizeof(ColorRGBs);
			MAX3DS_CHUNK chunk2_2_3_2;
			chunk2_2_3_2.id = RGB2; // 0x0012
			ColorRGBs rgb2_2_3_2;
			rgb2_2_3_2.r = 255;
			rgb2_2_3_2.g = 255;
			rgb2_2_3_2.b = 255;
			chunk2_2_3_2.size += sizeof(ColorRGBs);
			chunk2_2_3.size += chunk2_2_3_1.size;
			chunk2_2_3.size += chunk2_2_3_2.size;
			chunk2_2.size += chunk2_2_3.size;
			f.Write(&chunk2_2_3, sizeof(MAX3DS_CHUNK)); // MATDIFFUSE
			f.Write(&chunk2_2_3_1, sizeof(MAX3DS_CHUNK)); // RGB1
			f.Write(&rgb2_2_3_1, sizeof(ColorRGBs));
			f.Write(&chunk2_2_3_2, sizeof(MAX3DS_CHUNK)); // RGB2
			f.Write(&rgb2_2_3_2, sizeof(ColorRGBs));

			//chunk2_2_4.size += 18;
			MAX3DS_CHUNK chunk2_2_4;
			chunk2_2_4.id = MATSPECULAR; // 0xA030
			MAX3DS_CHUNK chunk2_2_4_1;
			chunk2_2_4_1.id = RGB1; // 0x0011
			ColorRGBs rgb2_2_4_1;
			rgb2_2_4_1.r = 0;
			rgb2_2_4_1.g = 0;
			rgb2_2_4_1.b = 0;
			chunk2_2_4_1.size += sizeof(ColorRGBs);
			MAX3DS_CHUNK chunk2_2_4_2;
			chunk2_2_4_2.id = RGB2; // 0x0012
			ColorRGBs rgb2_2_4_2;
			rgb2_2_4_2.r = 0;
			rgb2_2_4_2.g = 0;
			rgb2_2_4_2.b = 0;
			chunk2_2_4_2.size += sizeof(ColorRGBs);
			chunk2_2_4.size += chunk2_2_4_1.size;
			chunk2_2_4.size += chunk2_2_4_2.size;
			chunk2_2.size += chunk2_2_4.size;
			f.Write(&chunk2_2_4, sizeof(MAX3DS_CHUNK)); // MATDIFFUSE
			f.Write(&chunk2_2_4_1, sizeof(MAX3DS_CHUNK)); // RGB1
			f.Write(&rgb2_2_4_1, sizeof(ColorRGBs));
			f.Write(&chunk2_2_4_2, sizeof(MAX3DS_CHUNK)); // RGB2
			f.Write(&rgb2_2_4_2, sizeof(ColorRGBs));

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x40, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x41, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x50, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x52, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x53, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x08;
			char aa[] = {0x00, 0xa1, 0x08, 0, 0, 0, 0x3, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0e;
			char aa[] = {0x84, 0xa0, 0x0e, 0, 0, 0, 0x30, 0, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x0a;
			char aa[] = {0x87, 0xa0, 0x0a, 0, 0, 0, 0, 0, 0x80, 0x3f};
			f.Write(aa, sizeof(aa));
			}

			{
			chunk2_2.size += 0x06;
			char aa[] = {0x8c, 0xa0, 0x06, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			MAX3DS_CHUNK chunk2_2_6;
			chunk2_2_6.id = MATMAPFILE; // 0xA300
			wxString mapName(fn, wxConvUTF8);
			mapName = mapName.AfterLast('\\').BeforeLast('.');
			mapName << wxT("_") << p.tex << wxT(".tga");
			mapName.Append('\0');
			chunk2_2_6.size = sizeof(MAX3DS_CHUNK) + mapName.length();

			// save texture to file
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += mapName;
			SaveTexture(texFilename);

			{
			// conatins 0xa300, 0xa351
			chunk2_2.size += (0x0e+chunk2_2_6.size+0x08);
			char aa[] = {0x00, 0xa2, 0x0e+chunk2_2_6.size+0x08, 0, 0, 0, 0x30, 0, 8, 0, 0, 0, 0x64, 0};
			f.Write(aa, sizeof(aa));
			}

			f.Write(&chunk2_2_6, sizeof(MAX3DS_CHUNK)); // MATMAPNAME
			f.Write(mapName.data(), mapName.length());

			{
			char aa[] = {0x51, 0xa3, 0x08, 0, 0, 0, 0, 0};
			f.Write(aa, sizeof(aa));
			}

			int off_t = 0-chunk2_2.size;
			f.SeekO(off_t, wxFromCurrent);
			f.Write(&chunk2_2, sizeof(MAX3DS_CHUNK)); // EDIT_MATERIAL
			f.SeekO(0, wxFromEnd);
			chunk1.size += chunk2_2.size;
		}
	}

	//EDIT_CONFIG1 chunk
	MAX3DS_CHUNK chunk2_3;
	chunk2_3.id = EDIT_CONFIG1; // 0x0100
	float config1 = 1.0f;
	chunk2_3.size += sizeof(float);

	f.Write(&chunk2_3, sizeof(MAX3DS_CHUNK)); // EDIT_CONFIG1
	f.Write(&config1, sizeof(float));
	chunk1.size += chunk2_3.size;

	{
	chunk1.size += 0x0a;
	char aa[] = {0x0, 0x14, 0x0a, 0, 0, 0, 0, 0, 0x80, 0x3f};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x08;
	char aa[] = {0x20, 0x14, 0x08, 0, 0, 0, 0, 2};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x0a;
	char aa[] = {0x50, 0x14, 0x0a, 0, 0, 0, 0, 0, 0x40, 0x40};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0xa;
	char aa[] = {0x60, 0x14, 0x0a, 0, 0, 0, 0, 0, 0x80, 0x3f};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x12;
	char aa[] = {0x0, 0x15, 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	f.Write(aa, sizeof(aa));
	}

	{
	chunk1.size += 0x2a;
	char aa[] = {0x0, 0x21, 0x2a, 0, 0, 0, 0x10, 0, 0x12, 0, 0, 0, 0x9a, 0x99, 0x99, 0x3e,
		0x9a, 0x99, 0x99, 0x3e, 0x9a, 0x99, 0x99, 0x3e, 0x13, 0, 0x12, 0, 0, 0, 0x9a, 0x99,
		0x99, 0x3e, 0x9a, 0x99, 0x99, 0x3e, 0x9a, 0x99, 0x99, 0x3e};
	f.Write(aa, sizeof(aa));
	}


	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			
			// EDIT OBJECT chunk
			MAX3DS_CHUNK chunk2_4;
			chunk2_4.id = EDIT_OBJECT; // 0x4000
			
			// Model name
			wxString modName = wxString::Format(_T("Geoset_%i"), i);
			modName.Append('\0');
			chunk2_4.size += modName.length();
			
			// OBJ_MESH chunk
			MAX3DS_CHUNK chunk3;
			chunk3.id = OBJ_MESH; // 0x4100
			
			// The objects vertices
			MAX3DS_CHUNK chunk4_1;
			chunk4_1.id = MESH_VERTICES; // 0x4110
			chunk4_1.size += (sizeof(unsigned short) + p.indexCount*3*4);
			
			// The UV Map
			MAX3DS_CHUNK chunk4_2;
			chunk4_2.id = MESH_UV; // 0x4140
			chunk4_2.size += (sizeof(unsigned short) + p.indexCount*2*4);

			// The objects faces
			MAX3DS_CHUNK chunk4_3;
			chunk4_3.id = MESH_FACEL1; // 0x4120
			numFaces = p.indexCount / 3;
			chunk4_3.size += (sizeof(unsigned short) + numFaces*4*2);

			// Material
			wxString matName = wxString::Format(_T("Material_%i"), i);
			matName.Append('\0');
			MAX3DS_CHUNK chunk4_3_1;
			chunk4_3_1.id = MESH_MATERIAL; // 0x4130
			chunk4_3_1.size += matName.length();
			chunk4_3_1.size += (sizeof(unsigned short) + numFaces*2);
			chunk4_3.size += chunk4_3_1.size;

			// Smooth
			MAX3DS_CHUNK chunk4_3_2;
			chunk4_3_2.id = MESH_SMOOTH; // 0x4150
			chunk4_3_2.size += (numFaces*4);
			chunk4_3.size += chunk4_3_2.size;

			chunk3.size += chunk4_1.size;
			chunk3.size += chunk4_2.size;
			chunk3.size += chunk4_3.size;
			chunk2_4.size += chunk3.size;
			chunk1.size += chunk2_4.size;

			f.Write(&chunk2_4, sizeof(MAX3DS_CHUNK));
			f.Write(modName.data(), modName.length());
			f.Write(&chunk3, sizeof(MAX3DS_CHUNK));

			// Mesh chunk Vertice count, and vertices 0x4110
			f.Write(&chunk4_1, sizeof(MAX3DS_CHUNK));
			f.Write(&p.indexCount, sizeof(unsigned short));
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				if (init == false) {
					vert.x = (m->vertices[a].x);
					vert.y = (m->vertices[a].y);
					vert.z = (m->vertices[a].z);
				} else {
					vert.x = (m->origVertices[a].pos.x);
					vert.y = (m->origVertices[a].pos.y);
					vert.z = (m->origVertices[a].pos.z);
				}
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
			}

			// The UV Map 0x4140
			f.Write(&chunk4_2, sizeof(MAX3DS_CHUNK));
			f.Write(&p.indexCount, sizeof(unsigned short));
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				float x = m->origVertices[a].texcoords.x;
				float y = (1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&x), 4);
				f.Write(reinterpret_cast<char *>(&y), 4);
			}		

			// 0x4160
			// 0x4165

			// Face List 0x4120
			f.Write(&chunk4_3, sizeof(MAX3DS_CHUNK));
			int count = 0, unkid = 0;
			f.Write(&numFaces, sizeof(unsigned short));
			for (uint16 k=0; k<p.indexCount; k+=3) {
				f.Write(&count, 2);
				count++;
				f.Write(&count, 2);
				count++;
				f.Write(&count, 2);
				count++;
				f.Write(&unkid, 2);
			}

			// Face List - Material 0x4130
			f.Write(&chunk4_3_1, sizeof(MAX3DS_CHUNK));
			f.Write(matName.data(), matName.length());
			f.Write(&numFaces, sizeof(unsigned short));
			count = 0;
			for (uint16 k=0; k<p.indexCount; k+=3) {
				f.Write(&count, 2);
				count++;
			}

			// Face List - Smooth 0x4150
			f.Write(&chunk4_3_2, sizeof(MAX3DS_CHUNK));
			count = 1;
			for (uint16 k=0; k<p.indexCount; k+=3) {
				f.Write(&count, 4);
			}
		}
	}
	header.primary.size += chunk1.size;

	f.SeekO(0, wxFromStart);
	f.Write(&header, sizeof(MAX3DS_HEADER));
	f.Write(&chunk1, sizeof(MAX3DS_CHUNK)); // EDIT3DS
	f.SeekO(0, wxFromEnd);

	f.Close();
}

// MilkShape 3D
void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init)
{
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	InitCommon(att, init);

	// Write the header
	ms3d_header_t header;
	strncpy(header.id, "MS3D000000", sizeof(header.id));
	header.version = 4;

	// Header
	f.Write(reinterpret_cast<char *>(&header), sizeof(ms3d_header_t));
	// Vertex Count
	f.Write(reinterpret_cast<char *>(&numVerts), sizeof(numVerts));
	
	// Write Vertex data?
	for (unsigned int i=0; i<numVerts; i++) {
		ms3d_vertex_t vert;
		vert.boneId = verts[i].boneid;
		vert.flags = 0; //SELECTED;
		vert.referenceCount = 0; // what the?
		vert.vertex[0] = verts[i].vertex.x;
		vert.vertex[1] = verts[i].vertex.y;
		vert.vertex[2] = verts[i].vertex.z;
		f.Write(reinterpret_cast<char *>(&vert), sizeof(ms3d_vertex_t));
	}
	// ---------------------------

	// Triangle Count
	f.Write(reinterpret_cast<char *>(&numFaces), sizeof(numFaces));

	// Write Triangle Data?
	for (unsigned int i=0; i<numVerts; i+=3) {
		ms3d_triangle_t tri;
		tri.flags = 0; //SELECTED;
		tri.groupIndex = (unsigned char)verts[i].groupIndex;
		tri.smoothingGroup = 1; // 1 - 32

		for (int j=0; j<3; j++) {
			tri.vertexIndices[j] = i+j;
			tri.s[j] = verts[i+j].tu;
			tri.t[j] = verts[i+j].tv;
			
			tri.vertexNormals[j][0] = verts[i+j].normal.x;
			tri.vertexNormals[j][1] = verts[i+j].normal.y;
			tri.vertexNormals[j][2] = verts[i+j].normal.z;
		}

		f.Write(reinterpret_cast<char *>(&tri), sizeof(ms3d_triangle_t));
	}
	// ---------------------------

	// Number of groups
	f.Write(reinterpret_cast<char *>(&numGroups), sizeof(numGroups));

	unsigned short indiceCount = 0;
	for (unsigned short i=0; i<numGroups; i++) {
		wxString groupName(wxString::Format(_T("Geoset_%i"), i));

		const char flags = 0; // SELECTED
		f.Write(&flags, sizeof(flags));

		char name[32];
		strncpy(name, groupName.mb_str(), sizeof(name));
		f.Write(name, sizeof(name));

		unsigned short faceCount = groups[i].p.indexCount / 3;
		f.Write(reinterpret_cast<char *>(&faceCount), sizeof(faceCount));
		
		for (int k=0; k<faceCount; k++) {
			//triIndices[k] = indiceCount;
			f.Write(reinterpret_cast<char *>(&indiceCount), sizeof(indiceCount));
			indiceCount++;
		}

		unsigned char gIndex = (char)i;
		f.Write(reinterpret_cast<char *>(&gIndex), sizeof(gIndex));
	}

	// Number of materials (pretty much identical to groups, each group has its own material)
	f.Write(reinterpret_cast<char *>(&numGroups), sizeof(numGroups));
	
	for (unsigned short i=0; i<numGroups; i++) {
		wxString matName(wxString::Format(_T("Material_%i"), i));

		ModelRenderPass p = groups[i].p;
		if (p.init(groups[i].m)) {
			ms3d_material_t mat;
			memset(mat.alphamap, '\0', sizeof(mat.alphamap));

			strncpy(mat.name, matName.mb_str(), sizeof(mat.name));
			mat.ambient[0] = 0.7f;
			mat.ambient[1] = 0.7f;
			mat.ambient[2] = 0.7f;
			mat.ambient[3] = 1.0f;
			mat.diffuse[0] = p.ocol.x;
			mat.diffuse[1] = p.ocol.y;
			mat.diffuse[2] = p.ocol.z;
			mat.diffuse[3] = p.ocol.w;
			mat.specular[0] = 0.0f;
			mat.specular[1] = 0.0f;
			mat.specular[2] = 0.0f;
			mat.specular[3] = 1.0f;
			mat.emissive[0] = p.ecol.x;
			mat.emissive[1] = p.ecol.y;
			mat.emissive[2] = p.ecol.z;
			mat.emissive[3] = p.ecol.w;
			mat.transparency = p.ocol.w;

			if (p.useEnvMap) {
				mat.shininess = 30.0f;
				mat.mode = 1;
			} else {
				mat.shininess = 0.0f;
				mat.mode = 0;
			}
/*
			unsigned int bindtex = 0;
			if (groups[i].m->specialTextures[p.tex]==-1) 
				bindtex = groups[i].m->textures[p.tex];
			else 
				bindtex = groups[i].m->replaceTextures[groups[i].m->specialTextures[p.tex]];
*/
			wxString texName(fn, wxConvUTF8);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << wxT("_") << p.tex << wxT(".tga");
			strncpy(mat.texture, texName.mb_str(), sizeof(mat.texture));

			f.Write(reinterpret_cast<char *>(&mat), sizeof(ms3d_material_t));

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}

#if 0
	// save some keyframe data
	float fps = 1.0f;
	float fCurTime = 0.0f;
	int totalFrames = 0;

	f.Write(reinterpret_cast<char *>(&fps), sizeof(fps));
	f.Write(reinterpret_cast<char *>(&fCurTime), sizeof(fCurTime));
	f.Write(reinterpret_cast<char *>(&totalFrames), sizeof(totalFrames));
	
	
	// number of joints
	unsigned short numJoints = 0; //(unsigned short)m->header.nBones;

	f.Write(reinterpret_cast<char *>(&numJoints), sizeof(numJoints));
#else
	// TODO
	// save some keyframe data
	float fps = 1.0f; //m->anims[m->anim].playSpeed;
	float fCurTime = 0.0f;
	int totalFrames = 0; // (m->anims[m->anim].timeEnd - m->anims[m->anim].timeStart);

	f.Write(reinterpret_cast<char *>(&fps), sizeof(fps));
	f.Write(reinterpret_cast<char *>(&fCurTime), sizeof(fCurTime));
	f.Write(reinterpret_cast<char *>(&totalFrames), sizeof(totalFrames));
	
	// number of joints

	unsigned short numJoints = (unsigned short)m->header.nBones;

	f.Write(reinterpret_cast<char *>(&numJoints), sizeof(numJoints));

	for (int i=0; i<numJoints; i++) {
		ms3d_joint_t joint;

		joint.flags = 0; // SELECTED
		memset(joint.name, '\0', sizeof(joint.name));
		snprintf(joint.name, sizeof(joint.name), "Bone_%i_%i", m->anim, i);
		memset(joint.parentName, '\0', sizeof(joint.parentName));

		joint.rotation[0] = 0; // m->bones[i].pivot.x;
		joint.rotation[1] = 0; // m->bones[i].pivot.y;
		joint.rotation[2] = 0; // m->bones[i].pivot.z;

		joint.position[0] = m->bones[i].transPivot.x;
		joint.position[1] = m->bones[i].transPivot.y;
		joint.position[2] = m->bones[i].transPivot.z;

		int parent = m->bones[i].parent;
		if (parent > -1) {
			snprintf(joint.parentName, sizeof(joint.parentName), "Bone_%i_%i", m->anim, parent);

			joint.position[0] -= m->bones[parent].transPivot.x;
			joint.position[1] -= m->bones[parent].transPivot.y;
			joint.position[2] -= m->bones[parent].transPivot.z;
		}

		joint.numKeyFramesRot = 0; //(unsigned short)m->bones[i].rot.data[m->anim].size();
		joint.numKeyFramesTrans = 0; //(unsigned short)m->bones[i].trans.data[m->anim].size();

		f.Write(reinterpret_cast<char *>(&joint), sizeof(ms3d_joint_t));

		if (joint.numKeyFramesRot > 0) {
			ms3d_keyframe_rot_t *keyFramesRot = new ms3d_keyframe_rot_t[joint.numKeyFramesRot];
			for (int j=0; j<joint.numKeyFramesRot; j++) {
				keyFramesRot[j].time = m->bones[i].rot.times[m->anim][j]; // Error, time in seconds;
				keyFramesRot[j].rotation[0] = m->bones[i].rot.data[m->anim][j].x;
				keyFramesRot[j].rotation[1] = m->bones[i].rot.data[m->anim][j].y;
				keyFramesRot[j].rotation[2] = m->bones[i].rot.data[m->anim][j].z;
			}

			f.Write(reinterpret_cast<char *>(keyFramesRot), sizeof(ms3d_keyframe_rot_t) * joint.numKeyFramesRot);
			wxDELETEA(keyFramesRot);
		}

		if (joint.numKeyFramesTrans > 0) {
			ms3d_keyframe_pos_t *keyFramesTrans = new ms3d_keyframe_pos_t[joint.numKeyFramesTrans];
			for (int j=0; j<joint.numKeyFramesTrans; j++) {
				keyFramesTrans[j].time = m->bones[i].trans.times[m->anim][j]; // Error,time in seconds;;
				keyFramesTrans[j].position[0] = m->bones[i].trans.data[m->anim][j].x;
				keyFramesTrans[j].position[1] = m->bones[i].trans.data[m->anim][j].y;
				keyFramesTrans[j].position[2] = m->bones[i].trans.data[m->anim][j].z;
				if (parent > -1) {
					keyFramesTrans[j].position[0] -= m->bones[parent].transPivot.x;
					keyFramesTrans[j].position[1] -= m->bones[parent].transPivot.y;
					keyFramesTrans[j].position[2] -= m->bones[parent].transPivot.z;
					if (m->bones[parent].trans.data[m->anim].size() > j) {
						keyFramesTrans[j].position[0] -= m->bones[parent].trans.data[m->anim][j].x;
						keyFramesTrans[j].position[1] -= m->bones[parent].trans.data[m->anim][j].y;
						keyFramesTrans[j].position[2] -= m->bones[parent].trans.data[m->anim][j].z;
					}
				}
			}

			f.Write(reinterpret_cast<char *>(keyFramesTrans), sizeof(ms3d_keyframe_pos_t) * joint.numKeyFramesTrans);
			wxDELETEA(keyFramesTrans);
		}
	}
#endif
	f.Close();

	if (verts)
		wxDELETEA(verts);
	if (groups)
		wxDELETEA(groups);
}

void ExportM2toOBJ(Attachment *att, Model *m, const char *fn, bool init)
{
	// Open file
	ofstream f(fn, ios_base::out | ios_base::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	InitCommon(att, init);
	wxString out;

	// http://people.sc.fsu.edu/~burkardt/data/mtl/mtl.html
	wxString matName(fn, wxConvUTF8);
	matName = matName.BeforeLast('.');
	matName << _T(".mtl");

	ofstream fm(matName.mb_str(), ios_base::out | ios_base::trunc);
	matName = matName.AfterLast('\\');

	fm << "#" << endl;
	fm << "# " << matName.mb_str() << endl;
	fm << "#" << endl;
	fm <<  endl;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
			
		if (p.init(m)) {
			wxString texName(fn, wxConvUTF8);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << _T("_") << p.tex << _T(".tga");
			fm << "newmtl " << "Material_" << i << endl;
			fm << "illum 2" << endl;
			out = wxString::Format(_T("Kd %.06f %.06f %.06f"), p.ocol.x, p.ocol.y, p.ocol.z);
			fm << out.c_str() << endl;
			out = wxString::Format(_T("Ka %.06f %.06f %.06f"), 0.7f, 0.7f, 0.7f);
			fm << out.c_str() << endl;
			out = wxString::Format(_T("Ks %.06f %.06f %.06f"), p.ecol.x, p.ecol.y, p.ecol.z);
			fm << out.c_str() << endl;
			fm << "Ke 0.000000 0.000000 0.000000" << endl;
			out = wxString::Format(_T("Ns %0.6f"), 0.0f);
			fm << out.c_str() << endl;
			//fm << "Ka " << 0.7f << " " << 0.7f << " " << 0.7f << endl;
			//fm << "Kd " << p.ocol.x << " " << p.ocol.y << " " << p.ocol.z << endl;
			//fm << "Ks " << p.ecol.x << " " << p.ecol.y << " " << p.ecol.z << endl;
			//fm << "Ns " << p.ocol.w << endl;
			fm << "map_Kd " << texName.c_str() << endl << endl;

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}

	fm.close();

	f << "# Wavefront OBJ exported by WoW Model Viewer v0.5.0.8" << endl << endl;
	f << "mtllib " << matName.mb_str() << endl << endl;

	// output all the vertice data
	int vertics = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			//f << "# Chunk Indice Count: " << p.indexCount << endl;

			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				if (m->vertices == NULL || init == true) {
					out = wxString::Format(_T("v %.06f %.06f %.06f"), m->origVertices[a].pos.x, m->origVertices[a].pos.y, m->origVertices[a].pos.z);
					f << out.c_str() << endl;
					//f << "v " << m->origVertices[a].pos.x << " " << m->origVertices[a].pos.y << " " << m->origVertices[a].pos.z << endl;
				} else {
					out = wxString::Format(_T("v %.06f %.06f %.06f"), m->vertices[a].x, m->vertices[a].y, m->vertices[a].z);
					f << out.c_str() << endl;
					//f << "v " << m->vertices[a].x << " " << m->vertices[a].y << " " << m->vertices[a].z << endl;
				}
				vertics ++;
			}
		}
	}
	f << "# " << vertics << " vertices" << endl << endl;

	// output all the texture coordinate data
	int textures = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		// we don't want to render completely transparent parts
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				out = wxString::Format(_T("vt %.06f %.06f"), m->origVertices[a].texcoords.x, 1-m->origVertices[a].texcoords.y);
				f << out.c_str() << endl;
				//f << "vt " << m->origVertices[a].texcoords.x << " " << (1 - m->origVertices[a].texcoords.y) << endl;
				textures ++;
			}
		}
	}
	f << "# " << textures << " texture coordinates" << endl << endl;

	// output all the vertice normals data
	int normals = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				out = wxString::Format(_T("vn %.06f %.06f %.06f"), m->origVertices[a].normal.x, m->origVertices[a].normal.y, m->origVertices[a].normal.z);
				f << out.c_str() << endl;
				//f << "vn " << m->origVertices[a].normal.x << " " << m->origVertices[a].normal.y << " " << m->origVertices[a].normal.z << endl;
				normals ++;
			}
		}
	}
	f << "# " << normals << " normals" << endl << endl;

	int counter=1;
	// output the indice data
	int triangles = 0;
	int triangles_total = 0;
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			f << "g Geoset_" << i << endl;
			f << "usemtl Material_" << i << endl;
			f << "s 1" << endl;
			triangles = 0;
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				f << "f ";
				f << counter << "/" << counter << "/" << counter << " ";
				counter ++;
				f << counter << "/" << counter << "/" << counter << " ";
				counter ++;
				f << counter << "/" << counter << "/" << counter << endl;
				counter ++;
				triangles ++;
			}
			f << "# " << triangles << " triangles in group" << endl << endl;
			triangles_total += triangles;
		}
	}

	f << "# " << triangles_total << " triangles total" << endl << endl;
	
	// Close file
	f.close();
}

void ExportWMOtoOBJ(WMO *m, const char *fn)
{
	// Open file
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast('\\');
		Name << file.AfterLast('\\');
		Path2 << wxString(m->name).BeforeLast('\\');

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << '\\' << Path2 << '\\' << Name;
	}

	ofstream f(file.c_str(), ios_base::out | ios_base::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	wxString mtlName = file;
	mtlName = mtlName.BeforeLast('.');
	mtlName << _T(".mtl");

	ofstream fm(mtlName.mb_str(), ios_base::out | ios_base::trunc);
	mtlName = mtlName.AfterLast('\\');

	fm << "#" << endl;
	fm << "# " << mtlName.mb_str() << endl;
	fm << "#" << endl;
	fm <<  endl;

	wxString *texarray = new wxString[m->nTextures+1];

	// Find a Match for mat->tex and place it into the Texture Name Array.
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
				outname = outname.AfterLast('\\');
				texarray[mat->tex] = outname << wxString::Format(_T("_Material_%03i"), mat->tex);
			}
		}
	}

	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			//wxString matName(wxString::Format(_T("Material_%03i"), mat->tex));
			wxString matName = texarray[mat->tex];

			//wxString texName(fn, wxConvUTF8);
			wxString texName = texarray[mat->tex];
			wxString texPath = texName.BeforeLast('\\');
			texName = texName.AfterLast('\\');
			//texName << _T("_") << mat->tex << _T(".tga");
			texName << _T(".tga");

			//fm << "newmtl " << "Material_" << mat->tex+1 << endl;
			// MilkShape3D cann't read long texname
			if (modelExport_PreserveDir == true)
				fm << "newmtl " << texarray[mat->tex] << endl;
			else
				fm << "newmtl " << texarray[mat->tex].AfterLast('\\') << endl;
			fm << "Kd 0.750000 0.750000 0.750000" << endl; // diffuse
			fm << "Ka 0.250000 0.250000 0.250000" << endl; // ambient
			fm << "Ks 0.000000 0.000000 0.000000" << endl; // specular
			//fm << "Ke 0.000000 0.000000 0.000000" << endl;
			fm << "Ns 0.000000" << endl;
			fm << "map_Kd " << texName.c_str() << endl << endl;

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			
			if (modelExport_PreserveDir == true){
				wxString Path1, Path2, Name;
				Path1 << wxString(texFilename, wxConvUTF8).BeforeLast('\\');
				Name << texName.AfterLast('\\');
				Path2 << texPath;

				MakeDirs(Path1,Path2);

				texFilename.Empty();
				texFilename << Path1 << '\\' << Path2 << '\\' << Name;
			}

			// setup texture
			glBindTexture(GL_TEXTURE_2D, mat->tex);
			SaveTexture(texFilename);
		}
	}
	fm.close();

	f << "# Wavefront OBJ exported by WoW Model Viewer v0.5.0.8" << endl << endl;
	f << "mtllib " << mtlName.mb_str() << endl << endl;

	// geometric vertices (v)
	// v x y z weight
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "v " << m->groups[i].vertices[a].x << " " << m->groups[i].vertices[a].z << " " << -m->groups[i].vertices[a].y << endl;
			}
		}
	}
	f << endl;

	// texture vertices (vt)
	// vt horizontal vertical depth
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "vt " << m->groups[i].texcoords[a].x << " " << (1 - m->groups[i].texcoords[a].y) << endl;
			}
		}
	}
	f << endl;

	// vertex normals (vn)
	// vn x y z
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "vn " << m->groups[i].normals[a].x << " " << m->groups[i].normals[a].z << " " << -m->groups[i].normals[a].y << endl;
			}
		}
	}
	f << endl;

	// Referencing groups of vertices
	// f v/vt/vn v/vt/vn v/vt/vn v/vt/vn
	int counter = 1;
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			WMOMaterial *mat = &m->mat[batch->texture];

			// batch->texture or mat->tex ?
			f << "g Geoset_" << i << "_" << j << "_tex_" << int(batch->texture) << endl;
			f << "s 1" << endl;
			//f << "usemtl Material_" << mat->tex+1 << endl;
			// MilkShape3D cann't read long texname
			if (modelExport_PreserveDir == true)
				f << "usemtl " << texarray[mat->tex] << endl;
			else
				f << "usemtl " << texarray[mat->tex].AfterLast('\\') << endl;
			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				f << "f ";
				f << counter << "/" << counter << "/" << counter << " ";
				f << (counter+1) << "/" << (counter+1) << "/" << (counter+1) << " ";
				f << (counter+2) << "/" << (counter+2) << "/" << (counter+2) << endl;
				counter += 3;
			}
			f << endl;
		}
	}

	// Close file
	f.close();
	wxDELETEA(texarray);
}

// Now for the non-working functions. These are most-likely disabled, but are here so
// someone can eventually work on them.
void ExportM2toCOLLADA(Attachment *att, Model *m, const char *fn, bool init){}
void ExportWMOtoCOLLADA(WMO *m, const char *fn){}
void ExportWMOtoMS3D(WMO *m, const char *fn){}
void ExportWMOto3DS(WMO *m, const char *fn){}
void ExportWMOtoX3D(WMO *m, const char *fn){}
void ExportWMOtoXHTML(WMO *m, const char *fn){}
