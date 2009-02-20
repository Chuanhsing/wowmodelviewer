
#include <wx/wfstream.h>

#include "modelexport.h"
#include "lwoheader.h"
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
};

struct GroupData {
	ModelRenderPass p;
	Model *m;
};

ModelData *verts = NULL;
GroupData *groups = NULL;

// 2 methods to go,  just export the entire m2 model.
// or use our "drawing" routine to export only whats being drawn.

template <typename T>
T reverse_endian(T n) // for 32bits
{
	_asm
	{
		mov EAX, n;
		bswap EAX;
		mov n, EAX;
	}

	return n;
}

unsigned short ByteSwap16 (unsigned short nValue) // 16bit
{
   return (((nValue>> 8)) | (nValue << 8));
}


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
		newImage->Save(fn, CXIMAGE_FORMAT_PNG);
	else
		newImage->Save(fn, CXIMAGE_FORMAT_TGA);

	newImage->Destroy();
	wxDELETE(newImage);
	wxDELETEA(pixels);
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

void AddVertices(Model *m, Attachment *att)
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
		pos = mParent->atts[boneID].pos;
		Matrix mat = mParent->bones[mParent->atts[boneID].bone].mat;
		/*
		pos.x += mat.m[0][4];
		pos.y += mat.m[1][4];
		pos.z += mat.m[2][4];
		*/
		scale.x = mat.m[0][0];
		scale.y = mat.m[1][1];
		scale.z = mat.m[2][2];
	}

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {			
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				
				if (m->vertices) {
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

				vertIndex++;
			}
			groups[grpIndex].p = p;
			groups[grpIndex].m = m;
			grpIndex++;
		}
	}
}

void InitCommon(Attachment *att)
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
		AddVertices(m, att);

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddVertices(mAtt, att->children[i]);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddVertices(mAttChild, att2->children[j]);
		}
	}
}

void ExportM2to3DS(Model *m, const char *fn)
{
	if (!m)
		return;

	//ofstream f(fn, ios::out | ios::binary | ios::trunc);
	wxFFileOutputStream f(fn, "w+b");

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	unsigned short numVerts = 0;

	/*
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				numVerts++;
			}
		}
	}
	*/
	numVerts = m->header.nVertices;

	Vertex3f *verts = new Vertex3f[numVerts];
	for (int i=0; i<numVerts; i++) {
		verts[i].x = m->vertices[i].x;
		verts[i].y = m->vertices[i].y;
		verts[i].z = m->vertices[i].z;
	}

	unsigned int numIndices = 0;

	// Indice count
	for (size_t i=0; i<m->passes.size(); i++) {
		if (m->passes[i].init(m)) {
			numIndices += m->passes[i].indexCount;
		}
	}

	numFaces = numIndices / 3;
	MAX3DS_FACE *tris = new MAX3DS_FACE[numFaces];

	int count=0;
	int triCount = 0;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			//int tri=0;
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k+=3,b+=3) {
				tris[triCount].x = m->indices[b];
				tris[triCount].y = m->indices[b+1];
				tris[triCount].z = m->indices[b+2];

				triCount++;

				/*
				uint16 a = m->indices[b];

				verts[count].x = m->vertices[a].x;
				verts[count].y = m->vertices[a].y;
				verts[count].z = m->vertices[a].z;
				count++;
				tri++;

				if (tri == 3) {
					tri = 0;
					tris[tricount].x = count-2;
					tris[tricount].y = count-1;
					tris[tricount].z = count;
					tricount++;
				}
				*/
			}
		}
	}


	// Primary chunk
	MAX3DS_HEADER header;

	// EDIT3DS chunk
	MAX3DS_CHUNK chunk1;
	chunk1.id = EDIT3DS;

	// EDIT_CONFIG2 chunk
	MAX3DS_CHUNK chunk2_1;
	chunk2_1.id = MESH_VERSION;
	
	int meshVersion = 3;
	chunk2_1.size += sizeof(int);

	// EDIT_MATERIAL (0xAFFF)
	MAX3DS_CHUNK chunk2_2;
	chunk2_2.id = EDIT_MATERIAL;

	// MATNAME (0xA000) (See mli Doc) 
	MAX3DS_CHUNK chunk2_2_1;
	chunk2_2_1.id = MATNAME;

	// Material name
	wxString matName(_T("Material1\0"));
	const char *materialName = matName.c_str();
	chunk2_2_1.size += int(matName.size() + 1);

	//COL_RGB  

	//EDIT_CONFIG1  (0x0100) chunk
	MAX3DS_CHUNK chunk2_3;
	chunk2_3.id = EDIT_CONFIG1;
	float config1 = 1.0f;
	chunk2_3.size += sizeof(float);

	// EDIT OBJECT chunk
	MAX3DS_CHUNK chunk2_4;
	chunk2_4.id = EDIT_OBJECT;

	// Model name
	wxString modName(_T("Model1\0"));
	const char *modelName = modName.c_str();
	chunk2_4.size += int(modName.size() + 1);

	// OBJ_MESH chunk
	MAX3DS_CHUNK chunk3;
	chunk3.id = OBJ_MESH;

	// Object name
	wxString objName(m->name);
	const char *objectName = objName.c_str();

	//MESH_VERTICES	0x4110 // The objects vertices
	MAX3DS_CHUNK chunk4_1;
	chunk4_1.id = MESH_VERTICES;

	// Vertice count
	// ^ numVerts
	chunk4_1.size += 2;
	// Vertices
	chunk4_1.size += (numVerts * sizeof(Vertex3f));

	//MESH_FACEL1		0x4120 // The objects faces
	MAX3DS_CHUNK chunk4_2;
	chunk4_2.id = MESH_FACEL1;

	// number of triangle faces
	// ^numIndices
	chunk4_2.size += 4;
	// Indices
	chunk4_2.size += (numFaces * sizeof(MAX3DS_FACE));
	// ------------------------------------


	// ------------------------------------
	// Correct the chunk sizes
	chunk3.size += chunk4_2.size;
	chunk3.size += chunk4_1.size;
	chunk2_2.size += chunk2_2_1.size;
	chunk2_4.size += chunk3.size;
	chunk1.size += chunk2_1.size;
	chunk1.size += chunk2_2.size;
	chunk1.size += chunk2_3.size;
	chunk1.size += chunk2_4.size;
	header.primary.size += chunk1.size;

	// Now write the data to file
	f.Write(&header, sizeof(MAX3DS_HEADER));
	f.Write(&chunk1, sizeof(MAX3DS_CHUNK));

	f.Write(&chunk2_1, sizeof(MAX3DS_CHUNK));
	f.Write(&meshVersion, sizeof(int));

	f.Write(&chunk2_2, sizeof(MAX3DS_CHUNK));

	f.Write(&chunk2_2_1, sizeof(MAX3DS_CHUNK));
	f.Write(materialName, (matName.size() + 1));

	//COL_RGB  

	f.Write(&chunk2_3, sizeof(MAX3DS_CHUNK));
	f.Write(&config1, sizeof(float));

	f.Write(&chunk2_4, sizeof(MAX3DS_CHUNK));
	f.Write(modelName, (modName.size() + 1));

	f.Write(&chunk3, sizeof(MAX3DS_CHUNK));
	//f.Write(objectName, (objName.size() + 1));

	// Mesh chunk Vertice count, and vertices
	f.Write(&chunk4_1, sizeof(MAX3DS_CHUNK));
	f.Write(&numVerts, sizeof(unsigned short));
	for (int i=0; i<numVerts; i++)
		f.Write(&verts[i], sizeof(Vertex3f));
	
	// Index chunk - Indice count, and indices
	f.Write(&chunk4_2, sizeof(MAX3DS_CHUNK));
	f.Write(&numFaces, sizeof(unsigned int));
	for (int i=0; i<numFaces; i++)
		f.Write(&tris[i], sizeof(MAX3DS_FACE));

	f.Close();

	wxDELETEA(tris);
	wxDELETEA(verts);
}

void ExportM2toMS3D(Attachment *att, const char *fn)
{
	ofstream f(fn, ios::out | ios::binary | ios::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	InitCommon(att);

	// Write the header
	ms3d_header_t header;
	strncpy(header.id, "MS3D000000", sizeof(header.id));
	header.version = 4;

	// Header
	f.write(reinterpret_cast<char *>(&header), sizeof(ms3d_header_t));
	// Vertex Count
	f.write(reinterpret_cast<char *>(&numVerts), sizeof(numVerts));
	
	// Write Vertex data?
	for (unsigned int i=0; i<numVerts; i++) {
		ms3d_vertex_t vert;
		vert.boneId = char(-1);
		vert.flags = SELECTED;
		vert.referenceCount = 0; // what the?
		vert.vertex[0] = verts[i].vertex.x;
		vert.vertex[1] = verts[i].vertex.y;
		vert.vertex[2] = verts[i].vertex.z;
		f.write(reinterpret_cast<char *>(&vert), sizeof(ms3d_vertex_t));
	}
	// ---------------------------

	// Triangle Count
	f.write(reinterpret_cast<char *>(&numFaces), 2);

	// Write Triangle Data?
	for (unsigned int i=0; i<numVerts; i+=3) {
		ms3d_triangle_t tri;
		tri.flags = SELECTED;
		tri.groupIndex = (unsigned char)verts[i].groupIndex;
		tri.smoothingGroup = 1;

		for (int j=0; j<3; j++) {
			tri.vertexIndices[j] = i+j;
			tri.s[j] = verts[i+j].tu;
			tri.t[j] = verts[i+j].tv;
			
			tri.vertexNormals[j][0] = verts[i+j].normal.x;
			tri.vertexNormals[j][1] = verts[i+j].normal.y;
			tri.vertexNormals[j][2] = verts[i+j].normal.z;
		}

		f.write(reinterpret_cast<char *>(&tri), sizeof(ms3d_triangle_t));
	}
	// ---------------------------

	// Number of groups
	f.write(reinterpret_cast<char *>(&numGroups), 2);

	unsigned short indiceCount = 0;
	for (unsigned short i=0; i<numGroups; i++) {
		wxString groupName(wxString::Format("Geoset_%i", i));

		const char flags = SELECTED;
		f.write(&flags, 1);

		char name[32];
		strncpy(name, groupName.c_str(), sizeof(name));
		f.write(name, 32);

		unsigned short faceCount = groups[i].p.indexCount / 3;
		f.write(reinterpret_cast<char *>(&faceCount), 2);
		
		for (int k=0; k<faceCount; k++) {
			//triIndices[k] = indiceCount;
			f.write(reinterpret_cast<char *>(&indiceCount), 2);
			indiceCount++;
		}

		unsigned char gIndex = (char)i;
		f.write(reinterpret_cast<char *>(&gIndex), 1);
	}

	// Number of materials (pretty much identical to groups, each group has its own material)
	f.write(reinterpret_cast<char *>(&numGroups), 2);
	
	for (unsigned short i=0; i<numGroups; i++) {
		wxString matName(wxString::Format("Material_%i", i));

		ModelRenderPass p = groups[i].p;
		if (p.init(groups[i].m)) {
			ms3d_material_t mat;
			memset(mat.alphamap, '\0', 128);
			//mat.alphamap = (char)0;
			mat.ambient[0] = 0.7f;
			mat.ambient[1] = 0.7f;
			mat.ambient[2] = 0.7f;
			mat.ambient[3] = 1.0f;
			mat.diffuse[0] = p.ocol.x;
			mat.diffuse[1] = p.ocol.y;
			mat.diffuse[2] = p.ocol.z;
			mat.diffuse[3] = p.ocol.w;
			mat.emissive[0] = p.ecol.x;
			mat.emissive[1] = p.ecol.y;
			mat.emissive[2] = p.ecol.z;
			mat.emissive[3] = p.ecol.w;

			strncpy(mat.name, matName.c_str(), sizeof(mat.name));

			if (p.useEnvMap) {
				mat.shininess = 30.0f;
				mat.mode = 1;
			} else {
				mat.shininess = 0.0f;
				mat.mode = 0;
			}
			
			mat.specular[0] = 0.0f;
			mat.specular[1] = 0.0f;
			mat.specular[2] = 0.0f;
			mat.specular[3] = 1.0f;

			unsigned int bindtex = 0;
			if (groups[i].m->specialTextures[p.tex]==-1) 
				bindtex = groups[i].m->textures[p.tex];
			else 
				bindtex = groups[i].m->replaceTextures[groups[i].m->specialTextures[p.tex]];

			wxString texName(fn);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << "_" << bindtex << ".png";
			strncpy(mat.texture, texName.c_str(), sizeof(mat.texture));
			mat.transparency = p.ocol.w;

			f.write(reinterpret_cast<char *>(&mat), sizeof(ms3d_material_t));

			wxString texFilename(fn);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}

	// save some keyframe data
	float fps = 1.0f; //m->anims[m->anim].playSpeed;
	float fCurTime = 0.0f;
	int totalFrames = 0;

	f.write(reinterpret_cast<char *>(&fps), 4);
	f.write(reinterpret_cast<char *>(&fCurTime), 4);
	f.write(reinterpret_cast<char *>(&totalFrames), 4);
	
	
	// number of joints
	unsigned short numJoints = 0; //(unsigned short)m->header.nBones;

	f.write(reinterpret_cast<char *>(&numJoints), 2);

	/*
	for (int i=0; i<numJoints; i++) {
		ms3d_joint_t joint;

		joint.flags = SELECTED;
		memset(joint.name, '\0', 32);
		memset(joint.parentName, '\0', 32);
		
		joint.rotation[0] = m->bones[i].pivot.x;
		joint.rotation[1] = m->bones[i].pivot.y;
		joint.rotation[2] = m->bones[i].pivot.z;

		joint.position[0] = m->bones[i].transPivot.x;
		joint.position[1] = m->bones[i].transPivot.y;
		joint.position[2] = m->bones[i].transPivot.z;

		joint.numKeyFramesRot = (unsigned short)m->bones[i].boneDef.rotation.nKeys;
		joint.numKeyFramesTrans = (unsigned short)m->bones[i].boneDef.translation.nKeys;

		f.write(reinterpret_cast<char *>(&joint), sizeof(ms3d_joint_t));

		if (joint.numKeyFramesRot > 0) {
			ms3d_keyframe_rot_t *keyFramesRot = new ms3d_keyframe_rot_t[joint.numKeyFramesRot];
			for (int j=0; j<joint.numKeyFramesRot; j++) {
				keyFramesRot[j].time = 0;
				keyFramesRot[j].rotation[0] = m->bones[i].rot.getValue(0,0).x;
				keyFramesRot[j].rotation[1] = m->bones[i].rot.getValue(0,0).y;
				keyFramesRot[j].rotation[2] = m->bones[i].rot.getValue(0,0).z;
			}

			f.write(reinterpret_cast<char *>(keyFramesRot), sizeof(ms3d_keyframe_rot_t) * joint.numKeyFramesRot);
			wxDELETEA(keyFramesRot);
		}

		if (joint.numKeyFramesTrans > 0) {
			ms3d_keyframe_pos_t *keyFramesTrans = new ms3d_keyframe_pos_t[joint.numKeyFramesTrans];
			for (int j=0; j<joint.numKeyFramesTrans; j++) {
				keyFramesTrans[j].time = 0;
				keyFramesTrans[j].position[0] = m->bones[i].trans.getValue(0,0).x;
				keyFramesTrans[j].position[1] = m->bones[i].trans.getValue(0,0).y;
				keyFramesTrans[j].position[2] = m->bones[i].trans.getValue(0,0).z;
			}

			f.write(reinterpret_cast<char *>(keyFramesTrans), sizeof(ms3d_keyframe_pos_t) * joint.numKeyFramesTrans);
			wxDELETEA(keyFramesTrans);
		}
	}
	*/

	f.close();

	if (verts)
		wxDELETEA(verts);
	if (groups)
		wxDELETEA(groups);
}

void ExportM2toLWO2(Attachment *att, const char *fn)
{
	/*
	int i32;
	uint32 u32;
	float f32;
	uint16 u16;
	unsigned char ub;
	*/

	ofstream f(fn, ios::out | ios::binary | ios::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	InitCommon(att);

	unsigned int fileLen = 0;

	f.write("FORM", 4);
	f.write(reinterpret_cast<char *>(&fileLen), 4);

	f.write("LWO2", 4);
	fileLen += 4;


	f.close();
}


void ExportM2toLWO(Model *m, const char *fn)
{
	int i32;
	uint32 u32;
	float f32;
	uint16 u16;
	unsigned char ub;

	ofstream f(fn, ios::out | ios::binary | ios::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	unsigned int fileLen = 0;

	f.write("FORM", 4);
	f.write(reinterpret_cast<char *>(&fileLen), 4);

	f.write("LWOB", 4);
	fileLen += 4;


	// --
	// POINTS CHUNK, this is the vertice data
	uint32 pointsSize = 0;
	f.write("PNTS", 4);
	u32 = reverse_endian<uint32>(pointsSize);
	f.write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the vertice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				vert.x = reverse_endian<float>(m->vertices[a].x);
				vert.y = reverse_endian<float>(m->vertices[a].y);
				vert.z = reverse_endian<float>(m->vertices[a].z);
				f.write(reinterpret_cast<char *>(&vert.x), 4);
				f.write(reinterpret_cast<char *>(&vert.y), 4);
				f.write(reinterpret_cast<char *>(&vert.z), 4);
				fileLen += 12;
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
	// ================


	// --
	// SURFACE CHUNK,
	uint32 surfaceSize = 0;
	f.write("SRFS", 4);
	f.write(reinterpret_cast<char *>(&surfaceSize), 4);
	fileLen += 8;

	wxString surfName;
	int surfaceCounter = 0;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			surfName = "geoset_";
			surfName << surfaceCounter;
			surfaceCounter++;
			
			surfName.Append('\0');
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append('\0');

			f.write(surfName.c_str(), (int)surfName.length());
			
			fileLen += (uint32)surfName.length();
			surfaceSize += (uint32)surfName.length();
		}
	}
	
	f.seekp(-4 - surfaceSize, ios::cur);
	u32 = reverse_endian<uint32>(surfaceSize);
	f.write(reinterpret_cast<char *>(&u32), 4);
	f.seekp(0, ios::end);
	// =================

	// --
	// POLYGON CHUNK
	int32 polySize = (numVerts / 3) * sizeof(POLYCHUNK);

	f.write("POLS", 4);
	i32 = reverse_endian<int32>(polySize);
	f.write(reinterpret_cast<char *>(&i32), 4);
	fileLen += 8; // an extra 4 bytes for chunk size

	uint16 counter=0;
	int16 surfCounter=0;
	POLYCHUNK tri;
	
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {

			surfCounter++;

			for (unsigned int k=0; k<p.indexCount; k+=3) {
				u16 = 3;
				tri.numVerts = ByteSwap16(u16);
				for (uint16 b=0; b<3; b++) {
					tri.indice[b] = ByteSwap16(counter);
					counter++;
				}
				tri.surfIndex = ByteSwap16(surfCounter);

				f.write(reinterpret_cast<char *>(&tri), sizeof(POLYCHUNK));

				fileLen += 10;
			}
		}
	}
	
	// Now lets go back and correct our data lengths
	//f.seekp(-4 - polySize, ios::cur);
	//i32 = reverse_endian<int32>(polySize);
	//f.write(reinterpret_cast<char *>(&i32), 4);
	// ========


	// Now we need to write our surface definitions (ie. RenderPass data)
	// --------------------------
	//f.seekp(0, ios::end);

	surfaceCounter = 0;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			uint32 surfaceDefSize = 0;
			f.write("SURF", 4);
			f.write(reinterpret_cast<char *>(&surfaceDefSize), 4);
			fileLen += 8;

			// Surface name
			surfName = "geoset_";
			surfName << surfaceCounter;
			surfaceCounter++;
			
			surfName.Append('\0');
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append('\0');

			f.write(surfName.data(), (int)surfName.length());
			
			fileLen += (uint32)surfName.length();
			surfaceDefSize += (uint32)surfName.length();

			// Surface Attributes
			// COLOUR, size 4, bytes 2
			f.write("COLR", 4);
			u16 = 4;
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);
			
			// value
			ub = (unsigned char)(p.ocol.x * 255);
			f.write(reinterpret_cast<char *>(&ub), 1);
			ub = (unsigned char)(p.ocol.y * 255);
			f.write(reinterpret_cast<char *>(&ub), 1);
			ub = (unsigned char)(p.ocol.z * 255);
			f.write(reinterpret_cast<char *>(&ub), 1);
			ub = '\0';
			f.write(reinterpret_cast<char *>(&ub), 1);

			fileLen += 10;
			surfaceDefSize += 10;
			
			// FLAGS
			f.write("FLAG", 4);
			// size
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			// value
			u16 = 0;
			if (!p.cull)
				u16 &= SUF_DOUBLESIDED;
			if (p.blendmode>2)
				u16 &= SUF_ADDITIVE;
			if (p.blendmode==1)
				u16 &= SUF_EDGETRANSPARENT;
			
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			fileLen += 8;
			surfaceDefSize += 8;

			// GLOSSINESS
			f.write("GLOS", 4);
			// size
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			// Value
			// try 50% gloss for 'relfection surfaces
			if (p.useEnvMap)
				u16 = 128;
			else
				u16 = 0;
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			fileLen += 8;
			surfaceDefSize += 8;
			
			if (p.useEnvMap) {
				// REFLECTION
				f.write("FRFL", 4);
				// size
				u16 = 4;
				u16 = ByteSwap16(u16);
				f.write(reinterpret_cast<char *>(&u16), 2);

				// value
				f32 = 0.2f;
				f32 = reverse_endian<float>(f32);
				f.write(reinterpret_cast<char *>(&f32), 4);

				fileLen += 10;
				surfaceDefSize += 10;
				
				// REFLECTION
				f.write("RFLT", 4);
				// size
				u16 = 2;
				u16 = ByteSwap16(u16);
				f.write(reinterpret_cast<char *>(&u16), 2);

				// value
				u16 = 1;
				u16 = ByteSwap16(u16);
				f.write(reinterpret_cast<char *>(&u16), 2);

				fileLen += 8;
				surfaceDefSize += 8;
			}
			

			// TRANSPARENCY
			f.write("FTRN", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			// value
			f32 = p.ocol.w;
			f32 = reverse_endian<float>(f32);
			f.write(reinterpret_cast<char *>(&f32), 4);

			fileLen += 10;
			surfaceDefSize += 10;

			// TEXTURE FLAGS
			f.write("TFLG", 4);
			// size
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			// value
			u16 = 0; // don't know the flag info yet
			if (p.trans)
				u16 &= TXF_PIXBLEND;
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);

			fileLen += 8;
			surfaceDefSize += 8;

			wxString texName(fn);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << "_" << p.tex << ".tga" << '\0';
			if (fmod((float)texName.length(), 2.0f) > 0)
				texName.Append('\0');

			// TEXTURE filename
			f.write("TIMG", 4);
			u16 = (unsigned short)texName.length();
			u16 = ByteSwap16(u16);
			f.write(reinterpret_cast<char *>(&u16), 2);
			f.write(texName.data(), (int)texName.length());

			fileLen += (unsigned int)texName.length() + 6;
			surfaceDefSize += (unsigned int)texName.length() + 6;

			// update the chunks length
			f.seekp(-4 - surfaceDefSize, ios::cur);
			u32 = reverse_endian<uint32>(surfaceDefSize);
			f.write(reinterpret_cast<char *>(&u32), 4);
			f.seekp(0, ios::end);

			wxString texFilename(fn);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}
	
	f.seekp(4,ios::beg);
	u32 = reverse_endian<uint32>(fileLen);
	f.write(reinterpret_cast<char *>(&u32), 4);
	
	f.seekp(16, ios::beg);
	u32 = reverse_endian<uint32>(pointsSize);
	f.write(reinterpret_cast<char *>(&u32), 4);
	// ===========================


	f.close();
}

void ExportM2toOBJ(Model *m, const char *fn)
{
	// Open file
	ofstream f(fn, ios_base::out | ios_base::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	wxString matName(fn);
	matName = matName.BeforeLast('.');
	matName << ".mtl";

	ofstream fm(matName.c_str(), ios_base::out | ios_base::trunc);
	
	// output all the vertice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			f << "# Chunk Indice Count: " << p.indexCount << endl;

			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				f << "v " << m->vertices[a].x << " " << m->vertices[a].y << " " << m->vertices[a].z << " " << "1.0" << endl;
			}

			wxString texName(fn);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << "_" << p.tex << ".tga";
			fm << "newmtl " << "Material_" << p.geoset << endl;
			fm << "Ka " << 0.7f << " " << 0.7f << " " << 0.7f << endl;
			fm << "Kd " << p.ocol.x << " " << p.ocol.y << " " << p.ocol.z << endl;
			fm << "Ks " << p.ecol.x << " " << p.ecol.y << " " << p.ecol.z << endl;
			fm << "Ns " << p.ocol.w << endl;
			fm << "map_Kd " << texName.c_str() << endl << endl;

			wxString texFilename(fn);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}

	fm.close();

	f << "mtllib " << matName.c_str() << endl;

	// output all the texture coordinate data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		// we don't want to render completely transparent parts
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				f << "vt " << m->origVertices[a].texcoords.x << " " << m->origVertices[a].texcoords.y << endl;
			}
		}
	}

	// output all the vertice normals data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				f << "vn " << m->origVertices[a].normal.x << " " << m->origVertices[a].normal.y << " " << m->origVertices[a].normal.z << endl;
			}
		}
	}

	int counter=1;
	// output the indice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			f << "g Geoset_" << p.geoset << endl;
			f << "s 1" << endl;
			f << "usemtl Material_" << p.geoset << endl;
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				f << "f " << counter << " " << counter+1 << " " << counter+2 << endl;
				counter+=3;
			}
		}
	}

	f << "# Total Indices: " << counter << endl;
	
	// Close file
	f.close();
}

void ExportM2toRAW(Model *m)
{
	
}