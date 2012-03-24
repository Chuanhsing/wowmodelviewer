#include "globalvars.h"
#include "modelviewer.h"
#include "model.h"
#include "mpq.h"

#include <cassert>
#include <algorithm>
#include "util.h"

size_t globalTime = 0;
extern ModelViewer *g_modelViewer;

void
glGetAll()
{
	GLint bled;
	wxLogMessage(wxT("glGetAll Information"));
	wxLogMessage(wxT("GL_ALPHA_TEST: %d"), glIsEnabled(GL_ALPHA_TEST));
	wxLogMessage(wxT("GL_BLEND: %d"), glIsEnabled(GL_BLEND));
	wxLogMessage(wxT("GL_CULL_FACE: %d"), glIsEnabled(GL_CULL_FACE));
	glGetIntegerv(GL_FRONT_FACE, &bled);
	if (bled == GL_CW)
	    wxLogMessage(wxT("glFrontFace: GL_CW"));
	else if (bled == GL_CCW)
	    wxLogMessage(wxT("glFrontFace: GL_CCW"));
	wxLogMessage(wxT("GL_DEPTH_TEST: %d"), glIsEnabled(GL_DEPTH_TEST));
	wxLogMessage(wxT("GL_DEPTH_WRITEMASK: %d"), glIsEnabled(GL_DEPTH_WRITEMASK));
	wxLogMessage(wxT("GL_COLOR_MATERIAL: %d"), glIsEnabled(GL_COLOR_MATERIAL));
	wxLogMessage(wxT("GL_LIGHT0: %d"), glIsEnabled(GL_LIGHT0));
	wxLogMessage(wxT("GL_LIGHT1: %d"), glIsEnabled(GL_LIGHT1));
	wxLogMessage(wxT("GL_LIGHT2: %d"), glIsEnabled(GL_LIGHT2));
	wxLogMessage(wxT("GL_LIGHT3: %d"), glIsEnabled(GL_LIGHT3));
	wxLogMessage(wxT("GL_LIGHTING: %d"), glIsEnabled(GL_LIGHTING));
	wxLogMessage(wxT("GL_TEXTURE_2D: %d"), glIsEnabled(GL_TEXTURE_2D));
	glGetIntegerv(GL_BLEND_SRC, &bled);
	wxLogMessage(wxT("GL_BLEND_SRC: 0x%x"), bled);
	glGetIntegerv(GL_BLEND_DST, &bled);
	wxLogMessage(wxT("GL_BLEND_DST: 0x%x"), bled);
}

void glInitAll()
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Vec4D(1.0f, 1.0f, 1.0f, 1.0f));
        glLightfv(GL_LIGHT0, GL_AMBIENT, Vec4D(1.0f, 1.0f, 1.0f, 1.0f));
        glLightfv(GL_LIGHT0, GL_SPECULAR, Vec4D(1.0f, 1.0f, 1.0f, 1.0f));
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_ONE, GL_ZERO);
	glFrontFace(GL_CCW);
	//glDepthMask(GL_TRUE);
	glDepthFunc(GL_NEVER);
}



AnimManager::AnimManager(ModelAnimation *anim) {
	AnimIDSecondary = -1;
	SecondaryCount = UPPER_BODY_BONES;
	AnimIDMouth = -1;
	anims = anim;
	AnimParticles = false;

	Count = 1;
	PlayIndex = 0;
	CurLoop = 0;
	animList[0].AnimID = 0;
	animList[0].Loops = 0;

	if (anims != NULL) { 
		Frame = anims[0].timeStart;
		TotalFrames = anims[0].timeEnd - anims[0].timeStart;
	} else {
		Frame = 0;
		TotalFrames = 0;
	}

	Speed = 1.0f;
	mouthSpeed = 1.0f;
	
	Paused = false;
}

AnimManager::~AnimManager() {
	anims = NULL;
}

void AnimManager::SetCount(int count)
{
	Count = count;
}

void AnimManager::AddAnim(unsigned int id, short loops) {
	if (Count > 3)
		return;

	animList[Count].AnimID = id;
	animList[Count].Loops = loops;
	Count++;
}

void AnimManager::SetAnim(short index, unsigned int id, short loops) {
	// error check, we currently only support 4 animations.
	if (index > 3)
		return;

	animList[index].AnimID = id;
	animList[index].Loops = loops;

	// Just an error check for our "auto animate"
	if (index == 0) {
		Count = 1;
		PlayIndex = index;
		Frame = anims[id].timeStart;
		TotalFrames = anims[id].timeEnd - anims[id].timeStart;
	}

	if (index+1 > Count)
		Count = index+1;
}

void AnimManager::Play() {
	PlayIndex = 0;
	//if (Frame == 0 && PlayID == 0) {
		CurLoop = animList[PlayIndex].Loops;
		Frame = anims[animList[PlayIndex].AnimID].timeStart;
		TotalFrames = GetFrameCount();
	//}

	Paused = false;
	AnimParticles = false;
}

void AnimManager::Stop() {
	Paused = true;
	PlayIndex = 0;
	Frame = anims[animList[0].AnimID].timeStart;
	CurLoop = animList[0].Loops;
}

void AnimManager::Pause(bool force) {
	if (Paused && force == false) {
		Paused = false;
		AnimParticles = !Paused;
	} else {
		Paused = true;
		AnimParticles = !Paused;
	}
}

void AnimManager::Next() {
	if(CurLoop == 1) {
		PlayIndex++;
		if (PlayIndex >= Count) {
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	} else if(CurLoop > 1) {
		CurLoop--;
	} else if(CurLoop == 0) {
		PlayIndex++;
		if (PlayIndex >= Count) {
			PlayIndex = 0;
		}
	}
	// don't change g_selModel->currentAnim in AnimManager
	//g_selModel->currentAnim = animList[PlayIndex].AnimID;
	
	Frame = anims[animList[PlayIndex].AnimID].timeStart;
	if (gameVersion >= VERSION_WOTLK)
		TotalFrames = GetFrameCount();
}

void AnimManager::Prev() {
	if(CurLoop >= animList[PlayIndex].Loops) {
		PlayIndex--;

		if (PlayIndex < 0) {
			Stop();
			return;
		}

		CurLoop = animList[PlayIndex].Loops;
	} else if(CurLoop < animList[PlayIndex].Loops) {
		CurLoop++;
	}

	Frame = anims[animList[PlayIndex].AnimID].timeEnd;
	if (gameVersion >= VERSION_WOTLK)
		TotalFrames = GetFrameCount();
}

int AnimManager::Tick(int time) {
	if((Count < PlayIndex) )
		return -1;

	Frame += int(time*Speed);

	// animate our mouth animation
	if (AnimIDMouth > -1) {
		FrameMouth += (time*mouthSpeed);

		if (FrameMouth >= anims[AnimIDMouth].timeEnd) {
			FrameMouth -= (anims[AnimIDMouth].timeEnd - anims[AnimIDMouth].timeStart);
		} else if (FrameMouth < anims[AnimIDMouth].timeStart) {
			FrameMouth += (anims[AnimIDMouth].timeEnd - anims[AnimIDMouth].timeStart);
		}
	}

	// animate our second (upper body) animation
	if (AnimIDSecondary > -1) {
		FrameSecondary += (time*Speed);

		if (FrameSecondary >= anims[AnimIDSecondary].timeEnd) {
			FrameSecondary -= (anims[AnimIDSecondary].timeEnd - anims[AnimIDSecondary].timeStart);
		} else if (FrameSecondary < anims[AnimIDSecondary].timeStart) {
			FrameSecondary += (anims[AnimIDSecondary].timeEnd - anims[AnimIDSecondary].timeStart);
		}
	}

	if (Frame >= anims[animList[PlayIndex].AnimID].timeEnd) {
		Next();
		return 1;
	} else if (Frame < anims[animList[PlayIndex].AnimID].timeStart) {
		Prev();
		return 1;
	}

	return 0;
}

size_t AnimManager::GetFrameCount() {
	return (anims[animList[PlayIndex].AnimID].timeEnd - anims[animList[PlayIndex].AnimID].timeStart);
}


void AnimManager::NextFrame()
{
	//AnimateParticles();
	ssize_t id = animList[PlayIndex].AnimID;
	Frame += ((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = ((anims[id].timeEnd - anims[id].timeStart) / 60);
}

void AnimManager::PrevFrame()
{
	//AnimateParticles();
	ssize_t id = animList[PlayIndex].AnimID;
	Frame -= ((anims[id].timeEnd - anims[id].timeStart) / 60);
	TimeDiff = ((anims[id].timeEnd - anims[id].timeStart) / 60) * -1;
}

void AnimManager::SetFrame(size_t f)
{
	//TimeDiff = f - Frame;
	Frame = f;
}

ssize_t AnimManager::GetTimeDiff()
{
	ssize_t t = TimeDiff;
	TimeDiff = 0;
	return t;
}

void AnimManager::SetTimeDiff(ssize_t i)
{
	TimeDiff = i;
}

void AnimManager::Clear() {
	Stop();
	Paused = true;
	PlayIndex = 0;
	Count = 0;
	CurLoop = 0;
	Frame = 0;
}

Model::Model(wxString name, bool forceAnim) : ManagedItem(name), forceAnim(forceAnim)
{
	if (name == wxT(""))
		return;

	// replace .MDX with .M2
	wxString tempname(name);
	tempname = tempname.BeforeLast(wxT('.')) + wxT(".m2");

	// Initiate our model variables.
	trans = 1.0f;
	rad = 1.0f;
	pos = Vec3D(0.0f, 0.0f, 0.0f);
	rot = Vec3D(0.0f, 0.0f, 0.0f);

	for (size_t i=0; i<TEXTURE_MAX; i++) {
		specialTextures[i] = -1;
		replaceTextures[i] = 0;
		useReplaceTextures[i] = false;
	}

	for (size_t i=0; i<ATT_MAX; i++) 
		attLookup[i] = -1;

	for (size_t i=0; i<BONE_MAX; i++) 
		keyBoneLookup[i] = -1;


	dlist = 0;
	bounds = 0;
	boundTris = 0;
	showGeosets = 0;

	hasCamera = false;
	hasParticles = false;
	isWMO = false;
	isMount = false;

	showModel = false;
	showBones = false;
	showBounds = false;
	showWireframe = false;
	showParticles = false;
	showTexture = true;

	charModelDetails.Reset();
	
	vbuf = nbuf = tbuf = 0;
	
	origVertices = 0;
	vertices = 0;
	normals = 0;
	texCoords = 0;
	indices = 0;
	
	animtime = 0;
	anim = 0;
	anims = 0;
	animLookups = 0;
	animManager = NULL;
	bones = 0;
	bounds = 0;
	boundTris = 0;
	currentAnim = 0;
	colors = 0;
	globalSequences = 0;
	lights = 0;
	particleSystems = 0;
	ribbons = 0;
	texAnims = 0;
	textures = 0;
	transparency = 0;
	events = 0;
	modelType = MT_NORMAL;
	IndiceToVerts = 0;
	// --

	MPQFile f(tempname);
	g_modelViewer->modelOpened->Add(tempname);
	g_modelViewer->SetStatusText(tempname);
	ok = false;
	if (f.isEof() || (f.getSize() < sizeof(ModelHeader))) {
		wxLogMessage(wxT("Error: Unable to load model: [%s]"), tempname.c_str());
		// delete this; //?
		f.close();
		return;
	}
	ok = true;
	
	memcpy(&header, f.getBuffer(), sizeof(ModelHeader));

	wxLogMessage(wxT("Loading model: %s, size: %d\n"), tempname.c_str(), f.getSize());

	//displayHeader(header);

	// Error check
	if (header.id[0] != 'M' && header.id[1] != 'D' && header.id[2] != '2' && header.id[3] != '0') {
		wxLogMessage(wxT("Error:\t\tInvalid model!  May be corrupted."));
		ok = false;
		f.close();
		return;
	}

	animated = isAnimated(f) || forceAnim;  // isAnimated will set animGeometry and animTextures

	if (gameVersion >= VERSION_WOTLK) {
		modelname = tempname;
		if (header.nameOfs != 304 && header.nameOfs != 320) {
			wxLogMessage(wxT("Error:\t\tInvalid model nameOfs=%d/%d!  May be corrupted."), header.nameOfs, sizeof(ModelHeader));
			//ok = false;
			//f.close();
			//return;
		}
	} else {
		if (header.nameOfs != 336) {
			wxLogMessage(wxT("Error:\t\tInvalid model nameOfs=%d/%d!  May be corrupted."), header.nameOfs, sizeof(ModelHeader));
			//ok = false;
			//f.close();
			//return;
		}
	}

	// Error check
	// 10 1 0 0 = WoW 5.0 models (as of 15464)
	// 10 1 0 0 = WoW 4.0.0.12319 models
	// 9 1 0 0 = WoW 4.0 models
	// 8 1 0 0 = WoW 3.0 models
	// 4 1 0 0 = WoW 2.0 models
	// 0 1 0 0 = WoW 1.0 models
	if (header.version[0] != 4 && header.version[1] != 1 && header.version[2] != 0 && header.version[3] != 0) {
		wxLogMessage(wxT("Error:\t\tModel version is incorrect!\n\t\tMake sure you are loading models from World of Warcraft 2.0.1 or newer client."));
		ok = false;
		f.close();

		if (header.version[0] == 0)
			wxMessageBox(wxString::Format(wxT("An error occured while trying to load the model %s.\nWoW Model Viewer 0.5.x only supports loading WoW 2.0 models\nModels from WoW 1.12 or earlier are not supported"), tempname.c_str()), wxT("Error: Unable to load model"), wxICON_ERROR);

		return;
	}

	if (f.getSize() < header.ofsParticleEmitters) {
		wxLogMessage(wxT("Error: Unable to load the Model \"%s\", appears to be corrupted."), tempname.c_str());
	}
	
	if (header.nGlobalSequences) {
		globalSequences = new uint32[header.nGlobalSequences];
		memcpy(globalSequences, (f.getBuffer() + header.ofsGlobalSequences), header.nGlobalSequences * sizeof(uint32));
	}

	if (forceAnim)
		animBones = true;

	if (animated)
		initAnimated(f);
	else
		initStatic(f);

	f.close();
	
	// Ready to render.
	showModel = true;
	if (hasParticles)
		showParticles = true;
	alpha = 1.0f;
}

Model::~Model()
{
	if (ok) {
#ifdef _DEBUG
		wxLogMessage(wxT("Unloading model: %s\n"), name.c_str());
#endif

		// There is a small memory leak somewhere with the textures.
		// Especially if the texture was built into the model.
		// No matter what I try though I can't find the memory to unload.
		if (header.nTextures) {

			// For character models, the texture isn't loaded into the texture manager, manually remove it
			glDeleteTextures(1, &replaceTextures[1]);
			
			// Clears textures that were loaded from Model::InitCommon()
			for (size_t i=0; i<header.nTextures; i++) {
				if (textures[i]>0)
					texturemanager.del(textures[i]);
			}

			for (size_t i=0; i<TEXTURE_MAX; i++) {
				if (replaceTextures[i] > 0)
					texturemanager.del(replaceTextures[i]);
			}
			
			wxDELETEA(textures);
		}

		wxDELETEA(globalSequences);

		wxDELETEA(bounds);
		wxDELETEA(boundTris);
		wxDELETEA(showGeosets);

		wxDELETE(animManager);

		if (animated) {
			// unload all sorts of crap
			// Need this if statement because VBO supported
			// cards have already deleted it.
			if(video.supportVBO) {
				glDeleteBuffersARB(1, &nbuf);
				glDeleteBuffersARB(1, &vbuf);
				glDeleteBuffersARB(1, &tbuf);

				vertices = NULL;
			}

			wxDELETEA(normals);
			wxDELETEA(vertices);
			wxDELETEA(texCoords);

			wxDELETEA(indices);
			wxDELETEA(anims);
			wxDELETEA(animLookups);
			wxDELETEA(origVertices);

			wxDELETEA(bones);
			wxDELETEA(texAnims);
			wxDELETEA(colors);
			wxDELETEA(transparency);
			wxDELETEA(lights);
			wxDELETEA(events);
			wxDELETEA(particleSystems);
			wxDELETEA(ribbons);
			wxDELETEA(IndiceToVerts);

		} else {
			glDeleteLists(dlist, 1);
		}
		if (g_modelViewer)
			g_modelViewer->modelOpened->Clear();
	}
}


void Model::displayHeader(ModelHeader & a_header)
{
	std::cout << "id : " << a_header.id[0] << a_header.id[1] << a_header.id[2] << a_header.id[3] << std::endl;
	std::cout << "version : " << (int)a_header.version[0] << (int)a_header.version[1] << (int)a_header.version[2] << (int)a_header.version[3] << std::endl;
	std::cout << "nameLength : " << a_header.nameLength << std::endl;
	std::cout << "nameOfs : " << a_header.nameOfs << std::endl;
	std::cout << "GlobalModelFlags : " << a_header.GlobalModelFlags << std::endl;
	std::cout << "nGlobalSequences : " << a_header.nGlobalSequences << std::endl;
	std::cout << "ofsGlobalSequences : " << a_header.ofsGlobalSequences << std::endl;
	std::cout << "nAnimations : " << a_header.nAnimations << std::endl;
	std::cout << "ofsAnimations : " << a_header.ofsAnimations << std::endl;
	std::cout << "nAnimationLookup : " << a_header.nAnimationLookup << std::endl;
	std::cout << "ofsAnimationLookup : " << a_header.ofsAnimationLookup << std::endl;
	std::cout << "nBones : " << a_header.nBones << std::endl;
	std::cout << "ofsBones : " << a_header.ofsBones << std::endl;
	std::cout << "nKeyBoneLookup : " << a_header.nKeyBoneLookup << std::endl;
	std::cout << "ofsKeyBoneLookup : " << a_header.ofsKeyBoneLookup << std::endl;
	std::cout << "nVertices : " << a_header.nVertices << std::endl;
	std::cout << "ofsVertices : " << a_header.ofsVertices << std::endl;
	std::cout << "nViews : " << a_header.nViews << std::endl;
	std::cout << "nColors : " << a_header.nColors << std::endl;
	std::cout << "ofsColors : " << a_header.ofsColors << std::endl;
	std::cout << "nTextures : " << a_header.nTextures << std::endl;
	std::cout << "ofsTextures : " << a_header.ofsTextures << std::endl;
	std::cout << "nTransparency : " << a_header.nTransparency << std::endl;
	std::cout << "ofsTransparency : " << a_header.ofsTransparency << std::endl;
	std::cout << "nTexAnims : " << a_header.nTexAnims << std::endl;
	std::cout << "ofsTexAnims : " << a_header.ofsTexAnims << std::endl;
	std::cout << "nTexReplace : " << a_header.nTexReplace << std::endl;
	std::cout << "ofsTexReplace : " << a_header.ofsTexReplace << std::endl;
	std::cout << "nTexFlags : " << a_header.nTexFlags << std::endl;
	std::cout << "ofsTexFlags : " << a_header.ofsTexFlags << std::endl;
	std::cout << "nBoneLookup : " << a_header.nBoneLookup << std::endl;
	std::cout << "ofsBoneLookup : " << a_header.ofsBoneLookup << std::endl;
	std::cout << "nTexLookup : " << a_header.nTexLookup << std::endl;
	std::cout << "ofsTexLookup : " << a_header.ofsTexLookup << std::endl;
	std::cout << "nTexUnitLookup : " << a_header.nTexUnitLookup << std::endl;
	std::cout << "ofsTexUnitLookup : " << a_header.ofsTexUnitLookup << std::endl;
	std::cout << "nTransparencyLookup : " << a_header.nTransparencyLookup << std::endl;
	std::cout << "ofsTransparencyLookup : " << a_header.ofsTransparencyLookup << std::endl;
	std::cout << "nTexAnimLookup : " << a_header.nTexAnimLookup << std::endl;
	std::cout << "ofsTexAnimLookup : " << a_header.ofsTexAnimLookup << std::endl;

//	std::cout << "collisionSphere :" << std::endl;
//	displaySphere(a_header.collisionSphere);
//	std::cout << "boundSphere :" << std::endl;
//	displaySphere(a_header.boundSphere);

	std::cout << "nBoundingTriangles : " << a_header.nBoundingTriangles << std::endl;
	std::cout << "ofsBoundingTriangles : " << a_header.ofsBoundingTriangles << std::endl;
	std::cout << "nBoundingVertices : " << a_header.nBoundingVertices << std::endl;
	std::cout << "ofsBoundingVertices : " << a_header.ofsBoundingVertices << std::endl;
	std::cout << "nBoundingNormals : " << a_header.nBoundingNormals << std::endl;
	std::cout << "ofsBoundingNormals : " << a_header.ofsBoundingNormals << std::endl;

	std::cout << "nAttachments : " << a_header.nAttachments << std::endl;
	std::cout << "ofsAttachments : " << a_header.ofsAttachments << std::endl;
	std::cout << "nAttachLookup : " << a_header.nAttachLookup << std::endl;
	std::cout << "ofsAttachLookup : " << a_header.ofsAttachLookup << std::endl;
	std::cout << "nEvents : " << a_header.nEvents << std::endl;
	std::cout << "ofsEvents : " << a_header.ofsEvents << std::endl;
	std::cout << "nLights : " << a_header.nLights << std::endl;
	std::cout << "ofsLights : " << a_header.ofsLights << std::endl;
	std::cout << "nCameras : " << a_header.nCameras << std::endl;
	std::cout << "ofsCameras : " << a_header.ofsCameras << std::endl;
	std::cout << "nCameraLookup : " << a_header.nCameraLookup << std::endl;
	std::cout << "ofsCameraLookup : " << a_header.ofsCameraLookup << std::endl;
	std::cout << "nRibbonEmitters : " << a_header.nRibbonEmitters << std::endl;
	std::cout << "ofsRibbonEmitters : " << a_header.ofsRibbonEmitters << std::endl;
	std::cout << "nParticleEmitters : " << a_header.nParticleEmitters << std::endl;
	std::cout << "ofsParticleEmitters : " << a_header.ofsParticleEmitters << std::endl;
}


bool Model::isAnimated(MPQFile &f)
{
	// see if we have any animated bones
	ModelBoneDef *bo = (ModelBoneDef*)(f.getBuffer() + header.ofsBones);

	animGeometry = false;
	animBones = false;
	ind = false;

	ModelVertex *verts = (ModelVertex*)(f.getBuffer() + header.ofsVertices);
	for (size_t i=0; i<header.nVertices && !animGeometry; i++) {
		for (size_t b=0; b<4; b++) {
			if (verts[i].weights[b]>0) {
				ModelBoneDef &bb = bo[verts[i].bones[b]];
				if (bb.translation.type || bb.rotation.type || bb.scaling.type || (bb.flags & MODELBONE_BILLBOARD)) {
					if (bb.flags & MODELBONE_BILLBOARD) {
						// if we have billboarding, the model will need per-instance animation
						ind = true;
					}
					animGeometry = true;
					break;
				}
			}
		}
	}

	if (animGeometry) 
		animBones = true;
	else {
		for (size_t i=0; i<header.nBones; i++) {
			ModelBoneDef &bb = bo[i];
			if (bb.translation.type || bb.rotation.type || bb.scaling.type) {
				animBones = true;
				animGeometry = true;
				break;
			}
		}
	}

	animTextures = header.nTexAnims > 0;

	bool animMisc = header.nCameras>0 || // why waste time, pretty much all models with cameras need animation
					header.nLights>0 || // same here
					header.nParticleEmitters>0 ||
					header.nRibbonEmitters>0;

	if (animMisc) 
		animBones = true;

	// animated colors
	if (header.nColors) {
		ModelColorDef *cols = (ModelColorDef*)(f.getBuffer() + header.ofsColors);
		for (size_t i=0; i<header.nColors; i++) {
			if (cols[i].color.type!=0 || cols[i].opacity.type!=0) {
				animMisc = true;
				break;
			}
		}
	}

	// animated opacity
	if (header.nTransparency && !animMisc) {
		ModelTransDef *trs = (ModelTransDef*)(f.getBuffer() + header.ofsTransparency);
		for (size_t i=0; i<header.nTransparency; i++) {
			if (trs[i].trans.type!=0) {
				animMisc = true;
				break;
			}
		}
	}

	// guess not...
	return animGeometry || animTextures || animMisc;
}


Vec3D fixCoordSystem(Vec3D v)
{
	return Vec3D(v.x, v.z, -v.y);
}

Vec3D fixCoordSystem2(Vec3D v)
{
	return Vec3D(v.x, v.z, v.y);
}

Quaternion fixCoordSystemQuat(Quaternion v)
{
	return Quaternion(-v.x, -v.z, v.y, v.w);
}


void Model::initCommon(MPQFile &f)
{
	// assume: origVertices already set

	// This data is needed for both VBO and non-VBO cards.
	vertices = new Vec3D[header.nVertices];
	normals = new Vec3D[header.nVertices];

	// Correct the data from the model, so that its using the Y-Up axis mode.
	for (size_t i=0; i<header.nVertices; i++) {
		origVertices[i].pos = fixCoordSystem(origVertices[i].pos);
		origVertices[i].normal = fixCoordSystem(origVertices[i].normal);

		// Set the data for our vertices, normals from the model data
		//if (!animGeometry || !supportVBO) {
			vertices[i] = origVertices[i].pos;
			normals[i] = origVertices[i].normal.normalize();
		//}

		float len = origVertices[i].pos.lengthSquared();
		if (len > rad){ 
			rad = len;
		}
	}

	// model vertex radius
	rad = sqrtf(rad);

	// bounds
	if (header.nBoundingVertices > 0) {
		bounds = new Vec3D[header.nBoundingVertices];
		Vec3D *b = (Vec3D*)(f.getBuffer() + header.ofsBoundingVertices);
		for (size_t i=0; i<header.nBoundingVertices; i++) {
			bounds[i] = fixCoordSystem(b[i]);
		}
	}
	if (header.nBoundingTriangles > 0) {
		boundTris = new uint16[header.nBoundingTriangles];
		memcpy(boundTris, f.getBuffer() + header.ofsBoundingTriangles, header.nBoundingTriangles*sizeof(uint16));
	}

	// textures
	ModelTextureDef *texdef = (ModelTextureDef*)(f.getBuffer() + header.ofsTextures);
	if (header.nTextures) {
		textures = new TextureID[header.nTextures];
		for (size_t i=0; i<header.nTextures; i++) {

			// Error check
			if (i > TEXTURE_MAX-1) {
				wxLogMessage(wxT("Critical Error: Model Texture %d over %d"), header.nTextures, TEXTURE_MAX);
				break;
			}
			/*
			Texture Types
			Texture type is 0 for regular textures, nonzero for skinned textures (filename not referenced in the M2 file!) 
			For instance, in the NightElfFemale model, her eye glow is a type 0 texture and has a file name, 
			the other 3 textures have types of 1, 2 and 6. The texture filenames for these come from client database files:

			DBFilesClient\CharSections.dbc
			DBFilesClient\CreatureDisplayInfo.dbc
			DBFilesClient\ItemDisplayInfo.dbc
			(possibly more)
				
			0	 Texture given in filename
			1	 Body + clothes
			2	Cape
			6	Hair, beard
			8	Tauren fur
			11	Skin for creatures #1
			12	Skin for creatures #2
			13	Skin for creatures #3

			Texture Flags
			Value	 Meaning
			1	Texture wrap X
			2	Texture wrap Y
			*/

			if (texdef[i].type == TEXTURE_FILENAME) {
				wxString texname((char*)(f.getBuffer()+texdef[i].nameOfs), wxConvUTF8);
				textures[i] = texturemanager.add(texname);
				TextureList.push_back(texname);
				wxLogMessage(wxT("Info: Added %s to the TextureList[%i]."), texname.c_str(), TextureList.size());
			} else {
				// special texture - only on characters and such...
				textures[i] = 0;
				//while (texdef[i].type < TEXTURE_MAX && specialTextures[texdef[i].type]!=-1) texdef[i].type++;
				//if (texdef[i].type < TEXTURE_MAX)specialTextures[texdef[i].type] = (int)i;
				specialTextures[i] = texdef[i].type;

				
				wxString tex = wxT("Special_");
				tex << texdef[i].type;

				if (modelType == MT_NORMAL){
					if (texdef[i].type == TEXTURE_HAIR)
						tex = wxT("Hair.blp");
					else if(texdef[i].type == TEXTURE_BODY)
						tex = wxT("Body.blp");
					else if(texdef[i].type == TEXTURE_FUR)
						tex = wxT("Fur.blp");
				}




				//wxString tex = modelname.BeforeLast('.').AfterLast(SLASH) + wxT("_");
				/*
				if (modelType == MT_NORMAL){
					if (texdef[i].type == TEXTURE_HAIR){
						tex += wxT("Hair.blp");
					}else if(texdef[i].type == TEXTURE_BODY){
						tex += wxT("Body.blp");
					}else if(texdef[i].type == TEXTURE_CAPE){
						tex += wxT("Cape.blp");
					}else if(texdef[i].type == TEXTURE_FUR){
						tex += wxT("Fur.blp");
					}else if(texdef[i].type == TEXTURE_ARMORREFLECT){
						tex += wxT("Reflection.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT1){
						tex += wxT("ChangableTexture1.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT2){
						tex += wxT("ChangableTexture2.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT3){
						tex += wxT("ChangableTexture3.blp");
					}else if(texdef[i].type == TEXTURE_15){
						tex += wxT("Texture15.blp");
					}else if(texdef[i].type == TEXTURE_16){
						tex += wxT("Texture16.blp");
					}else if(texdef[i].type == TEXTURE_17){
						tex += wxT("Texture17.blp");
					}else{
						tex += wxT("Unknown.blp");
					}
				}else{
					if (texdef[i].type == TEXTURE_HAIR){
						tex += wxT("NHair.blp");
					}else if(texdef[i].type == TEXTURE_BODY){
						tex += wxT("NBody.blp");
					}else if(texdef[i].type == TEXTURE_CAPE){
						tex += wxT("NCape.blp");
					}else if(texdef[i].type == TEXTURE_FUR){
						tex += wxT("NFur");
					}else if(texdef[i].type == TEXTURE_ARMORREFLECT){
						tex += wxT("NReflection.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT1){
						tex += wxT("NChangableTexture1.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT2){
						tex += wxT("NChangableTexture2.blp");
					}else if(texdef[i].type == TEXTURE_GAMEOBJECT3){
						tex += wxT("NChangableTexture3.blp");
					}else if(texdef[i].type == TEXTURE_15){
						tex += wxT("NTexture15.blp");
					}else if(texdef[i].type == TEXTURE_16){
						tex += wxT("NTexture16.blp");
					}else if(texdef[i].type == TEXTURE_17){
						tex += wxT("NTexture17.blp");
					}else{
						tex += wxT("NUnknown.blp");
					}
				}
				*/

				wxLogMessage(wxT("Info: Added %s to the TextureList[%i] via specialTextures. Type: %i"), tex.c_str(), TextureList.size(), texdef[i].type);
				TextureList.push_back(tex);

				if (texdef[i].type < TEXTURE_MAX)
					useReplaceTextures[texdef[i].type] = true;

				if (texdef[i].type == TEXTURE_ARMORREFLECT) {
					// a fix for weapons with type-3 textures.
					replaceTextures[texdef[i].type] = texturemanager.add(wxT("Item\\ObjectComponents\\Weapon\\ArmorReflect4.BLP"));
				}
			}
		}
	}
	/*
	// replacable textures - it seems to be better to get this info from the texture types
	if (header.nTexReplace) {
		size_t m = header.nTexReplace;
		if (m>16) m = 16;
		int16 *texrep = (int16*)(f.getBuffer() + header.ofsTexReplace);
		for (size_t i=0; i<m; i++) specialTextures[i] = texrep[i];
	}
	*/

	// attachments
	if (header.nAttachments) {
		ModelAttachmentDef *attachments = (ModelAttachmentDef*)(f.getBuffer() + header.ofsAttachments);
		for (size_t i=0; i<header.nAttachments; i++) {
			ModelAttachment att;
			att.model = this;
			att.init(f, attachments[i], globalSequences);
			atts.push_back(att);
		}
	}

	if (header.nAttachLookup) {
		int16 *p = (int16*)(f.getBuffer() + header.ofsAttachLookup);
		if (header.nAttachLookup > ATT_MAX)
			wxLogMessage(wxT("Critical Error: Model AttachLookup %d over %d"), header.nAttachLookup, ATT_MAX);
		for (size_t i=0; i<header.nAttachLookup; i++) {
			if (i>ATT_MAX-1)
				break;
			attLookup[i] = p[i];
		}
	}


	// init colors
	if (header.nColors) {
		colors = new ModelColor[header.nColors];
		ModelColorDef *colorDefs = (ModelColorDef*)(f.getBuffer() + header.ofsColors);
		for (size_t i=0; i<header.nColors; i++) 
			colors[i].init(f, colorDefs[i], globalSequences);
	}

	// init transparency
	if (header.nTransparency) {
		transparency = new ModelTransparency[header.nTransparency];
		ModelTransDef *trDefs = (ModelTransDef*)(f.getBuffer() + header.ofsTransparency);
		for (size_t i=0; i<header.nTransparency; i++) 
			transparency[i].init(f, trDefs[i], globalSequences);
	}

	if (header.nViews) {
		// just use the first LOD/view
		// First LOD/View being the worst?
		// TODO: Add support for selecting the LOD.
		// int viewLOD = 0; // sets LOD to worst
		// int viewLOD = header.nViews - 1; // sets LOD to best
		setLOD(f, 0); // Set the default Level of Detail to the best possible. 
	}

	// build indice to vert array.
	if (nIndices) {
		IndiceToVerts = new size_t[nIndices+2];
		for (size_t i=0;i<nIndices;i++){
			size_t a = indices[i];
			for (size_t j=0;j<header.nVertices;j++){
				if (a < header.nVertices && origVertices[a].pos == origVertices[j].pos){
					IndiceToVerts[i] = j;
					break;
				}
			}
		}
	}
	// zomg done
}

void Model::initStatic(MPQFile &f)
{
	origVertices = (ModelVertex*)(f.getBuffer() + header.ofsVertices);

	initCommon(f);

	dlist = glGenLists(1);
	glNewList(dlist, GL_COMPILE);

    drawModel();

	glEndList();

	// clean up vertices, indices etc
	wxDELETEA(vertices);
	wxDELETEA(normals);
	wxDELETEA(indices);

	wxDELETEA(colors);
	wxDELETEA(transparency);
}

void Model::initAnimated(MPQFile &f)
{
	if (origVertices) {
		delete [] origVertices;
		origVertices = NULL;
	}

	origVertices = new ModelVertex[header.nVertices];
	memcpy(origVertices, f.getBuffer() + header.ofsVertices, header.nVertices * sizeof(ModelVertex));

	initCommon(f);

	if (header.nAnimations > 0) {
		anims = new ModelAnimation[header.nAnimations];

		if (gameVersion < VERSION_WOTLK) {
			memcpy(anims, f.getBuffer() + header.ofsAnimations, header.nAnimations * sizeof(ModelAnimation));
		} else {
			// or load anim files ondemand?
			ModelAnimationWotLK animsWotLK;
			wxString tempname;
			animfiles = new MPQFile[header.nAnimations];
			for(size_t i=0; i<header.nAnimations; i++) {
				memcpy(&animsWotLK, f.getBuffer() + header.ofsAnimations + i*sizeof(ModelAnimationWotLK), sizeof(ModelAnimationWotLK));
				anims[i].animID = animsWotLK.animID;
				anims[i].timeStart = 0;
				anims[i].timeEnd = animsWotLK.length;
				anims[i].moveSpeed = animsWotLK.moveSpeed;
				anims[i].flags = animsWotLK.flags;
				anims[i].probability = animsWotLK.probability;
				anims[i].d1 = animsWotLK.d1;
				anims[i].d2 = animsWotLK.d2;
				anims[i].playSpeed = animsWotLK.playSpeed;
				anims[i].boundSphere.min = animsWotLK.boundSphere.min;
				anims[i].boundSphere.max = animsWotLK.boundSphere.max;
				anims[i].boundSphere.radius = animsWotLK.boundSphere.radius;
				anims[i].NextAnimation = animsWotLK.NextAnimation;
				anims[i].Index = animsWotLK.Index;

				tempname = wxString::Format(wxT("%s%04d-%02d.anim"), (char *)modelname.BeforeLast(wxT('.')).c_str(), anims[i].animID, animsWotLK.subAnimID);
				if (MPQFile::getSize(tempname) > 0) {
					animfiles[i].openFile(tempname);
					g_modelViewer->modelOpened->Add(tempname);
				}
			}
		}

		animManager = new AnimManager(anims);
	}
	
	if (animBones) {
		// init bones...
		bones = new Bone[header.nBones];
		ModelBoneDef *mb = (ModelBoneDef*)(f.getBuffer() + header.ofsBones);
		for (size_t i=0; i<header.nBones; i++) {
			//if (i==0) mb[i].rotation.ofsRanges = 1.0f;
			if (gameVersion >= VERSION_WOTLK) {
				bones[i].model = this;
				bones[i].initV3(f, mb[i], globalSequences, animfiles);
			} else {
				bones[i].initV2(f, mb[i], globalSequences);
			}
		}

		// Block keyBoneLookup is a lookup table for Key Skeletal Bones, hands, arms, legs, etc.
		if (header.nKeyBoneLookup < BONE_MAX) {
			memcpy(keyBoneLookup, f.getBuffer() + header.ofsKeyBoneLookup, sizeof(int16)*header.nKeyBoneLookup);
		} else {
			memcpy(keyBoneLookup, f.getBuffer() + header.ofsKeyBoneLookup, sizeof(int16)*BONE_MAX);
			wxLogMessage(wxT("Error: keyBone number [%d] over [%d]"), header.nKeyBoneLookup, BONE_MAX);
		}
	}

	if (gameVersion >= VERSION_WOTLK) {
		// free MPQFile
		if (header.nAnimations > 0) {
			for(size_t i=0; i<header.nAnimations; i++) {
				if(animfiles[i].getSize() > 0)
					animfiles[i].close();
			}
			delete [] animfiles;
		}
	}

	// Index at ofsAnimations which represents the animation in AnimationData.dbc. -1 if none.
	if (header.nAnimationLookup > 0) {
		animLookups = new int16[header.nAnimationLookup];
		memcpy(animLookups, f.getBuffer() + header.ofsAnimationLookup, sizeof(int16)*header.nAnimationLookup);
	}
	
	const size_t size = (header.nVertices * sizeof(float));
	vbufsize = (3 * size); // we multiple by 3 for the x, y, z positions of the vertex

	texCoords = new Vec2D[header.nVertices];
	for (size_t i=0; i<header.nVertices; i++) 
		texCoords[i] = origVertices[i].texcoords;

	if (video.supportVBO) {
		// Vert buffer
		glGenBuffersARB(1,&vbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbufsize, vertices, GL_STATIC_DRAW_ARB);
		wxDELETEA(vertices);
		
		// Texture buffer
		glGenBuffersARB(1,&tbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, tbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2*size, texCoords, GL_STATIC_DRAW_ARB);
		wxDELETEA(texCoords);
		
		// normals buffer
		glGenBuffersARB(1,&nbuf);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbuf);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, vbufsize, normals, GL_STATIC_DRAW_ARB);
		wxDELETEA(normals);
		
		// clean bind
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	if (animTextures) {
		texAnims = new TextureAnim[header.nTexAnims];
		ModelTexAnimDef *ta = (ModelTexAnimDef*)(f.getBuffer() + header.ofsTexAnims);
		for (size_t i=0; i<header.nTexAnims; i++)
			texAnims[i].init(f, ta[i], globalSequences);
	}

	if (header.nEvents) {
		ModelEventDef *edefs = (ModelEventDef *)(f.getBuffer()+header.ofsEvents);
		events = new ModelEvent[header.nEvents];
		for (size_t i=0; i<header.nEvents; i++) {
			events[i].init(f, edefs[i], globalSequences);
		}
	}

	// particle systems
	if (header.nParticleEmitters) {
		if (header.version[0] >= 0x10) {
			ModelParticleEmitterDefV10 *pdefsV10 = (ModelParticleEmitterDefV10 *)(f.getBuffer() + header.ofsParticleEmitters);
			ModelParticleEmitterDef *pdefs;
			particleSystems = new ParticleSystem[header.nParticleEmitters];
			hasParticles = true;
			for (size_t i=0; i<header.nParticleEmitters; i++) {
				pdefs = (ModelParticleEmitterDef *) &pdefsV10[i];
				particleSystems[i].model = this;
				particleSystems[i].init(f, *pdefs, globalSequences);
			}
		} else {
			ModelParticleEmitterDef *pdefs = (ModelParticleEmitterDef *)(f.getBuffer() + header.ofsParticleEmitters);
			particleSystems = new ParticleSystem[header.nParticleEmitters];
			hasParticles = true;
			for (size_t i=0; i<header.nParticleEmitters; i++) {
				particleSystems[i].model = this;
				particleSystems[i].init(f, pdefs[i], globalSequences);
			}
		}
	}

	// ribbons
	if (header.nRibbonEmitters) {
		ModelRibbonEmitterDef *rdefs = (ModelRibbonEmitterDef *)(f.getBuffer() + header.ofsRibbonEmitters);
		ribbons = new RibbonEmitter[header.nRibbonEmitters];
		for (size_t i=0; i<header.nRibbonEmitters; i++) {
			ribbons[i].model = this;
			ribbons[i].init(f, rdefs[i], globalSequences);
		}
	}

	// Cameras
	if (header.nCameras>0) {
		if (header.version[0] <= 9){
			ModelCameraDef *camDefs = (ModelCameraDef*)(f.getBuffer() + header.ofsCameras);
			for (size_t i=0;i<header.nCameras;i++){
				ModelCamera a;
				a.init(f, camDefs[i], globalSequences, modelname);
				cam.push_back(a);
			}
		}else if (header.version[0] <= 16){
			ModelCameraDefV10 *camDefs = (ModelCameraDefV10*)(f.getBuffer() + header.ofsCameras);
			for (size_t i=0;i<header.nCameras;i++){
				ModelCamera a;
				a.initv10(f, camDefs[i], globalSequences, modelname);
				cam.push_back(a);
			}
		}
		if (cam.size() > 0){
			hasCamera = true;
		}
	}

	// init lights
	if (header.nLights) {
		lights = new ModelLight[header.nLights];
		ModelLightDef *lDefs = (ModelLightDef*)(f.getBuffer() + header.ofsLights);
		for (size_t i=0; i<header.nLights; i++) {
			lights[i].init(f, lDefs[i], globalSequences);
		}
	}

	animcalc = false;
}

void Model::setLOD(MPQFile &f, int index)
{
	// Texture definitions
	ModelTextureDef *texdef = (ModelTextureDef*)(f.getBuffer() + header.ofsTextures);

	// Transparency
	int16 *transLookup = (int16*)(f.getBuffer() + header.ofsTransparencyLookup);

	// I thought the view controlled the Level of detail,  but that doesn't seem to be the case.
	// Seems to only control the render order.  Which makes this function useless and not needed :(
#ifdef WotLK
	// remove suffix .M2
	lodname = modelname.BeforeLast(wxT('.')) + wxString::Format(wxT("%02d.skin"), index); // Lods: 00, 01, 02, 03
	MPQFile g(lodname);
	g_modelViewer->modelOpened->Add(lodname);
	if (g.isEof()) {
		wxLogMessage(wxT("Error: Unable to load Lods: [%s]"), lodname.c_str());
		g.close();
		return;
	}

	ModelView *view = (ModelView*)(g.getBuffer());

	if (view->id[0] != 'S' || view->id[1] != 'K' || view->id[2] != 'I' || view->id[3] != 'N') {
		wxLogMessage(wxT("Error: Unable to load Lods: [%s]"), lodname.c_str());
		g.close();
		return;
	}

	// Indices,  Triangles
	uint16 *indexLookup = (uint16*)(g.getBuffer() + view->ofsIndex);
	uint16 *triangles = (uint16*)(g.getBuffer() + view->ofsTris);
	nIndices = view->nTris;
	wxDELETEA(indices);
	indices = new uint16[nIndices];
	for (size_t i = 0; i<nIndices; i++) {
        indices[i] = indexLookup[triangles[i]];
	}

	// render ops
	ModelGeoset *ops = (ModelGeoset*)(g.getBuffer() + view->ofsSub);
	ModelTexUnit *tex = (ModelTexUnit*)(g.getBuffer() + view->ofsTex);
#else // not WotLK
	ModelView *view = (ModelView*)(f.getBuffer() + header.ofsViews);

	// Indices,  Triangles
	uint16 *indexLookup = (uint16*)(f.getBuffer() + view->ofsIndex);
	uint16 *triangles = (uint16*)(f.getBuffer() + view->ofsTris);
	nIndices = view->nTris;
	wxDELETE(indices);
	indices = new uint16[nIndices];
	for (size_t i = 0; i<nIndices; i++) {
        indices[i] = indexLookup[triangles[i]];
	}

	// render ops
	ModelGeoset *ops = (ModelGeoset*)(f.getBuffer() + view->ofsSub);
	ModelTexUnit *tex = (ModelTexUnit*)(f.getBuffer() + view->ofsTex);

#endif // WotLK
	/*
	{
		std::cout << "flags = " << hex << tex->flags << std::endl;
		std::cout << "shading = " << hex << tex->shading << std::endl;
		std::cout << "op = " << tex->op << std::endl;
		std::cout << "op2 = " << tex->op2 << std::endl;
		std::cout << "colorIndex = " << tex->colorIndex << std::endl;
		std::cout << "flagsIndex = " << hex << tex->flagsIndex << std::endl;
		std::cout << "texunit = " << tex->texunit << std::endl;
		std::cout << "mode = " << tex->mode << std::endl;
		std::cout << "textureid = " << tex->textureid << std::endl;
		std::cout << "texunit2 = " << tex->texunit2 << std::endl;
		std::cout << "transid = " << tex->transid << std::endl;
		std::cout << "texanimid = " << tex->texanimid << std::endl;
	}


	{
		std::cout << "=== ModelView ===" << std::endl;
#ifdef WotLK
		std::cout << "id : " << view->id[0] << view->id[1] << view->id[2] << view->id[3] << std::endl;
#endif
		std::cout << "nIndex : " << view->nIndex << std::endl;
		std::cout << "ofsIndex : " << view->ofsIndex << std::endl;
		std::cout << "nTris : " << view->nTris << std::endl;
		std::cout << "ofsTris : " << view->ofsTris << std::endl;
		std::cout << "nProps : " << view->nProps << std::endl;
		std::cout << "ofsProps : " << view->ofsProps << std::endl;
		std::cout << "nSub : " << view->nSub << std::endl;
		std::cout << "ofsSub : " << view->ofsSub << std::endl;
		std::cout << "nTex : " << view->nTex << std::endl;
		std::cout << "ofsTex : " << view->ofsTex << std::endl;
		std::cout << "lod : " << view->lod << std::endl;
	}
*/
	ModelRenderFlags *renderFlags = (ModelRenderFlags*)(f.getBuffer() + header.ofsTexFlags);
	uint16 *texlookup = (uint16*)(f.getBuffer() + header.ofsTexLookup);
	uint16 *texanimlookup = (uint16*)(f.getBuffer() + header.ofsTexAnimLookup);
	int16 *texunitlookup = (int16*)(f.getBuffer() + header.ofsTexUnitLookup);

	wxDELETEA(showGeosets);
	showGeosets = new bool[view->nSub];


	for (size_t i=0; i<view->nSub; i++) {
		geosets.push_back(ops[i]);
		showGeosets[i] = true;
	}

	passes.clear();
	for (size_t j = 0; j<view->nTex; j++) {
		ModelRenderPass pass;

		pass.useTex2 = false;
		pass.useEnvMap = false;
		pass.cull = false;
		pass.trans = false;
		pass.unlit = false;
		pass.noZWrite = false;
		pass.billboard = false;
		pass.texanim = -1; // no texture animation

		//pass.texture2 = 0;
		size_t geoset = tex[j].op;
		
		pass.geoset = (int)geoset;

		pass.indexStart = ops[geoset].istart;
		pass.indexCount = ops[geoset].icount;
		pass.vertexStart = ops[geoset].vstart;
		pass.vertexEnd = pass.vertexStart + ops[geoset].vcount;

		//TextureID texid = textures[texlookup[tex[j].textureid]];
		//pass.texture = texid;
		pass.tex = texlookup[tex[j].textureid];

		// TODO: figure out these flags properly -_-
		ModelRenderFlags &rf = renderFlags[tex[j].flagsIndex];
		
		pass.blendmode = rf.blend;
		//if (rf.blend == 0) // Test to disable/hide different blend types
		//	continue;

		pass.color = tex[j].colorIndex;
		pass.opacity = transLookup[tex[j].transid];

		pass.unlit = (rf.flags & RENDERFLAGS_UNLIT) != 0;

		pass.cull = (rf.flags & RENDERFLAGS_TWOSIDED) == 0;

		pass.billboard = (rf.flags & RENDERFLAGS_BILLBOARD) != 0;

		// Use environmental reflection effects?
		pass.useEnvMap = (texunitlookup[tex[j].texunit] == -1) && pass.billboard && rf.blend>2; //&& rf.blend<5;

		// Disable environmental mapping if its been unchecked.
		if (pass.useEnvMap && !video.useEnvMapping)
			pass.useEnvMap = false;

		pass.noZWrite = (rf.flags & RENDERFLAGS_ZBUFFERED) != 0;

		// ToDo: Work out the correct way to get the true/false of transparency
		pass.trans = (pass.blendmode>0) && (pass.opacity>0);	// Transparency - not the correct way to get transparency

		pass.p = ops[geoset].BoundingBox[0].z;

		// Texture flags
		pass.swrap = (texdef[pass.tex].flags & TEXTURE_WRAPX) != 0; // Texture wrap X
		pass.twrap = (texdef[pass.tex].flags & TEXTURE_WRAPY) != 0; // Texture wrap Y
		
		// tex[j].flags: Usually 16 for static textures, and 0 for animated textures.	
		if (animTextures && (tex[j].flags & TEXTUREUNIT_STATIC) == 0) {
			pass.texanim = texanimlookup[tex[j].texanimid];
		}

		passes.push_back(pass);
	}

#ifdef WotLK
	g.close();
#endif
	// transparent parts come later
	//std::sort(passes.begin(), passes.end());
}

void Model::calcBones(ssize_t anim, size_t time)
{
	// Reset all bones to 'false' which means they haven't been animated yet.
	for (size_t i=0; i<header.nBones; i++) {
		bones[i].calc = false;
	}

	// Character specific bone animation calculations.
	if (charModelDetails.isChar) {	

		// Animate the "core" rotations and transformations for the rest of the model to adopt into their transformations
		if (keyBoneLookup[BONE_ROOT] > -1)	{
			for (size_t i=0; i<=keyBoneLookup[BONE_ROOT]; i++) {
				bones[i].calcMatrix(bones, anim, time);
			}
		}

		// Find the close hands animation id
		int closeFistID = 0;
		/*
		for (size_t i=0; i<header.nAnimations; i++) {
			if (anims[i].animID==15) {  // closed fist
				closeFistID = i;
				break;
			}
		}
		*/
		// Alfred 2009.07.23 use animLookups to speedup
		if (header.nAnimationLookup >= ANIMATION_HANDSCLOSED && animLookups[ANIMATION_HANDSCLOSED] > 0) // closed fist
			closeFistID = animLookups[ANIMATION_HANDSCLOSED];

		// Animate key skeletal bones except the fingers which we do later.
		// -----
		size_t a, t;

		// if we have a "secondary animation" selected,  animate upper body using that.
		if (animManager->GetSecondaryID() > -1) {
			a = animManager->GetSecondaryID();
			t = animManager->GetSecondaryFrame();
		} else {
			a = anim;
			t = time;
		}

		for (size_t i=0; i<animManager->GetSecondaryCount(); i++) { // only goto 5, otherwise it affects the hip/waist rotation for the lower-body.
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].calcMatrix(bones, a, t);
		}

		if (animManager->GetMouthID() > -1) {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].calcMatrix(bones, animManager->GetMouthID(), animManager->GetMouthFrame());
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].calcMatrix(bones, animManager->GetMouthID(), animManager->GetMouthFrame());
		} else {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].calcMatrix(bones, a, t);
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].calcMatrix(bones, a, t);
		}

		// still not sure what 18-26 bone lookups are but I think its more for things like wrist, etc which are not as visually obvious.
		for (size_t i=BONE_BTH; i<BONE_MAX; i++) {
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].calcMatrix(bones, a, t);
		}
		// =====

		if (charModelDetails.closeRHand) {
			a = closeFistID;
			t = anims[closeFistID].timeStart+1;
		} else {
			a = anim;
			t = time;
		}

		for (size_t i=0; i<5; i++) {
			if (keyBoneLookup[BONE_RFINGER1 + i] > -1) 
				bones[keyBoneLookup[BONE_RFINGER1 + i]].calcMatrix(bones, a, t);
		}

		if (charModelDetails.closeLHand) {
			a = closeFistID;
			t = anims[closeFistID].timeStart+1;
		} else {
			a = anim;
			t = time;
		}

		for (size_t i=0; i<5; i++) {
			if (keyBoneLookup[BONE_LFINGER1 + i] > -1)
				bones[keyBoneLookup[BONE_LFINGER1 + i]].calcMatrix(bones, a, t);
		}
	} else {
		for (ssize_t i=0; i<keyBoneLookup[BONE_ROOT]; i++) {
			bones[i].calcMatrix(bones, anim, time);
		}

		// The following line fixes 'mounts' in that the character doesn't get rotated, but it also screws up the rotation for the entire model :(
		//bones[18].calcMatrix(bones, anim, time, false);

		// Animate key skeletal bones except the fingers which we do later.
		// -----
		size_t a, t;

		// if we have a "secondary animation" selected,  animate upper body using that.
		if (animManager->GetSecondaryID() > -1) {
			a = animManager->GetSecondaryID();
			t = animManager->GetSecondaryFrame();
		} else {
			a = anim;
			t = time;
		}

		for (size_t i=0; i<animManager->GetSecondaryCount(); i++) { // only goto 5, otherwise it affects the hip/waist rotation for the lower-body.
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].calcMatrix(bones, a, t);
		}

		if (animManager->GetMouthID() > -1) {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].calcMatrix(bones, animManager->GetMouthID(), animManager->GetMouthFrame());
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].calcMatrix(bones, animManager->GetMouthID(), animManager->GetMouthFrame());
		} else {
			// Animate the head and jaw
			if (keyBoneLookup[BONE_HEAD] > -1)
					bones[keyBoneLookup[BONE_HEAD]].calcMatrix(bones, a, t);
			if (keyBoneLookup[BONE_JAW] > -1)
					bones[keyBoneLookup[BONE_JAW]].calcMatrix(bones, a, t);
		}

		// still not sure what 18-26 bone lookups are but I think its more for things like wrist, etc which are not as visually obvious.
		for (size_t i=BONE_ROOT; i<BONE_MAX; i++) {
			if (keyBoneLookup[i] > -1)
				bones[keyBoneLookup[i]].calcMatrix(bones, a, t);
		}
	}

	// Animate everything thats left with the 'default' animation
	for (size_t i=0; i<header.nBones; i++) {
		bones[i].calcMatrix(bones, anim, time);
	}
}

void Model::animate(ssize_t anim)
{
	size_t t=0;
	
	ModelAnimation &a = anims[anim];
	int tmax = (a.timeEnd-a.timeStart);
	if (tmax==0) 
		tmax = 1;

	if (isWMO == true) {
		t = globalTime;
		t %= tmax;
		t += a.timeStart;
	} else
		t = animManager->GetFrame();
	
	this->animtime = t;
	this->anim = anim;

	if (animBones) // && (!animManager->IsPaused() || !animManager->IsParticlePaused()))
		calcBones(anim, t);

	if (animGeometry) {

		if (video.supportVBO)	{
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2*vbufsize, NULL, GL_STREAM_DRAW_ARB);
			vertices = (Vec3D*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY);

			// Something has been changed in the past couple of days that is causing nasty bugs
			// this is an extra error check to prevent the program from crashing.
			if (!vertices) {
				wxLogMessage(wxT("Critical Error: void Model::animate(int anim), Vertex Buffer is null"));
				return;
			}
		}

		// transform vertices
		ModelVertex *ov = origVertices;
		for (size_t i=0; i<header.nVertices; ++i,++ov) { //,k=0
			Vec3D v(0,0,0), n(0,0,0);

			for (size_t b=0; b<4; b++) {
				if (ov->weights[b]>0) {
					Vec3D tv = bones[ov->bones[b]].mat * ov->pos;
					Vec3D tn = bones[ov->bones[b]].mrot * ov->normal;
					v += tv * ((float)ov->weights[b] / 255.0f);
					n += tn * ((float)ov->weights[b] / 255.0f);
				}
			}

			vertices[i] = v;
			if (video.supportVBO)
				vertices[header.nVertices + i] = n.normalize(); // shouldn't these be normal by default?
			else
				normals[i] = n;
		}

		// clear bind
        if (video.supportVBO) {
			glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
		}
	}

	for (size_t i=0; i<header.nLights; i++) {
		if (lights[i].parent>=0) {
			lights[i].tpos = bones[lights[i].parent].mat * lights[i].pos;
			lights[i].tdir = bones[lights[i].parent].mrot * lights[i].dir;
		}
	}

	for (size_t i=0; i<header.nParticleEmitters; i++) {
		// random time distribution for teh win ..?
		//int pt = a.timeStart + (t + (int)(tmax*particleSystems[i].tofs)) % tmax;
		particleSystems[i].setup(anim, t);
	}

	for (size_t i=0; i<header.nRibbonEmitters; i++) {
		ribbons[i].setup(anim, t);
	}

	if (animTextures) {
		for (size_t i=0; i<header.nTexAnims; i++) {
			texAnims[i].calc(anim, t);
		}
	}
}


bool ModelRenderPass::init(Model *m)
{
	// May aswell check that we're going to render the geoset before doing all this crap.
	if (!m->showGeosets[geoset])
		return false;

	// COLOUR
	// Get the colour and transparency and check that we should even render
	ocol = Vec4D(1.0f, 1.0f, 1.0f, m->trans);
	ecol = Vec4D(0.0f, 0.0f, 0.0f, 0.0f);

	//if (m->trans == 1.0f)
	//	return false;

	// emissive colors
	if (color!=-1 && m->colors && m->colors[color].color.uses(0)) {
		Vec3D c;
		if (gameVersion >= VERSION_WOTLK) {
			/* Alfred 2008.10.02 buggy opacity make model invisable, TODO */
			c = m->colors[color].color.getValue(0,m->animtime);
			if (m->colors[color].opacity.uses(m->anim)) {
				ocol.w = m->colors[color].opacity.getValue(m->anim,m->animtime);
			}
		} else {
			c = m->colors[color].color.getValue(m->anim,m->animtime);
			ocol.w = m->colors[color].opacity.getValue(m->anim,m->animtime);
		}

		if (unlit) {
			ocol.x = c.x; ocol.y = c.y; ocol.z = c.z;
		} else {
			ocol.x = ocol.y = ocol.z = 0;
		}

		ecol = Vec4D(c, ocol.w);
		glMaterialfv(GL_FRONT, GL_EMISSION, ecol);
	}

	// opacity
	if (opacity!=-1) {
		if (gameVersion >= VERSION_WOTLK) {
			/* Alfred 2008.10.02 buggy opacity make model invisable, TODO */
			if (m->transparency && m->transparency[opacity].trans.uses(0))
				ocol.w *= m->transparency[opacity].trans.getValue(0, m->animtime);
		} else
			ocol.w *= m->transparency[opacity].trans.getValue(m->anim, m->animtime);
	}

	// exit and return false before affecting the opengl render state
	if (!((ocol.w > 0) && (color==-1 || ecol.w > 0)))
		return false;


	// TEXTURE
	// bind to our texture
	GLuint bindtex = 0;
	if (m->specialTextures[tex]==-1) 
		bindtex = m->textures[tex];
	else 
		bindtex = m->replaceTextures[m->specialTextures[tex]];

	glBindTexture(GL_TEXTURE_2D, bindtex);
	// --

	// TODO: Add proper support for multi-texturing.

	// ALPHA BLENDING
	// blend mode
	switch (blendmode) {
	case BM_OPAQUE:	// 0
		break;
	case BM_TRANSPARENT: // 1
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL,0.7f);
		
		/*
		// Tex settings
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
		*/
		
		break;
	case BM_ALPHA_BLEND: // 2
		//glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default blend func
		break;
	case BM_ADDITIVE: // 3
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		break;
	case BM_ADDITIVE_ALPHA: // 4
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		/*
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
		*/
		
		break;
	case BM_MODULATE:	// 5
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
		
		/*
		// Texture settings.
		glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_INTERPOLATE);
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		*/

		break;
	case BM_MODULATEX2:	// 6, not sure if this is right
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);	
		
		/*
		// Texture settings.
		glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_INTERPOLATE);
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		*/
		
		break;
	default:
		wxLogMessage(wxT("[Error] Unknown blendmode: %d\n"), blendmode);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
	}

	if (cull) {
		glEnable(GL_CULL_FACE);
	}

	// Texture wrapping around the geometry
	if (swrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	if (twrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// no writing to the depth buffer.
	if (noZWrite)
		glDepthMask(GL_FALSE);

	// Environmental mapping, material, and effects
	if (useEnvMap) {
		// Turn on the 'reflection' shine, using 18.0f as that is what WoW uses based on the reverse engineering
		// This is now set in InitGL(); - no need to call it every render.
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0f);

		// env mapping
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);

		const GLint maptype = GL_SPHERE_MAP;
		//const GLint maptype = GL_REFLECTION_MAP_ARB;

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, maptype);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, maptype);
	}

	if (texanim!=-1) {
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

		m->texAnims[texanim].setup(texanim);
	}

	// color
	glColor4fv(ocol);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, ocol);

	// don't use lighting on the surface
	if (unlit) {
		glDisable(GL_LIGHTING);
	}

	if (blendmode<=1 && ocol.w<1.0f)
		glEnable(GL_BLEND);

	return true;
}

void ModelRenderPass::deinit()
{

	switch (blendmode) {
	case BM_OPAQUE:
		break;
	case BM_TRANSPARENT:
		glDisable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GEQUAL,0.04f);

		/*
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
		glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE,1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		*/

		break;
	
	case BM_ALPHA_BLEND:
		glDisable(GL_BLEND);
		break;
	case BM_ADDITIVE: // 3
 		glDisable(GL_BLEND);
		break;
	case BM_ADDITIVE_ALPHA: // 4
		//glDisable(GL_ALPHA_TEST);
 		glDisable(GL_BLEND);
		break;
	
	case BM_MODULATE: // 5
		glDisable(GL_BLEND);
		break;

	case BM_MODULATEX2: // 6
		glDisable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	default:
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default blend func

		/*
		// Default texture settings.
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
		glTexEnvf(GL_TEXTURE_ENV,GL_RGB_SCALE, 1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvf(GL_TEXTURE_ENV,GL_ALPHA_SCALE, 1.000000);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_ALPHA, GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA, GL_REPLACE);
		*/
	}


	if (noZWrite)
		glDepthMask(GL_TRUE);

	if (texanim!=-1) {
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	if (unlit) {
		glEnable(GL_LIGHTING);
	}

	//if (billboard)
	//	glPopMatrix();

	if (cull)
		glDisable(GL_CULL_FACE);

	if (useEnvMap) {
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
	}

	if (swrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	if (twrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/*
	if (useTex2) {
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0);
	}
	*/

	if (opacity!=-1 || color!=-1) {
		GLfloat czero[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		glMaterialfv(GL_FRONT, GL_EMISSION, czero);
		
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		//glMaterialfv(GL_FRONT, GL_AMBIENT, ocol);
		//ocol = Vec4D(1.0f, 1.0f, 1.0f, 1.0f);
		//glMaterialfv(GL_FRONT, GL_DIFFUSE, ocol);
	}
}


inline void Model::drawModel()
{
	// assume these client states are enabled: GL_VERTEX_ARRAY, GL_NORMAL_ARRAY, GL_TEXTURE_COORD_ARRAY
	if (video.supportVBO && animated)	{
		// bind / point to the vertex normals buffer
		if (animGeometry) {
			glNormalPointer(GL_FLOAT, 0, GL_BUFFER_OFFSET(vbufsize));
		} else {
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, nbuf);
			glNormalPointer(GL_FLOAT, 0, 0);
		}

		// Bind the vertex buffer
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbuf);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		// Bind the texture coordinates buffer
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, tbuf);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
		
	} else if (animated) {
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glNormalPointer(GL_FLOAT, 0, normals);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	}
	
	// Display in wireframe mode?
	if (showWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// Render the various parts of the model.
	for (size_t i=0; i<passes.size(); i++) {
		ModelRenderPass &p = passes[i];

		if (p.init(this)) {
			// we don't want to render completely transparent parts

			// render
			if (animated) {
				//glDrawElements(GL_TRIANGLES, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart);
				// a GDC OpenGL Performace Tuning paper recommended glDrawRangeElements over glDrawElements
				// I can't notice a difference but I guess it can't hurt
				if (video.supportVBO && video.supportDrawRangeElements) {
					glDrawRangeElements(GL_TRIANGLES, p.vertexStart, p.vertexEnd, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart);
				
				//} else if (!video.supportVBO) {
				//	glDrawElements(GL_TRIANGLES, p.indexCount, GL_UNSIGNED_SHORT, indices + p.indexStart); 
				} else {
					glBegin(GL_TRIANGLES);
					for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
						uint16 a = indices[b];
						glNormal3fv(normals[a]);
						glTexCoord2fv(origVertices[a].texcoords);
						glVertex3fv(vertices[a]);
					}
					glEnd();
				}
			} else {
				glBegin(GL_TRIANGLES);
				for (size_t k = 0, b=p.indexStart; k<p.indexCount; k++,b++) {
					uint16 a = indices[b];
					glNormal3fv(normals[a]);
					glTexCoord2fv(origVertices[a].texcoords);
					glVertex3fv(vertices[a]);
				}
				glEnd();
			}

			p.deinit();
		}
	}
	
	if (showWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// clean bind
	if (video.supportVBO && animated) {
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}
	// done with all render ops
}

void TextureAnim::calc(ssize_t anim, size_t time)
{
	if (trans.uses(anim)) {
		tval = trans.getValue(anim, time);
	}
	if (rot.uses(anim)) {
        rval = rot.getValue(anim, time);
	}
	if (scale.uses(anim)) {
       	sval = scale.getValue(anim, time);
	}
}

void TextureAnim::setup(ssize_t anim)
{
	glLoadIdentity();
	if (trans.uses(anim)) {
		glTranslatef(tval.x, tval.y, tval.z);
	}
	if (rot.uses(anim)) {
		glRotatef(rval.x, 0, 0, 1); // this is wrong, I have no idea what I'm doing here ;)
	}
	if (scale.uses(anim)) {
		glScalef(sval.x, sval.y, sval.z);
	}
}

void ModelCamera::init(MPQFile &f, ModelCameraDef &mcd, uint32 *global, wxString modelname)
{
	wxLogMessage(wxT("Using original Camera Model Definitions."));
	ok = true;
    nearclip = mcd.nearclip;
	farclip = mcd.farclip;
	fov = mcd.fov;
	pos = fixCoordSystem(mcd.pos);
	target = fixCoordSystem(mcd.target);
	tPos.init(mcd.transPos, f, global);
	tTarget.init(mcd.transTarget, f, global);
	rot.init(mcd.rot, f, global);
	tPos.fix(fixCoordSystem);
	tTarget.fix(fixCoordSystem);

	Vec3D wopos = Vec3D(0,0,0);
	float worot = 0.0f;
	if (modelname.Find(wxT("Cameras\\"))>-1) {
		try {
			wxLogMessage(wxT("Trying Camera DB..."));
			wxString mn = modelname.BeforeLast(wxT('.')) + wxT(".mdx");
			wxLogMessage(wxT("ModelName: %s"), mn.c_str());
			CamCinematicDB::Record r = camcinemadb.getByCamModel(mn.c_str());
			wxLogMessage(wxT("Setting variables.."));
			wopos = fixCoordSystem(Vec3D(r.getFloat(camcinemadb.PosX),r.getFloat(camcinemadb.PosY),r.getFloat(camcinemadb.PosZ)));
			worot = r.getFloat(camcinemadb.Rot);
		}
		catch (CamCinematicDB::NotFound) {
			wxLogMessage(wxT("DBFilesClient/CinematicCamera.dbc not found."));
			wopos = Vec3D(0,0,0);
			worot = 0.0f;
		}
		wxLogMessage(wxT("WorldPos: %f, %f, %f"), wopos.x, wopos.y, wopos.z);
		wxLogMessage(wxT("WorldRot: %f"), worot);
	}
	WorldOffset = fixCoordSystem(wopos);
	WorldRotation = worot;
}

void ModelCamera::initv10(MPQFile &f, ModelCameraDefV10 &mcd, uint32 *global, wxString modelname)
{
	wxLogMessage(wxT("Using version 10 Camera Model Definitions."));
	ok = true;
    nearclip = mcd.nearclip;
	farclip = mcd.farclip;
	pos = fixCoordSystem(mcd.pos);
	target = fixCoordSystem(mcd.target);
	tPos.init(mcd.transPos, f, global);
	tTarget.init(mcd.transTarget, f, global);
	rot.init(mcd.rot, f, global);
	tPos.fix(fixCoordSystem);
	tTarget.fix(fixCoordSystem);
	fov = 0.95f;

	Vec3D wopos = Vec3D(0,0,0);
	float worot = 0.0f;
	if (modelname.Find(wxT("Cameras\\"))>-1) {
		try {
			wxLogMessage(wxT("Trying Camera DB..."));
			wxString mn = modelname.BeforeLast(wxT('.')) + wxT(".mdx");
			wxLogMessage(wxT("ModelName: %s"), mn.c_str());
			CamCinematicDB::Record r = camcinemadb.getByCamModel(mn.c_str());
			wxLogMessage(wxT("Setting variables.."));
			wopos = fixCoordSystem(Vec3D(r.getFloat(camcinemadb.PosX),r.getFloat(camcinemadb.PosY),r.getFloat(camcinemadb.PosZ)));
			worot = r.getFloat(camcinemadb.Rot);
		}
		catch (CamCinematicDB::NotFound) {
			wxLogMessage(wxT("DBFilesClient/CinematicCamera.dbc not found."));
			wopos = Vec3D(0,0,0);
			worot = 0.0f;
		}
		wxLogMessage(wxT("WorldPos: %f, %f, %f"), wopos.x, wopos.y, wopos.z);
		wxLogMessage(wxT("WorldRot: %f"), worot);
	}
	WorldOffset = fixCoordSystem(wopos);
	WorldRotation = worot;
}

void ModelCamera::setup(size_t time)
{
	if (!ok) return;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov * 34.5f, (GLfloat)video.xRes/(GLfloat)video.yRes, nearclip, farclip*5);

	Vec3D p = pos + tPos.getValue(0, time);
	Vec3D t = target + tTarget.getValue(0, time);

	Vec3D u(0,1,0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(p.x, p.y, p.z, t.x, t.y, t.z, u.x, u.y, u.z);
	//float roll = rot.getValue(0, time) / PI * 180.0f;
	//glRotatef(roll, 0, 0, 1);
}

void ModelColor::init(MPQFile &f, ModelColorDef &mcd, uint32 *global)
{
	color.init(mcd.color, f, global);
	opacity.init(mcd.opacity, f, global);
}

void ModelTransparency::init(MPQFile &f, ModelTransDef &mcd, uint32 *global)
{
	trans.init(mcd.trans, f, global);
}

void ModelLight::init(MPQFile &f, ModelLightDef &mld, uint32 *global)
{
	tpos = pos = fixCoordSystem(mld.pos);
	tdir = dir = Vec3D(0,1,0); // no idea
	type = mld.type;
	parent = mld.bone;
	ambColor.init(mld.ambientColor, f, global);
	ambIntensity.init(mld.ambientIntensity, f, global);
	diffColor.init(mld.diffuseColor, f, global);
	diffIntensity.init(mld.diffuseIntensity, f, global);
	AttenStart.init(mld.attenuationStart, f, global);
	AttenEnd.init(mld.attenuationEnd, f, global);
	UseAttenuation.init(mld.useAttenuation, f, global);
}

void ModelLight::setup(size_t time, GLuint l)
{
	Vec4D ambcol(ambColor.getValue(0, time) * ambIntensity.getValue(0, time), 1.0f);
	Vec4D diffcol(diffColor.getValue(0, time) * diffIntensity.getValue(0, time), 1.0f);
	Vec4D p;
	if (type==MODELLIGHT_DIRECTIONAL) {
		// directional
		p = Vec4D(tdir, 0.0f);
	} else if (type==MODELLIGHT_POINT) {
		// point
		p = Vec4D(tpos, 1.0f);
	} else {
		p = Vec4D(tpos, 1.0f);
		wxLogMessage(wxT("Error: Light type %d is unknown."), type);
	}
	//gLog("Light %d (%f,%f,%f) (%f,%f,%f) [%f,%f,%f]\n", l-GL_LIGHT4, ambcol.x, ambcol.y, ambcol.z, diffcol.x, diffcol.y, diffcol.z, p.x, p.y, p.z);
	glLightfv(l, GL_POSITION, p);
	glLightfv(l, GL_DIFFUSE, diffcol);
	glLightfv(l, GL_AMBIENT, ambcol);
	glEnable(l);
}

void TextureAnim::init(MPQFile &f, ModelTexAnimDef &mta, uint32 *global)
{
	trans.init(mta.trans, f, global);
	rot.init(mta.rot, f, global);
	scale.init(mta.scale, f, global);
}

void Bone::initV3(MPQFile &f, ModelBoneDef &b, uint32 *global, MPQFile *animfiles)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;
	//billboard = false;

	boneDef = b;
	
	trans.init(b.translation, f, global, animfiles);
	rot.init(b.rotation, f, global, animfiles);
	scale.init(b.scaling, f, global, animfiles);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}

void Bone::initV2(MPQFile &f, ModelBoneDef &b, uint32 *global)
{
	calc = false;

	parent = b.parent;
	pivot = fixCoordSystem(b.pivot);
	billboard = (b.flags & MODELBONE_BILLBOARD) != 0;
	//billboard = false;

	boneDef = b;
	
	trans.init(b.translation, f, global);
	rot.init(b.rotation, f, global);
	scale.init(b.scaling, f, global);
	trans.fix(fixCoordSystem);
	rot.fix(fixCoordSystemQuat);
	scale.fix(fixCoordSystem2);
}

void ModelAttachment::init(MPQFile &f, ModelAttachmentDef &mad, uint32 *global)
{
	pos = fixCoordSystem(mad.pos);
	bone = mad.bone;
	id = mad.id;
}

void ModelAttachment::setup()
{
	Matrix m = model->bones[bone].mat;
	m.transpose();
	glMultMatrixf(m);
	glTranslatef(pos.x, pos.y, pos.z);
}

void ModelAttachment::setupParticle()
{
	Matrix m = model->bones[bone].mat;
	m.transpose();
	glMultMatrixf(m);
	glTranslatef(pos.x, pos.y, pos.z);
}

void Bone::calcMatrix(Bone *allbones, ssize_t anim, size_t time, bool rotate)
{
	if (calc)
		return;

	Matrix m;
	Quaternion q;

	bool tr = rot.uses(anim) || scale.uses(anim) || trans.uses(anim) || billboard;
	if (tr) {
		m.translation(pivot);
		
		if (trans.uses(anim)) {
			Vec3D tr = trans.getValue(anim, time);
			m *= Matrix::newTranslation(tr);
		}

		if (rot.uses(anim) && rotate) {
			q = rot.getValue(anim, time);
			m *= Matrix::newQuatRotate(q);
		}

		if (scale.uses(anim)) {
			Vec3D sc = scale.getValue(anim, time);
			m *= Matrix::newScale(sc);
		}

		if (billboard) {			
			float modelview[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

			Vec3D vRight = Vec3D(modelview[0], modelview[4], modelview[8]);
			Vec3D vUp = Vec3D(modelview[1], modelview[5], modelview[9]); // Spherical billboarding
			//Vec3D vUp = Vec3D(0,1,0); // Cylindrical billboarding
			vRight = vRight * -1;
			m.m[0][2] = vRight.x;
			m.m[1][2] = vRight.y;
			m.m[2][2] = vRight.z;
			m.m[0][1] = vUp.x;
			m.m[1][1] = vUp.y;
			m.m[2][1] = vUp.z;
		}

		m *= Matrix::newTranslation(pivot*-1.0f);
		
	} else m.unit();

	if (parent > -1) {
		allbones[parent].calcMatrix(allbones, anim, time, rotate);
		mat = allbones[parent].mat * m;
	} else mat = m;

	// transform matrix for normal vectors ... ??
	if (rot.uses(anim) && rotate) {
		if (parent>=0)
			mrot = allbones[parent].mrot * Matrix::newQuatRotate(q);
		else
			mrot = Matrix::newQuatRotate(q);
	} else mrot.unit();

	transPivot = mat * pivot;

	calc = true;
}


inline void Model::draw()
{
	if (!ok)
		return;

	if (!animated) {
		if(showModel)
			glCallList(dlist);

	} else {
		if (ind) {
			animate(currentAnim);
		} else {
			if (!animcalc) {
				animate(currentAnim);
				//animcalc = true; // Not sure what this is really for but it breaks WMO animation
			}
		}

		if(showModel)
			drawModel();
	}
}

// These aren't really needed in the model viewer.. only wowmapviewer
void Model::lightsOn(GLuint lbase)
{
	// setup lights
	for (size_t i=0, l=lbase; i<header.nLights; i++) 
		lights[i].setup(animtime, (GLuint)l++);
}

// These aren't really needed in the model viewer.. only wowmapviewer
void Model::lightsOff(GLuint lbase)
{
	for (size_t i=0, l=lbase; i<header.nLights; i++) 
		glDisable((GLenum)l++);
}

// Updates our particles within models.
void Model::updateEmitters(float dt)
{
	if (!ok || !showParticles || !bShowParticle) 
		return;

	for (size_t i=0; i<header.nParticleEmitters; i++) {
		particleSystems[i].update(dt);
	}
}


// Draws the "bones" of models  (skeletal animation)
void Model::drawBones()
{
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	for (size_t i=0; i<header.nBones; i++) {
	//for (size_t i=30; i<40; i++) {
		if (bones[i].parent != -1) {
			glVertex3fv(bones[i].transPivot);
			glVertex3fv(bones[bones[i].parent].transPivot);
		}
	}
	glEnd();
	glEnable(GL_DEPTH_TEST);
}

// Sets up the models attachments
void Model::setupAtt(int id)
{
	int l = attLookup[id];
	if (l>-1)
		atts[l].setup();
}

// Sets up the models attachments
void Model::setupAtt2(int id)
{
	int l = attLookup[id];
	if (l>=0)
		atts[l].setupParticle();
}

// Draws the Bounding Volume, which is used for Collision detection.
void Model::drawBoundingVolume()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	for (size_t i=0; i<header.nBoundingTriangles; i++) {
		size_t v = boundTris[i];
		if (v < header.nBoundingVertices)
			glVertex3fv(bounds[v]);
		else 
			glVertex3f(0,0,0);
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Renders our particles into the pipeline.
void Model::drawParticles()
{
	// draw particle systems
	for (size_t i=0; i<header.nParticleEmitters; i++) {
		if (particleSystems != NULL)
			particleSystems[i].draw();
	}

	// draw ribbons
	for (size_t i=0; i<header.nRibbonEmitters; i++) {
		if (ribbons != NULL)
			ribbons[i].draw();
	}
}

// Adds models to the model manager, used by WMO's
int ModelManager::add(wxString name)
{
	int id;
	if (names.find(name) != names.end()) {
		id = names[name];
		items[id]->addref();
		return id;
	}
	// load new
	Model *model = new Model(name);
	id = nextID();
    do_add(name, id, model);
    return id;
}

// Resets the animation back to default.
void ModelManager::resetAnim()
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		((Model*)it->second)->animcalc = false;
	}
}

// same as other updateEmitter except does it for the all the models being managed - for WMO's
void ModelManager::updateEmitters(float dt)
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		((Model*)it->second)->updateEmitters(dt);
	}
}

void ModelManager::clear()
{
	for (std::map<int, ManagedItem*>::iterator it = items.begin(); it != items.end(); ++it) {
		doDelete(it->first);
		delete it->second;
	}
	items.clear();
	names.clear();
}

void ModelEvent::init(MPQFile &f, ModelEventDef &me, uint32 *globals)
{
	def = me;
}

