#include <wx/wfstream.h>

#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_lwo.h"
#include "modelcanvas.h"

//#include "CxImage/ximage.h"

//---------------------------------------------
// Scene Writing Functions
//---------------------------------------------

// Writes a single Key for an envelope.
void WriteLWSceneEnvKey(ofstream &fs, uint32 Chan, float value, float time, uint32 spline = 0)
{
	fs << _T("  Key ");				// Announces the start of a Key
	fs << value;					// The Key's Value;
	fs << _T(" " << time);			// Time, in seconds, a float. This can be negative, zero or positive. Keys are listed in the envelope in increasing time order.
	fs << _T(" " << spline);		// The curve type, an integer: 0 - TCB, 1 - Hermite, 2 - 1D Bezier (obsolete, equivalent to Hermite), 3 - Linear, 4 - Stepped, 5 - 2D Bezier
	fs << _T(" 0 0 0 0 0 0 \n");	// Curve Data 1-6, all 0s for now.
}

// Writes an entire channel with only 1 key.
// Use WriteLWSceneEnvArray for writing animations.
void WriteLWSceneEnvChannel(ofstream &fs, uint32 ChanNum, float value, float time, uint32 spline = 0)
{
	fs << _T("Channel " << ChanNum << "\n");	// Channel Number
	fs << _T("{ Envelope\n");
	fs << _T("  1\n");							// Number of Keys in this envelope.
	WriteLWSceneEnvKey(fs,ChanNum,value,time,spline);
	fs << _T("  Behaviors 1 1\n");				// Pre/Post Behaviors. Defaults to 1 - Constant.
	fs << _T("}\n");
}

// Used for writing the keyframes of an animation.
void WriteLWSceneEnvArray(ofstream &fs, uint32 ChanNum, std::vector<float> value, std::vector<float> time, std::vector<uint8> spline)
{
	fs << _T("Channel " << ChanNum << "\n");
	fs << _T("{ Envelope\n");
	fs << _T("  " << value.size() << "\n");
	for (uint32 n=0;n<value.size();n++){
		uint32 thisspline = 0;
		if (spline[n]) {
			thisspline = spline[n];
		}
		WriteLWSceneEnvKey(fs,ChanNum,value[n],time[n],thisspline);
	}

	fs << _T("  Behaviors 1 1\n");
	fs << _T("}\n");
}

// Writes the "Plugin" information for a scene object, light, camera &/or bones.
void WriteLWScenePlugin(ofstream &fs, wxString type, uint32 PluginCount, wxString PluginName, wxString Data = wxEmptyString)
{
	fs << _T("Plugin " << type << " " << PluginCount << " " << PluginName << "\n" << Data << "EndPlugin\n");
}

// Writes an Object or Null Object to the scene file.
void WriteLWSceneObject(ofstream &fs, wxString Filename, AnimationData AnimData, uint32 &ItemNumber, int Visibility = 7, bool isNull = false, int32 ParentNum = -1, wxString Label=wxEmptyString)
{
	bool isLabeled = false;
	bool isParented = false;
	if (!Label.IsEmpty())
		isLabeled = true;
	if (ParentNum > -1)
		isParented = true;
	if (Visibility > 7)
		Visibility = 7;
	if (Visibility < 0)
		Visibility = 0;

	if (isNull == true){
		fs << _T("AddNullObject");
	}else{
		fs << _T("LoadObjectLayer 1");
	}
	fs << _T(" 1" << wxString::Format(_T("%07x"),ItemNumber) << " " << Filename << "\nChangeObject 0\n");
	if (isLabeled)
		fs << _T("// " << Label << "\n");
	fs << _T("ShowObject "<<Visibility<<" -1 0.376471 0.878431 0.941176 \nGroup 0\nObjectMotion\nNumChannels 9\n");
	std::vector<uint8>splines;
	std::vector<float>Time;
	std::vector<float>PosX;
	std::vector<float>PosY;
	std::vector<float>PosZ;
	std::vector<float>RotX;
	std::vector<float>RotY;
	std::vector<float>RotZ;
	std::vector<float>ScaX;
	std::vector<float>ScaY;
	std::vector<float>ScaZ;
	for (unsigned int c=0;c<AnimData.Size();c++){
		splines.push_back(0);
		Time.push_back(AnimData.Time[c]);
		PosX.push_back(AnimData.Position[c].x);
		PosY.push_back(AnimData.Position[c].y);
		PosZ.push_back(-AnimData.Position[c].z);
		RotX.push_back(AnimData.Rotation[c].x);
		RotY.push_back(AnimData.Rotation[c].y);
		RotZ.push_back(AnimData.Rotation[c].z);
		ScaX.push_back(AnimData.Scale[c].x);
		ScaY.push_back(AnimData.Scale[c].y);
		ScaZ.push_back(AnimData.Scale[c].z);
	}
	// Position
	//WriteLWSceneEnvChannel(fs,1,Pos.y,0);
	WriteLWSceneEnvArray(fs,0,PosX,Time,splines);
	WriteLWSceneEnvArray(fs,1,PosY,Time,splines);
	WriteLWSceneEnvArray(fs,2,PosZ,Time,splines);
	// Rotation
	WriteLWSceneEnvArray(fs,3,RotX,Time,splines);
	WriteLWSceneEnvArray(fs,4,RotY,Time,splines);
	WriteLWSceneEnvArray(fs,5,RotZ,Time,splines);
	// Scale
	WriteLWSceneEnvArray(fs,6,ScaX,Time,splines);
	WriteLWSceneEnvArray(fs,7,ScaY,Time,splines);
	WriteLWSceneEnvArray(fs,8,ScaZ,Time,splines);

	fs << _T("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
	if (isParented == true)
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentNum) << "\n");
	fs << _T("IKInitialState 0\nSubPatchLevel 3 3\nShadowOptions 7\n");

	fs << _T("\n");
	ItemNumber++;
}

void WriteLWSceneObject2(ofstream &fs, LWSceneObj Object)
{
	/*
	bool isLabeled = false;
	bool isParented = false;
	*/

	if (Object.isNull == true){
		fs << _T("AddNullObject");
	}else{
		fs << _T("LoadObjectLayer 1");
	}
	fs << _T(" 1" << wxString::Format(_T("%07x"),Object.ObjectID) << " " << Object.Name << "\nChangeObject 0\n");
	/*
	if (isLabeled)
		fs << _T("// " << Label << "\n");
	fs << _T("ShowObject 7 -1 0.376471 0.878431 0.941176 \nGroup 0\nObjectMotion\nNumChannels 9\n");
	std::vector<uint8>splines;
	std::vector<float>Time;
	std::vector<float>PosX;
	std::vector<float>PosY;
	std::vector<float>PosZ;
	std::vector<float>RotX;
	std::vector<float>RotY;
	std::vector<float>RotZ;
	std::vector<float>ScaX;
	std::vector<float>ScaY;
	std::vector<float>ScaZ;
	for (unsigned int c=0;c<AnimData.Size();c++){
		splines.push_back(0);
		Time.push_back(AnimData.Time[c]);
		PosX.push_back(AnimData.Position[c].x);
		PosY.push_back(AnimData.Position[c].y);
		PosZ.push_back(-AnimData.Position[c].z);
		RotX.push_back(AnimData.Rotation[c].x);
		RotY.push_back(AnimData.Rotation[c].y);
		RotZ.push_back(AnimData.Rotation[c].z);
		ScaX.push_back(AnimData.Scale[c].x);
		ScaY.push_back(AnimData.Scale[c].y);
		ScaZ.push_back(AnimData.Scale[c].z);
	}
	// Position
	//WriteLWSceneEnvChannel(fs,1,Pos.y,0);
	WriteLWSceneEnvArray(fs,0,PosX,Time,splines);
	WriteLWSceneEnvArray(fs,1,PosY,Time,splines);
	WriteLWSceneEnvArray(fs,2,PosZ,Time,splines);
	// Rotation
	WriteLWSceneEnvArray(fs,3,RotX,Time,splines);
	WriteLWSceneEnvArray(fs,4,RotY,Time,splines);
	WriteLWSceneEnvArray(fs,5,RotZ,Time,splines);
	// Scale
	WriteLWSceneEnvArray(fs,6,ScaX,Time,splines);
	WriteLWSceneEnvArray(fs,7,ScaY,Time,splines);
	WriteLWSceneEnvArray(fs,8,ScaZ,Time,splines);

	fs << _T("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
	if (isParented == true)
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentNum) << "\n");
	fs << _T("IKInitialState 0\nSubPatchLevel 3 3\nShadowOptions 7\n");

	fs << _T("\n");
	ItemNumber++;
	*/
}

// Writes an Object's Bone to the scene file.
void WriteLWSceneBone(ofstream &fs, wxString BoneName, int BoneType, Vec3D Pos, Vec3D Rot, float Length, uint32 BoneNumber, uint32 ParentObject, int32 ParentNum = -1)
{
	bool isParented = false;
	if (ParentNum > -1){
		isParented = true;
	}

	fs << _T("AddBone 4") << wxString::Format(_T("%03x"),BoneNumber) << _T("0000\nBoneName " << BoneName << "\n");
	fs << _T("ShowBone 1 -1 0.376471 0.878431 0.941176\nBoneActive 1" << "\n");
	fs << _T("BoneStrength 1\nScaleBoneStrength 1" << "\n");
	fs << _T("BoneRestPosition "<<Pos.x<<" "<<Pos.y<<" "<<Pos.z<< "\n");
	fs << _T("BoneRestDirection "<<Rot.x<<" "<<Rot.y<<" "<<Rot.z<< "\n");
	fs << _T("BoneRestLength "<<Length<< "\n");
	fs << _T("BoneType " << BoneType << "\n");
	fs << _T("BoneMotion\nNumChannels 9\n");
	// Position
	WriteLWSceneEnvChannel(fs,0,Pos.x,0);
	WriteLWSceneEnvChannel(fs,1,Pos.y,0);
	WriteLWSceneEnvChannel(fs,2,Pos.z,0);
	// Rotation
	WriteLWSceneEnvChannel(fs,3,Rot.x,0);
	WriteLWSceneEnvChannel(fs,4,Rot.y,0);
	WriteLWSceneEnvChannel(fs,5,Rot.z,0);
	// Scale
	WriteLWSceneEnvChannel(fs,6,1,0);
	WriteLWSceneEnvChannel(fs,7,1,0);
	WriteLWSceneEnvChannel(fs,8,1,0);

	fs << _T("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
	if (isParented == true){
		fs << _T("ParentItem 4" << wxString::Format(_T("%03x"),ParentNum) << "0000\n");
	}else{
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentObject) << "\n");
	}

	fs << _T("IKInitialState 0");

	fs << _T("\n");
}

// Write a Light to the Scene File
void WriteLWSceneLight(ofstream &fs, uint32 &lcount, Vec3D LPos, uint32 Ltype, Vec3D Lcolor, float Lintensity, bool useAtten, float AttenEnd, float defRange = 2.5, wxString prefix = wxEmptyString, uint32 ParentNum = -1)
{
	bool isParented = false;
	if (ParentNum >= 0)
		isParented = true;
	if (!prefix.IsEmpty())
		prefix = _T(" "+prefix);
	if ((useAtten == true)&&(AttenEnd<=0))
		useAtten = false;


	fs << _T("AddLight 2" << wxString::Format(_T("%07x"),lcount) << "\n");
	//modelname[0] = toupper(modelname[0]);
	fs << _T("LightName " << prefix << "Light " << lcount+1 << "\n");
	fs << _T("ShowLight 1 -1 0.941176 0.376471 0.941176\n");	// Last 3 Numbers are Layout Color for the Light.
	fs << _T("LightMotion\n");
	fs << _T("NumChannels 9\n");
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
		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ParentNum) << "\n");

	// Light Color Reducer
	// Some lights have a color channel greater than 255. This reduces all the colors, but increases the intensity, which should keep it looking the way Blizzard intended.
	while ((Lcolor.x > 1)||(Lcolor.y > 1)||(Lcolor.z > 1)) {
		Lcolor.x = Lcolor.x * 0.99;
		Lcolor.y = Lcolor.y * 0.99;
		Lcolor.z = Lcolor.z * 0.99;
		Lintensity = Lintensity / 0.99;
	}

	fs << _T("LightColor " << Lcolor.x << " " << Lcolor.y << " " << Lcolor.z << "\n");
	fs << _T("LightIntensity " << Lintensity << "\n");

	// Process Light type & output!
	switch (Ltype) {
		// Omni Lights
		case 1:
		default:
			// Default to an Omni (Point) light.
			fs << _T("LightType 1\n");

			if (useAtten == true) {
				// Use Inverse Distance for the default Light Falloff Type. Should better simulate WoW Lights, until I can write a WoW light plugin for Lightwave...
				fs << _T("LightFalloffType 2\nLightRange " << AttenEnd << "\n");
			}else{
				// Default to these settings, which look pretty close...
				fs << _T("LightFalloffType 2\nLightRange " << defRange << "\n");
			}
			fs << _T("ShadowType 1\nShadowColor 0 0 0\n");
			WriteLWScenePlugin(fs,_T("LightHandler"),1,_T("PointLight"));
	}
	fs << _T("\n");
	lcount++;
}

// Data Writing Functions

// VX is Lightwave Shorthand for any Point Number, because Lightwave stores points differently if they're over a certain threshold.
void LW_WriteVX(wxFFileOutputStream &f, uint32 p, uint32 &Size){
	if (p <= 0xFF00){
		uint16 indice = MSB2(p & 0x0000FFFF);
		f.Write(reinterpret_cast<char *>(&indice),2);
		Size += 2;
	}else{
		uint32 indice = MSB4<uint32>(p + 0xFF000000);
		f.Write(reinterpret_cast<char *>(&indice), 4);
		Size += 4;
	}
}

void LW_WriteSurface(wxFFileOutputStream &f, wxString surfName, Vec4D Color, float reflect, bool cull, bool hasVertColors, uint32 surfID, wxString comment, uint32 &fileSize){
	#ifdef _DEBUG
		wxLogMessage(_T("LW Write Surface Vars:\nSurfName: %s\nColor: %f/%f/%f\nReflect Value: %f\nCulling: %s\nSurfaceID: %i\nComment: \"%s\""),surfName.c_str(),Color.x,Color.y,Color.z,(reflect*10),(cull?_T("True"):_T("False")),surfID,comment);
	#endif
	int off_T;

	uint32 surfaceDefSize = 0;
	f.Write(_T("SURF"), 4);
	u32 = MSB4<uint32>(surfaceDefSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileSize += 8;

	surfName.Append(_T('\0'));
	if (fmod((float)surfName.length(), 2.0f) > 0)
		surfName.Append(_T('\0'));

	surfName.Append(_T('\0')); // ""
	surfName.Append(_T('\0')); // Evens out the Code.
	f.Write(surfName.data(), (int)surfName.length());

	surfaceDefSize += (uint32)surfName.length();

	// Surface Attributes
	// COLOUR, size 4, bytes 2
	f.Write(_T("COLR"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	// value
	f32 = MSB4<float>(Color.x);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f32 = MSB4<float>(Color.y);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f32 = MSB4<float>(Color.z);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 20;

	// LUMI
	f.Write(_T("LUMI"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// DIFF
	f.Write(_T("DIFF"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// SPEC
	f.Write(_T("SPEC"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = reflect/2;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// REFL
	f.Write(_T("REFL"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0f;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// TRAN
	f.Write(_T("TRAN"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = MSB4<float>(reflect);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 12;

	// GLOSSINESS
	f.Write(_T("GLOS"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	// Value
	// Set to 20%, because that seems right.
	f32 = 0.2f;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	surfaceDefSize += 12;

	if (hasVertColors == true){
		// VCOL (Vector Colors)
		f.Write(_T("VCOL"), 4);
		u16 = MSB2(18); // size
		uint16 zero = 0;
		f.Write(reinterpret_cast<char *>(&u16), 2);
		// Unknown Values
		f32 = MSB4<float>(1.0f);
		f.Write(reinterpret_cast<char *>(&f32), 4);
		f.Write(reinterpret_cast<char *>(&zero), 2);
		// RGBA Map Name
		f.Write(_T("RGBA"), 4);
		f.Write(_T("Colors"), 6);
		f.Write(reinterpret_cast<char *>(&zero), 2);
		surfaceDefSize += 24;
	}

	// SMAN (Smoothing)
	f.Write(_T("SMAN"), 4);
	u16 = 4; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	// Smoothing is done in radiens. PI = 180 degree smoothing.
	f32 = (float)PI;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	surfaceDefSize += 10;

	// RFOP
	f.Write(_T("RFOP"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 8;

	// TROP
	f.Write(_T("TROP"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 8;

	// SIDE
	f.Write(_T("SIDE"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	if (cull == false){
		u16 = 3;
	}
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);

	surfaceDefSize += 8;

	// --
	// BLOK
	uint16 blokSize = 0;
	f.Write(_T("BLOK"), 4);
	f.Write(reinterpret_cast<char *>(&blokSize), 2);
	surfaceDefSize += 6;

	// IMAP
	f.Write(_T("IMAP"), 4);
	u16 = 50-8; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0x80;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// CHAN
	f.Write(_T("CHAN"), 4);
	u16 = 4; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f.Write(_T("COLR"), 4);
	blokSize += 10;

	// OPAC
	f.Write(_T("OPAC"), 4);
	u16 = 8; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1.0;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 14;

	// ENAB
	f.Write(_T("ENAB"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// NEGA
	f.Write(_T("NEGA"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;
/*
	// AXIS
	// This is only needed for Planar images. Everything but ADTs uses UV data.
	f.Write(_T("AXIS"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;
*/
	// TMAP
	f.Write(_T("TMAP"), 4);
	u16 = 98; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 6;

	// CNTR
	f.Write(_T("CNTR"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0;
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 20;

	// SIZE
	f.Write(_T("SIZE"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1.0;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 20;

	// ROTA
	f.Write(_T("ROTA"), 4);
	u16 = 14; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 0.0;
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 20;

	// FALL
	f.Write(_T("FALL"), 4);
	u16 = 16; // size
	u16 = MSB2(u16);
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
	f.Write(_T("OREF"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// CSYS
	f.Write(_T("CSYS"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 0;
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// end TMAP

	// PROJ
	f.Write(_T("PROJ"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 5;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// AXIS
	f.Write(_T("AXIS"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 2;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// IMAG
	f.Write(_T("IMAG"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = surfID;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// WRAP
	f.Write(_T("WRAP"), 4);
	u16 = 4; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 10;

	// WRPW
	f.Write(_T("WRPW"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 12;

	// WRPH
	f.Write(_T("WRPH"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	u16 = 0;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 12;

	// VMAP
	f.Write(_T("VMAP"), 4);
	u16 = 8; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	wxString t = _T("Texture");
	t.Append(_T('\0'));
	f.Write(t.data(), t.length());
	blokSize += 14;

	// AAST
	f.Write(_T("AAST"), 4);
	u16 = 6; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 1;
	f32 = MSB4<float>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	blokSize += 12;

	// PIXB
	f.Write(_T("PIXB"), 4);
	u16 = 2; // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	u16 = 1;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	blokSize += 8;

	// Fix Blok Size
	surfaceDefSize += blokSize;
	off_T = -2-blokSize;
	f.SeekO(off_T, wxFromCurrent);
	u16 = MSB2(blokSize);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f.SeekO(0, wxFromEnd);
	// ================

	// CMNT
	f.Write(_T("CMNT"), 4);
	comment.Append(_T('\0'));
	if (fmod((float)comment.length(), 2.0f) > 0)
		comment.Append(_T('\0'));
	u16 = (uint16)comment.length(); // size
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f.Write(comment.data(), comment.length());
	surfaceDefSize += 6 + (uint32)comment.length();

	f.Write(_T("VERS"), 4);
	u16 = 4;
	u16 = MSB2(u16);
	f.Write(reinterpret_cast<char *>(&u16), 2);
	f32 = 950;
	f32 = MSB4<int32>(f32);
	f.Write(reinterpret_cast<char *>(&f32), 4);
	surfaceDefSize += 10;
			
	// Fix Surface Size
	fileSize += surfaceDefSize;
	off_T = -4-surfaceDefSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(surfaceDefSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
}


// Scene Writer
void ExportLightwaveScene(LWScene SceneData){
	wxLogMessage(_T("Export Lightwave Scene Function running."));

	wxString SceneName = SceneData.FilePath + SceneData.FileName;

	ofstream fs(SceneName.fn_str(), ios_base::out | ios_base::trunc);
	if (!fs.is_open()) {
		wxMessageBox(_T("Unable to open the scene file for exporting."),_T("Scene Export Failure"));
		wxLogMessage(_T("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	wxLogMessage(_T("Opened %s for writing..."),SceneName.c_str());
	SceneName = SceneName.AfterLast(SLASH);

	// File Top
	fs << _T("LWSC\n");
	fs << _T("5\n\n"); // I think this is a version-compatibility number...

	for (uint32 nobj=0;nobj<SceneData.Object.size();nobj++){
		LWSceneObj Obj = SceneData.Object[nobj];
		WriteLWSceneObject2(fs,Obj);
	}

	// Global Light Options
	if (!SceneData.AmbientIntensity){
		SceneData.AmbientIntensity = 0.5f;		// Default to 50%
	}
	fs << _T("AmbientColor 1 1 1\nAmbientIntensity ") << SceneData.AmbientIntensity << _T("\nDoubleSidedAreaLights 1\nRadiosityType 2\nRadiosityInterpolated 1\nRadiosityTransparency 0\nRadiosityIntensity 1\nRadiosityTolerance 0.2928932\nRadiosityRays 64\nSecondaryBounceRays 16\nRadiosityMinPixelSpacing 4\nRadiosityMaxPixelSpacing 100\nRadiosityMultiplier 1\nRadiosityDirectionalRays 0\nRadiosityUseGradients 0\nRadiosityUseBehindTest 1\nBlurRadiosity 1\nRadiosityFlags 0\nRadiosityCacheModulus 1\nRadiosityCacheFilePath radiosity.cache\nPixelFilterForceMT 0\n\n");
}


//---------------------------------------------
// M2 Functions
//---------------------------------------------

/* LWO2

Links to helpful documents:
http://gpwiki.org/index.php/LWO
http://home.comcast.net/~erniew/lwsdk/docs/filefmts/lwo2.html
*/

//---------------------------------------------
// --==M2 Bones Scene File==--
//---------------------------------------------
// Exports a Model to a Lightwave Scene File.
void ExportM2toScene(Model *m, const char *fn, bool init){
	// Should we generate a scene file?
	// Wll only generate if there are bones, lights, or a Camera in the model.
	bool doreturn = false;
	if ((m->header.nLights == 0) && (m->header.nBones == 0)){
		doreturn = true;
	}
	if (modelExport_UseWMVPosRot == true)
		doreturn = false;
	if (m->hasCamera == true){
		doreturn = false;
	}
	if (doreturn == true){
		wxLogMessage(_T("M2 Scene Export: Did not find any reason to export a scene. Stopping Scene export."));
		return;
	}
	wxLogMessage(_T("M2 Scene Export Values:\n  nLights: %i\n  nBones: %i\n  hasCamera: %s\n  Use WMV Pos/Rot: %s\n  doreturn: %s"),m->header.nLights,m->header.nBones,(m->hasCamera?_T("true"):_T("false")),(modelExport_UseWMVPosRot?_T("true"):_T("false")),(doreturn?_T("true"):_T("false")));

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast(_T('.'));
	SceneName << _T(".lws");

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);

		MakeDirs(Path,_T("Scenes"));

		SceneName.Empty();
		SceneName << Path << SLASH << _T("Scenes") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true && m->modelType != MT_CHAR){
		wxString Path1, Path2, Name;
		Path1 << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		SceneName.Empty();
		SceneName << Path1 << SLASH << Path2 << SLASH << Name;
	}

	ofstream fs(SceneName.fn_str(), ios_base::out | ios_base::trunc);

	if (!fs.is_open()) {
		wxMessageBox(_T("Unable to open the scene file for exporting."),_T("Scene Export Failure"));
		wxLogMessage(_T("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	wxLogMessage(_T("Opened %s for writing..."),SceneName.c_str());
	SceneName = SceneName.AfterLast(SLASH);

	// File Top
	fs << _T("LWSC\n");
	fs << _T("5\n\n"); // I think this is a version-compatibility number...

	uint32 RangeEnd = 0;

	uint32 mcount = 0; // Model Count
	//uint32 lcount = 0; // Light Count
	//uint32 bcount = 0; // Bone Count

	Vec3D ZeroPos(0,0,0);
	Vec3D ZeroRot(0,0,0);
	Vec3D OneScale(1,1,1);

	// Objects/Doodads go here

	// Exported Object
	int ModelID = mcount;
	wxString Obj = wxString(fn, wxConvUTF8).AfterLast(SLASH);
	wxString objFilename = wxEmptyString;
	if (modelExport_LW_PreserveDir == true){
		objFilename << _T("Objects") << SLASH;
	}
	if (modelExport_PreserveDir == true){
		objFilename += wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		objFilename << SLASH;
		objFilename.Replace(_T("\\"),_T("/"));
	}
	objFilename += Obj;

	AnimationData ObjData;
	Vec3D ObjPos = ZeroPos; //m->pos;
	Vec3D ObjRot = ZeroRot; //m->rot;
	if (modelExport_UseWMVPosRot == true){
		ObjPos = m->pos;
		ObjRot = m->rot;
	}
	if (m->hasCamera == true){
		ModelCamera *cam = &m->cam;
		if (cam->WorldOffset != Vec3D(0,0,0)){
			ObjPos = cam->WorldOffset;
			// ObjRot = QuaternionToXYZ(Vec3D(0,0,0), cam->WorldRotation);
		}
	}
	MakeModelFaceForwards(ObjPos,false);
	float temp;
	temp = ObjRot.y;
	ObjRot.y = ObjRot.z;
	ObjRot.z = temp;
	ObjData.Push(ObjPos,(ObjRot/(float)RADIAN),OneScale,0);

	WriteLWSceneObject(fs,objFilename,ObjData,mcount);

	// Export Bones
	if (m->header.nBones > 0){
		fs << _T("BoneFalloffType 5\nFasterBones 1\n");
		for (uint16 x=0;x<m->header.nBones;x++){
			Bone *cbone = &m->bones[x];
			Vec3D Pos = cbone->pivot;
			if (cbone->parent > -1)
				Pos -= m->bones[cbone->parent].pivot;
			if (init == false){
				Pos = cbone->transPivot;
				if (cbone->parent > -1)
					Pos -= m->bones[cbone->parent].transPivot;
			}
			Pos.z = -Pos.z;
			MakeModelFaceForwards(Pos,false);
			wxString bone_name = wxString::Format(_T("Bone_%03i"), x);
			for (int j=0; j<BONE_MAX; ++j) {
				if (m->keyBoneLookup[j] == static_cast<int>(x)) {
					bone_name = Bone_Names[j];
					break;
				}
			}
			WriteLWSceneBone(fs, bone_name, 1, Pos, Vec3D(0,0,0), 0.25, x, ModelID, cbone->parent);
		}
	}

	// Lighting Basics
	fs << _T("AmbientColor 1 1 1\nAmbientIntensity 0.25\nDoubleSidedAreaLights 1\n\n");

	// Lights

	// Camera data goes here.
	if (m->hasCamera == true){
		ModelCamera *cam = &m->cam;
		uint32 anim = m->animManager->GetAnim();
		uint32 CameraTargetID = mcount;
		AnimationData CamData;
		if (cam->tTarget.data[anim].size() > 1){
			for (unsigned int x=0;x<cam->tTarget.data[anim].size();x++){
				Vec3D a = cam->target + cam->tTarget.data[anim][x];
				MakeModelFaceForwards(a,false);
				uint32 ctime = cam->tTarget.times[anim][x]/30;
				a.x = -a.x;
				a.z = -a.z;
				CamData.Push(a,ZeroRot,OneScale,ctime/30);
				if (ctime > RangeEnd)
					RangeEnd = ctime;
			}
		}else{
			CamData.Push(cam->target,ZeroRot,OneScale,0);
		}

		WriteLWSceneObject(fs,_T("Camera Target"),CamData,mcount,0,true,ModelID);

		fs << _T("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nCameraMotion\nNumChannels 6\n");

		if (cam->tPos.data[anim].size() > 1){
			std::vector<float> time, PosX, PosY, PosZ, ZeroFloat;
			std::vector<uint8> splines;
			// Animations
			for (unsigned int x=0;x<cam->tPos.data[anim].size();x++){
				// Position Data
				Vec3D p_val = cam->pos + cam->tPos.data[anim][x];
				MakeModelFaceForwards(p_val,true);
				uint32 ctime = cam->tPos.times[anim][x]/30;
				float p_Time = ctime/30;
				if (ctime > RangeEnd)
					RangeEnd = ctime;
				splines.push_back(0);
				ZeroFloat.push_back(0);
				time.push_back(p_Time);
				PosX.push_back(p_val.x);
				PosY.push_back(p_val.y);
				PosZ.push_back(p_val.z);
			}
			WriteLWSceneEnvArray(fs,0,PosX,time,splines);
			WriteLWSceneEnvArray(fs,0,PosY,time,splines);
			WriteLWSceneEnvArray(fs,0,PosZ,time,splines);
			WriteLWSceneEnvArray(fs,0,ZeroFloat,time,splines);
			WriteLWSceneEnvArray(fs,0,ZeroFloat,time,splines);
			WriteLWSceneEnvArray(fs,0,ZeroFloat,time,splines);
		}else{
			Vec3D vect = cam->pos;
			MakeModelFaceForwards(vect,false);
			WriteLWSceneEnvChannel(fs,0,vect.x,0);
			WriteLWSceneEnvChannel(fs,1,vect.y,0);
			WriteLWSceneEnvChannel(fs,2,vect.z,0);
			WriteLWSceneEnvChannel(fs,3,0,0);
			WriteLWSceneEnvChannel(fs,4,0,0);
			WriteLWSceneEnvChannel(fs,5,0,0);
		}

		fs << _T("ParentItem 1" << wxString::Format(_T("%07x"),ModelID) << "\n");
		fs << _T("IKInitCustomFrame 0\nGoalStrength 1\nIKFKBlending 0\nIKSoftMin 0.25\nIKSoftMax 0.75\nCtrlPosItemBlend 1\nCtrlRotItemBlend 1\nCtrlScaleItemBlend 1\n\n");
		fs << _T("HController 1\nPController 1\nPathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
		fs << _T("TargetItem 1"<<wxString::Format(_T("%07x"),CameraTargetID)<<"\n");
		fs << _T("ZoomFactor "<<(cam->fov*3.6)<<"\nZoomType 2\n");
		WriteLWScenePlugin(fs,_T("CameraHandler"),1,_T("Perspective"));	// Make the camera a Perspective camera
	}

	// Scene File Basics
	fs << _T("RenderRangeType 0\nFirstFrame 1\nLastFrame "<<RangeEnd<<"\n");
	fs << _T("FrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary 1-"<<RangeEnd<<"\n");
	fs << _T("PreviewFirstFrame 0\nPreviewLastFrame "<<RangeEnd<<"\nPreviewFrameStep 1\nCurrentFrame 0\nFramesPerSecond 30\nChangeScene 0\n\n");

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << _T("RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n");

	fs.close();
}

/*
NOTE!!
I've done some research into the LWO2 format. I have a HUGE commented section about it down in the WMO function that details the layout, and some of the byte info too.
I'll update this function once I re-tune the WMO function.
		-Kjasi
*/
void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init)
{
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		file.Empty();
		file << Path << SLASH << _T("Objects") << SLASH << Name;
	}
	if (m->modelType != MT_CHAR){
		if (modelExport_PreserveDir == true){
			wxString Path1, Path2, Name;
			Path1 << file.BeforeLast(SLASH);
			Name << file.AfterLast(SLASH);
			Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

			MakeDirs(Path1,Path2);

			file.Empty();
			file << Path1 << SLASH << Path2 << SLASH << Name;
		}
	}

	wxFFileOutputStream f(file, _T("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file.c_str());
		return;
	}
	LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));
	int off_T;
	uint32 counter=0;
	uint32 TagCounter=0;
	uint16 PartCounter=0;
	uint16 SurfCounter=0;
	unsigned short numVerts = 0;
	unsigned short numGroups = 0;
	uint16 dimension = 2;

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.

	//InitCommon(att, init);

	unsigned int fileLen = 0;

	// ===================================================
	// FORM		// Format Declaration
	//
	// Always exempt from the length of the file!
	// ===================================================
	f.Write(_T("FORM"), 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);


	// ===================================================
	// LWO2
	//
	// Declares this is the Lightwave Object 2 file format.
	// LWOB is the first format. It doesn't have a lot of the cool stuff LWO2 has...
	// ===================================================
	f.Write(_T("LWO2"), 4);
	fileLen += 4;


	// ===================================================
	// TAGS
	//
	// Used for various Strings. Known string types, in order:
	//		Sketch Color Names
	//		Part Names
	//		Surface Names
	// ===================================================
	f.Write(_T("TAGS"), 4);
	uint32 tagsSize = 0;
	wxString TAGS;
	u32 = 0;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	#ifdef _DEBUG
	// Debug Texture List
	wxLogMessage(_T("M2 Texture List for %s:"),m->modelname.c_str());
	for (unsigned short i=0; i<m->TextureList.size(); i++) {
		wxLogMessage(_T("Texture List[%i] = %s"),i,m->TextureList[i].c_str());
	}
	wxLogMessage(_T("M2 Texture List Complete for %s"),m->modelname.c_str());
	#endif

	// Mesh & Slot names
	wxString meshes[19] = {_T("Hairstyles"), _T("Facial1"), _T("Facial2"), _T("Facial3"), _T("Braces"),
		_T("Boots"), wxEmptyString, _T("Ears"), _T("Wristbands"),  _T("Kneepads"), _T("Pants"), _T("Pants"),
		_T("Tarbard"), _T("Trousers"), wxEmptyString, _T("Cape"), wxEmptyString, _T("Eyeglows"), _T("Belt") };
	wxString slots[15] = {_T("Helm"), wxEmptyString, _T("Shoulder"), wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString,
		wxEmptyString, wxEmptyString, _T("Right Hand Item"), _T("Left Hand Item"), wxEmptyString, wxEmptyString, _T("Quiver") };

	// Part Names
	for (unsigned short p=0; p<m->passes.size(); p++) {
		if (m->passes[p].init(m)){
			int g = m->passes[p].geoset;

			wxString partName;
			int mesh = m->geosets[g].id / 100;
			if (m->modelType == MT_CHAR && mesh < 19 && meshes[mesh] != wxEmptyString){
				partName = wxString::Format(_T("Geoset %03i - %s"),g,meshes[mesh].c_str());
			}else{
				partName = wxString::Format(_T("Geoset %03i"),g);
			}

			partName.Append(_T('\0'));
			if (fmod((float)partName.length(), 2.0f) > 0)
				partName.Append(_T('\0'));
			f.Write(partName.data(), partName.length());
			tagsSize += (uint32)partName.length();
		}
	}


	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {						
						wxString partName;
						if (att->slot < 15 && slots[att->slot]!=wxEmptyString){
							partName = wxString::Format(_T("%s"),slots[att->slot].c_str());
						}else{
							partName = wxString::Format(_T("Slot %02i"),att->slot);
						}

						partName.Append(_T('\0'));
						if (fmod((float)partName.length(), 2.0f) > 0)
							partName.Append(_T('\0'));
						f.Write(partName.data(), partName.length());
						tagsSize += (uint32)partName.length();
					}
				}
			}
		}

		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							int thisslot = att2->children[j]->slot;
							wxString partName;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								partName = wxString::Format(_T("Child %02i - %s"),j,slots[thisslot].c_str());
							}else{
								partName = wxString::Format(_T("Child %02i - Slot %02i"),j,att2->children[j]->slot);
							}

							partName.Append(_T('\0'));
							if (fmod((float)partName.length(), 2.0f) > 0)
								partName.Append(_T('\0'));
							f.Write(partName.data(), partName.length());
							tagsSize += (uint32)partName.length();
						}
					}
				}
			}
		}
	}

	#ifdef _DEBUG
	wxLogMessage(_T("M2 Part Names Written for %s"),m->modelname.c_str());
	#endif

	// Surface Name
	//wxString *surfArray = new wxString[m->passes.size()];
	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)){
	//		if (!surfArray[m->passes[i].tex]){
				wxString matName;
				matName = m->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));
				
				if (matName.Len() == 0)
					matName = wxString::Format(_T("Material_%03i"), p.tex);

				matName.Append(_T('\0'));
				if (fmod((float)matName.length(), 2.0f) > 0)
					matName.Append(_T('\0'));
				f.Write(matName.data(), matName.length());
				tagsSize += (uint32)matName.length();
	//			surfArray[m->passes[i].tex] = m->TextureList[m->passes[i].tex];
	//		}
		}
	}


	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {					
						wxString matName = attM->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));

						if (matName.Len() == 0)
							matName = wxString::Format(_T("Attach Material %03i"), p.tex);

						matName.Append(_T('\0'));
						if (fmod((float)matName.length(), 2.0f) > 0)
							matName.Append(_T('\0'));
						f.Write(matName.data(), matName.length());
						tagsSize += (uint32)matName.length();
					}
				}
			}
		}

		for (uint32 a=0; a<att->children.size(); a++) {
			Attachment *att2 = att->children[a];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							wxString matName = mAttChild->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));
							int thisslot = att2->children[j]->slot;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								if (matName == _T("Cape")) {
									wxString tex = wxString(mAttChild->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
									if (tex.Len() > 0){
										matName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),tex.c_str());
									}else{
										matName = wxString::Format(_T("%s - Surface"),slots[thisslot].c_str());
									}
								}else if (matName != wxEmptyString){
									matName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),matName.c_str());
								}else {
									matName = wxString::Format(_T("%s - Material %02i"),slots[thisslot].c_str(),p.tex);
								}
							}

							if (matName.Len() == 0)
								matName = wxString::Format(_T("Child %02i - Material %03i"), j, p.tex);

							matName.Append(_T('\0'));
							if (fmod((float)matName.length(), 2.0f) > 0)
								matName.Append(_T('\0'));
							f.Write(matName.data(), matName.length());
							tagsSize += (uint32)matName.length();
						}
					}
				}
			}
		}
	}

	off_T = -4-tagsSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(tagsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	fileLen += tagsSize;
	// ================
	#ifdef _DEBUG
	wxLogMessage(_T("M2 Surface Names Written for %s"),m->modelname.c_str());
	#endif

	

	// ===================================================
	// LAYR
	//
	// Specifies the start of a new layer. Each layer has it's own Point & Poly
	// chunk, which tells it what data is on what layer. It's probably best
	// to only have 1 layer for now.
	// ===================================================
	f.Write(_T("LAYR"), 4);
	u32 = MSB4<uint32>(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;
	// ================
	#ifdef _DEBUG
	wxLogMessage(_T("M2 Layer Defined for %s"),m->modelname.c_str());
	#endif

	// --
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	uint32 pointsSize = 0;
	f.Write(_T("PNTS"), 4);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the model vertice data
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				if ((init == false)&&(m->vertices)) {
					Vec3D vect = m->vertices[a];
					MakeModelFaceForwards(vect,true);
					vert.x = MSB4<float>(vect.x);
					vert.y = MSB4<float>(vect.y);
					vert.z = MSB4<float>(vect.z);
				} else {
					Vec3D vect = m->origVertices[a].pos;
					MakeModelFaceForwards(vect,true);
					vert.x = MSB4<float>(vect.x);
					vert.y = MSB4<float>(vect.y);
					vert.z = MSB4<float>(vect.z);
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

	// Output the Attachment vertice data
	if (att!=NULL){
		wxLogMessage(_T("Attachment found! Attempting to save Point Data..."));
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);
			wxLogMessage(_T("Loaded Attached Model %s for export."),attM->modelname.c_str());

			if (attM){
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

				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						wxLogMessage(_T("Exporting Point data for Attachment..."));
						for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
							uint16 a = attM->indices[b];
							Vec3D vert;
							if ((init == false)&&(attM->vertices)) {
								Vec3D v = attM->vertices[a];
								MakeModelFaceForwards(v,true);
								vert.z = MSB4<float>((v.x * scale.x) + pos.x);
								vert.y = MSB4<float>((v.y * scale.y) + pos.y);
								vert.x = MSB4<float>((v.z * scale.z) + pos.z);
							} else {
								Vec3D v = attM->origVertices[a].pos;
								MakeModelFaceForwards(v,true);
								vert.z = MSB4<float>((v.x * scale.x) + pos.x);
								vert.y = MSB4<float>((v.y * scale.y) + pos.y);
								vert.x = MSB4<float>((v.z * scale.z) - pos.z);
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
			}
		}

		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
				wxLogMessage(_T("Loaded Attached 2nd Child Model %s for export."),mAttChild->modelname.c_str());

				if (mAttChild){
					int boneID = -1;
					Model *mParent = NULL;

					if (att2->parent) {
						mParent = static_cast<Model*>(att2->children[j]->parent->model);
						if (mParent)
							boneID = mParent->attLookup[att2->children[j]->id];
					}
					Vec3D pos(0,0,0);
					Vec3D scale(1,1,1);
					if (boneID>-1) {
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
							scale.x = mat.m[0][0];
							scale.y = mat.m[1][1];
							scale.z = mat.m[2][2];

							mat.translation(cbone.transPivot);

							pos.x = mat.m[0][3];
							pos.y = mat.m[1][3];
							pos.z = mat.m[2][3];
						}
					}
					MakeModelFaceForwards(pos,false);

					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							wxLogMessage(_T("Exporting Point data for Attached 2nd Child..."));
							for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
								uint16 a = mAttChild->indices[b];
								Vec3D vert;
								if ((init == false)&&(mAttChild->vertices)) {
									Vec3D v = mAttChild->vertices[a];
									MakeModelFaceForwards(v,true);
									vert.z = MSB4<float>(0-((v.x * scale.x) + pos.x));
									vert.y = MSB4<float>((v.y * scale.y) + pos.y);
									vert.x = MSB4<float>((v.z * scale.z) + pos.z);
								} else {
									Vec3D v = mAttChild->origVertices[a].pos;
									MakeModelFaceForwards(v,true);
									vert.z = MSB4<float>(0-((v.x * scale.x) + pos.x));
									vert.y = MSB4<float>((v.y * scale.y) + pos.y);
									vert.x = MSB4<float>((v.z * scale.z) + pos.z);
								}
								MakeModelFaceForwards(vert,false);
								f.Write(reinterpret_cast<char *>(&vert.x), 4);
								f.Write(reinterpret_cast<char *>(&vert.y), 4);
								f.Write(reinterpret_cast<char *>(&vert.z), 4);
								pointsSize += 12;
								numVerts++;
							}
							numGroups++;
						}
					}
				}
			}
		}
		wxLogMessage(_T("Finished Attachment Point Data"));
	}

	fileLen += pointsSize;
	off_T = -4-pointsSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 Point Data Written for %s"),m->modelname.c_str());
#endif

/*
	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write(_T("BBOX"), 4);
	u32 = MSB4<uint32>(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = MSB4<float>(m->header.ps.BoundingBox[0].x);
	vert.y = MSB4<float>(m->header.ps.BoundingBox[0].y);
	vert.z = MSB4<float>(m->header.ps.BoundingBox[0].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = MSB4<float>(m->header.ps.BoundingBox[1].x);
	vert.y = MSB4<float>(m->header.ps.BoundingBox[1].y);
	vert.z = MSB4<float>(m->header.ps.BoundingBox[1].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	// ================
*/

	// --
	uint32 vmapSize = 0;


	//Vertex Mapping
	f.Write(_T("VMAP"), 4);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	// UV Data
	f.Write(_T("TXUV"), 4);
	dimension = MSB2(2);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmapSize += 14;

	counter = 0;

	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)){
			for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
				uint16 a = m->indices[b];

				LW_WriteVX(f,counter,vmapSize);

				f32 = MSB4<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				vmapSize += 8;
				counter++;
			}
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
							uint16 a = attM->indices[b];

							LW_WriteVX(f,counter,vmapSize);

							f32 = MSB4<float>(attM->origVertices[a].texcoords.x);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							f32 = MSB4<float>(1 - attM->origVertices[a].texcoords.y);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							vmapSize += 8;
							counter++;
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
								uint16 a = mAttChild->indices[b];

								LW_WriteVX(f,counter,vmapSize);

								f32 = MSB4<float>(mAttChild->origVertices[a].texcoords.x);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								f32 = MSB4<float>(1 - mAttChild->origVertices[a].texcoords.y);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								vmapSize += 8;
								counter++;
							}
						}
					}
				}
			}
		}
	}

	fileLen += vmapSize;

	off_T = -4-vmapSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 UV data Written for %s"),m->modelname.c_str());
#endif

	//m->header
	//Vertex Mapping
	/*
	for (uint16 b=0;b<m->header.nBones;b++){
		f.Write(_T("VMAP"), 4);
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;
		// UV Data
		f.Write(_T("WGHT"), 4);
		dimension = MSB2(1);
		f.Write(reinterpret_cast<char *>(&dimension), 2);
		f.Write(_T("Texture"), 7);
		ub = 0;
		f.Write(reinterpret_cast<char *>(&ub), 1);
		vmapSize += 14;

		counter = 0;

		for (uint32 i=0; i<m->passes.size(); i++) {
			ModelRenderPass &p = m->passes[i];

			if (p.init(m)){
				for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
					uint16 a = m->indices[b];

					LW_WriteVX(f,counter,vmapSize);

					f32 = MSB4<float>(m->origVertices[a].weights[0]);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					f32 = MSB4<float>(1 - m->origVertices[a].texcoords.y);
					f.Write(reinterpret_cast<char *>(&f32), 4);
					vmapSize += 8;
					counter++;
				}
			}
		}
	}
	*/
	/*
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
							uint16 a = attM->indices[b];

							LW_WriteVX(f,counter,vmapSize);

							f32 = MSB4<float>(attM->origVertices[a].texcoords.x);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							f32 = MSB4<float>(1 - attM->origVertices[a].texcoords.y);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							vmapSize += 8;
							counter++;
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for(uint32 k=0, b=p.indexStart;k<p.indexCount;k++,b++) {
								uint16 a = mAttChild->indices[b];

								LW_WriteVX(f,counter,vmapSize);

								f32 = MSB4<float>(mAttChild->origVertices[a].texcoords.x);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								f32 = MSB4<float>(1 - mAttChild->origVertices[a].texcoords.y);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								vmapSize += 8;
								counter++;
							}
						}
					}
				}
			}
		}
	}

	fileLen += vmapSize;

	off_T = -4-vmapSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(vmapSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	*/
#ifdef _DEBUG
	wxLogMessage(_T("M2 Weight data Written for %s"),m->modelname.c_str());
#endif


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

	f.Write(_T("POLS"), 4);
	uint32 polySize = 4;
	u32 = MSB4<uint32>(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8; // FACE is handled in the PolySize
	f.Write(_T("FACE"), 4);

	counter = 0;
	
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)) {
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				uint16 nverts;

				// Write the number of Verts
				nverts = MSB2(3);
				f.Write(reinterpret_cast<char *>(&nverts),2);
				polySize += 2;

				for (int x=0;x<3;x++,counter++){
					//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
					//uint16 indice16;
					//uint32 indice32;

					int mod = 0;
					if (x==1){
						mod = 1;
					}else if (x==2){
						mod = -1;
					}

					LW_WriteVX(f,counter+mod,polySize);
				}
			}
		}
	}
	
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (unsigned int k=0; k<p.indexCount; k+=3) {
							uint16 nverts;

							// Write the number of Verts
							nverts = MSB2(3);
							f.Write(reinterpret_cast<char *>(&nverts),2);
							polySize += 2;

							for (int x=0;x<3;x++,counter++){
								//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
								//uint16 indice16;
								//uint32 indice32;

								int mod = 0;
								if (x==1){
									mod = 1;
								}else if (x==2){
									mod = -1;
								}

								LW_WriteVX(f,counter+mod,polySize);
							}
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (unsigned int k=0; k<p.indexCount; k+=3) {
								uint16 nverts;

								// Write the number of Verts
								nverts = MSB2(3);
								f.Write(reinterpret_cast<char *>(&nverts),2);
								polySize += 2;

								for (int x=0;x<3;x++,counter++){
									//wxLogMessage("Batch %i, index %i, x=%i",b,k,x);
									//uint16 indice16;
									//uint32 indice32;

									int mod = 0;
									if (x==1){
										mod = 1;
									}else if (x==2){
										mod = -1;
									}

									LW_WriteVX(f,counter+mod,polySize);
								}
							}
						}
					}
				}
			}
		}
	}

	off_T = -4-polySize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(polySize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	fileLen += polySize;
	// ========
#ifdef _DEBUG
	wxLogMessage(_T("M2 Polygons Written for %s"),m->modelname.c_str());
#endif

	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.

	// NOTE: Every PTAG type needs a seperate PTAG call!

	TagCounter = 0;
	PartCounter = 0;
	SurfCounter = 0;
	counter = 0;
	uint32 ptagSize;

	// Parts PolyTag
	f.Write(_T("PTAG"), 4);
	ptagSize = 4;
	counter=0;
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write(_T("PART"), 4);
	for (unsigned int i=0;i<m->passes.size();i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)){
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				LW_WriteVX(f,counter,ptagSize);

				u16 = MSB2(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
			TagCounter++;
			PartCounter++;
		}
	}
	
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (unsigned int k=0; k<p.indexCount; k+=3) {
							LW_WriteVX(f,counter,ptagSize);

							u16 = MSB2(TagCounter);
							f.Write(reinterpret_cast<char *>(&u16), 2);
							ptagSize += 2;
							counter++;
						}
						TagCounter++;
						PartCounter++;
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (unsigned int k=0; k<p.indexCount; k+=3) {
								LW_WriteVX(f,counter,ptagSize);

								u16 = MSB2(TagCounter);
								f.Write(reinterpret_cast<char *>(&u16), 2);
								ptagSize += 2;
								counter++;
							}
							TagCounter++;
							PartCounter++;
						}
					}
				}
			}
		}
	}


	fileLen += ptagSize;

	off_T = -4-ptagSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// Surface PolyTag
	counter=0;
	ptagSize = 4;
	f.Write(_T("PTAG"), 4);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write(_T("SURF"), 4);
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				LW_WriteVX(f,counter,ptagSize);

				u16 = MSB2(TagCounter);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				ptagSize += 2;
				counter++;
			}
			TagCounter++;
			SurfCounter++;
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (unsigned int k=0; k<p.indexCount; k+=3) {
							LW_WriteVX(f,counter,ptagSize);

							u16 = MSB2(TagCounter);
							f.Write(reinterpret_cast<char *>(&u16), 2);
							ptagSize += 2;
							counter++;
						}
						TagCounter++;
						SurfCounter++;
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (unsigned int k=0; k<p.indexCount; k+=3) {
								LW_WriteVX(f,counter,ptagSize);

								u16 = MSB2(TagCounter);
								f.Write(reinterpret_cast<char *>(&u16), 2);
								ptagSize += 2;
								counter++;
							}
							TagCounter++;
							SurfCounter++;
						}
					}
				}
			}
		}
	}
	
	fileLen += ptagSize;

	off_T = -4-ptagSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(ptagSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);


	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 PTag Surface data Written for %s"),m->modelname.c_str());
#endif

	// --
	uint32 vmadSize = 0;
	f.Write(_T("VMAD"), 4);
	u32 = MSB4<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;
	f.Write(_T("TXUV"), 4);
	dimension = 2;
	dimension = MSB2(dimension);
	f.Write(reinterpret_cast<char *>(&dimension), 2);
	f.Write(_T("Texture"), 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	vmadSize += 14;

	counter = 0;
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				int a = m->indices[b];

				LW_WriteVX(f,counter,vmadSize);
				LW_WriteVX(f,(counter/3),vmadSize);

				f32 = MSB4<float>(m->origVertices[a].texcoords.x);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f32 = MSB4<float>(1 - m->origVertices[a].texcoords.y);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				counter++;
				vmadSize += 8;
			}
		}
	}
	
	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
							uint16 a = attM->indices[b];

							LW_WriteVX(f,counter,vmadSize);
							LW_WriteVX(f,(counter/3),vmadSize);

							f32 = MSB4<float>(attM->origVertices[a].texcoords.x);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							f32 = MSB4<float>(1 - attM->origVertices[a].texcoords.y);
							f.Write(reinterpret_cast<char *>(&f32), 4);
							counter++;
							vmadSize += 8;
						}
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							for (uint32 k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
								uint16 a = mAttChild->indices[b];

								LW_WriteVX(f,counter,vmadSize);
								LW_WriteVX(f,(counter/3),vmadSize);

								f32 = MSB4<float>(mAttChild->origVertices[a].texcoords.x);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								f32 = MSB4<float>(1 - mAttChild->origVertices[a].texcoords.y);
								f.Write(reinterpret_cast<char *>(&f32), 4);
								counter++;
								vmadSize += 8;
							}
						}
					}
				}
			}
		}
	}

	fileLen += vmadSize;
	off_T = -4-vmadSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(vmadSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 VMAD data Written for %s"),m->modelname.c_str());
#endif

	

	// --
	uint32 surfaceCounter = PartCounter;
	
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			int clipSize = 0;
			f.Write(_T("CLIP"), 4);
			u32 = MSB4<uint32>(0);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			u32 = MSB4<uint32>(++surfaceCounter);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write(_T("STIL"), 4);
			clipSize += 8;

#ifdef _DEBUG
			if (p.useTex2 != false){
				wxLogMessage(_T("Pass %i uses Texture 2!"),i);
			}
#endif
			
			wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
			wxString texName = m->TextureList[p.tex].BeforeLast(_T('.'));
			wxString texPath = texName.BeforeLast(SLASH);
			if (m->modelType == MT_CHAR){
				texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_") + texName.AfterLast(SLASH);
			}else if (texName.Find(SLASH) <= 0 && texName.EndsWith(_T("Cape"))){
				texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
				texPath = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			}else if (texName.Find(SLASH) <= 0){
				texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_") + texName;
				texPath = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			}else{
				texName = texName.AfterLast(SLASH);
			}

			if (texName.Length() == 0)
				texName << wxString(m->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),i);

			wxString sTexName = wxEmptyString;
			if (modelExport_LW_PreserveDir == true){
				sTexName << _T("Images") << SLASH;
			}
			if (m->modelType != MT_CHAR){
				if (modelExport_PreserveDir == true){
					sTexName += texPath;
					sTexName << SLASH;
					sTexName.Replace(_T("\\"),_T("/"));
				}
			}
			sTexName += texName;

			sTexName << _T(".tga") << _T('\0');

			if (fmod((float)sTexName.length(), 2.0f) > 0)
				sTexName.Append(_T('\0'));

			u16 = MSB2((unsigned short)sTexName.length());
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(sTexName.data(), sTexName.length());
			clipSize += (2+(int)sTexName.length());

			// update the chunks length
			off_T = -4-clipSize;
			f.SeekO(off_T, wxFromCurrent);
			u32 = MSB4<uint32>(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			// save texture to file
			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast(SLASH);
			texFilename += SLASH;
			texFilename += texName;

			if (modelExport_LW_PreserveDir == true){
				wxString Path, Name;

				Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
				Name << texFilename.AfterLast(SLASH);

				MakeDirs(Path,_T("Images"));

				texFilename.Empty();
				texFilename << Path << SLASH<<_T("Images")<<SLASH << Name;
			}
			if (m->modelType != MT_CHAR){
				if (modelExport_PreserveDir == true){
					wxString Path1, Path2, Name;
					Path1 << texFilename.BeforeLast(SLASH);
					Name << texName.AfterLast(SLASH);
					Path2 << texPath;

					MakeDirs(Path1,Path2);

					texFilename.Empty();
					texFilename << Path1 << SLASH << Path2 << SLASH << Name;
				}
			}
			if (texFilename.Length() == 0){
				texFilename << wxString(m->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),i);
			}
			texFilename << _T(".tga");
			wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
			SaveTexture(texFilename);

			fileLen += clipSize;
		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						int clipSize = 0;
						f.Write(_T("CLIP"), 4);
						u32 = MSB4<uint32>(0);
						f.Write(reinterpret_cast<char *>(&u32), 4);
						fileLen += 8;

						u32 = MSB4<uint32>(++surfaceCounter);
						f.Write(reinterpret_cast<char *>(&u32), 4);
						f.Write(_T("STIL"), 4);
						clipSize += 8;

						wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
						wxString texName = attM->TextureList[p.tex].BeforeLast(_T('.'));
						wxString texPath = texName.BeforeLast(SLASH);
						if (texName.AfterLast(SLASH) == _T("Cape")){
							texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + wxString(attM->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
							texPath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
						}else{
							texName = texName.AfterLast(SLASH);
						}

						if (texName.Length() == 0){
							texName << wxString(attM->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast('.');
							texPath = wxString(attM->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
						}

/*
						//texName = attM->TextureList[p.tex];

						if ((texName.Find(SLASH) <= 0)&&(texName == _T("Cape"))){
							texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
							texPath = wxString(attM->name).BeforeLast(SLASH);
						}else{
							texName = texName.AfterLast(SLASH);
						}

						if (texName.Length() == 0)
							texName << wxString(attM->modelname).AfterLast(SLASH).BeforeLast(_T('.')) << wxString::Format(_T("_Image_%03i"),i);
*/
						wxString sTexName = wxEmptyString;
						if (modelExport_LW_PreserveDir == true){
							sTexName << _T("Images") << SLASH;
						}
						if (modelExport_PreserveDir == true){
							sTexName += texPath;
							sTexName << SLASH;
							sTexName.Replace(_T("\\"),_T("/"));
						}
						sTexName += texName;

						sTexName << _T(".tga") << _T('\0');
						if (fmod((float)sTexName.length(), 2.0f) > 0)
							sTexName.Append(_T('\0'));

						u16 = MSB2((unsigned short)sTexName.length());
						f.Write(reinterpret_cast<char *>(&u16), 2);
						f.Write(sTexName.data(), sTexName.length());
						clipSize += (2+(int)sTexName.length());

						// update the chunks length
						off_T = -4-clipSize;
						f.SeekO(off_T, wxFromCurrent);
						u32 = MSB4<uint32>(clipSize);
						f.Write(reinterpret_cast<char *>(&u32), 4);
						f.SeekO(0, wxFromEnd);

						// save texture to file
						wxString texFilename(fn, wxConvUTF8);
						texFilename = texFilename.BeforeLast(SLASH);
						texFilename += SLASH;
						texFilename += texName;

						if (modelExport_LW_PreserveDir == true){
							wxString Path, Name;

							Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
							Name << texFilename.AfterLast(SLASH);

							MakeDirs(Path,_T("Images"));

							texFilename.Empty();
							texFilename << Path << SLASH<<_T("Images")<<SLASH << Name;
						}
						if (modelExport_PreserveDir == true){
							wxString Path1, Path2, Name;
							Path1 << texFilename.BeforeLast(SLASH);
							Name << texName.AfterLast(SLASH);
							Path2 << texPath;

							MakeDirs(Path1,Path2);

							texFilename.Empty();
							texFilename << Path1 << SLASH << Path2 << SLASH << Name;
						}
						
						if (texFilename.Length() == 0){
							texFilename << wxString(attM->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast('.') << wxString::Format(_T("_Image_%03i"),i);
						}
						texFilename << _T(".tga");
						wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
						SaveTexture(texFilename);

						fileLen += clipSize;
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 k=0; k<mAttChild->passes.size(); k++) {
						ModelRenderPass &p = mAttChild->passes[k];

						if (p.init(mAttChild)) {
							int clipSize = 0;
							f.Write(_T("CLIP"), 4);
							u32 = MSB4<uint32>(0);
							f.Write(reinterpret_cast<char *>(&u32), 4);
							fileLen += 8;

							u32 = MSB4<uint32>(++surfaceCounter);
							f.Write(reinterpret_cast<char *>(&u32), 4);
							f.Write(_T("STIL"), 4);
							clipSize += 8;

							wxString FilePath = wxString(fn, wxConvUTF8).BeforeLast(SLASH);
							wxString texName = mAttChild->TextureList[p.tex].BeforeLast(_T('.'));
							wxString texPath = texName.BeforeLast(SLASH);
							if (texName.AfterLast(SLASH) == _T("Cape")){
								//texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + wxString(mAttChild->name).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
								texName = wxString(mAttChild->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
								texPath = wxString(mAttChild->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
							}else{
								texName = texName.AfterLast(SLASH);
							}

							if (texName.Length() == 0){
								texName << wxString(mAttChild->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
								texPath = wxString(mAttChild->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
							}

							wxString sTexName = wxEmptyString;
							if (modelExport_LW_PreserveDir == true){
								sTexName << _T("Images") << SLASH;
							}
							if (modelExport_PreserveDir == true){
								sTexName += texPath;
								sTexName << SLASH;
								sTexName.Replace(_T("\\"),_T("/"));
							}
							sTexName += texName;

							sTexName << _T(".tga") << _T('\0');
							if (fmod((float)sTexName.length(), 2.0f) > 0)
								sTexName.Append(_T('\0'));

							u16 = MSB2((unsigned short)sTexName.length());
							f.Write(reinterpret_cast<char *>(&u16), 2);
							f.Write(sTexName.data(), sTexName.length());
							clipSize += (2+(int)sTexName.length());

							// update the chunks length
							off_T = -4-clipSize;
							f.SeekO(off_T, wxFromCurrent);
							u32 = MSB4<uint32>(clipSize);
							f.Write(reinterpret_cast<char *>(&u32), 4);
							f.SeekO(0, wxFromEnd);

							// save texture to file
							wxString texFilename(fn, wxConvUTF8);
							texFilename = texFilename.BeforeLast(SLASH);
							texFilename += SLASH;
							texFilename += texName;

							if (modelExport_LW_PreserveDir == true){
								wxString Path, Name;

								Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
								Name << texFilename.AfterLast(SLASH);

								MakeDirs(Path,_T("Images"));

								texFilename.Empty();
								texFilename << Path << SLASH<<_T("Images")<<SLASH << Name;
							}
							if (modelExport_PreserveDir == true){
								wxString Path1, Path2, Name;
								Path1 << texFilename.BeforeLast(SLASH);
								Name << texName.AfterLast(SLASH);
								Path2 << texPath;

								MakeDirs(Path1,Path2);

								texFilename.Empty();
								texFilename << Path1 << SLASH << Path2 << SLASH << Name;
							}
							
							if (texFilename.Length() == 0){
								texFilename << wxString(attM->modelname.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast('.') << wxString::Format(_T("_Image_%03i"),i);
							}
							texFilename << _T(".tga");
							wxLogMessage(_T("Exporting Image: %s"),texFilename.c_str());
							SaveTexture(texFilename);

							fileLen += clipSize;
						}
					}
				}
			}
		}
	}

	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 Images & Image Data Written for %s"),m->modelname.c_str());
#endif

	// --
	wxString surfName;
	surfaceCounter = PartCounter;
	for (uint32 i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			surfaceCounter++;

			// Surface name
			surfName = m->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));
			if (surfName.Len() == 0)
				surfName = wxString::Format(_T("Material_%03i"),p.tex);

			Vec4D color = p.ocol;
			float reflect = 0.0f;
			if (p.useEnvMap)
				reflect = 0.5f;

			wxString cmnt;
			if (m->modelType == MT_CHAR){
				cmnt = wxString::Format(_T("Character Material %03i"),p.tex);
			}else{
				cmnt = m->TextureList[p.tex].BeforeLast(_T('.')) + _T(".TGA");
			}

			// If double-sided...
			bool cull = true;
			if (p.cull == false)
				cull = false;
			LW_WriteSurface(f,surfName,color,reflect,cull,false,surfaceCounter,cmnt,fileLen);

		}
	}

	if (att!=NULL){
		Model *attM = NULL;
		if (att->model) {
			attM = static_cast<Model*>(att->model);

			if (attM){
				for (uint32 i=0; i<attM->passes.size(); i++) {
					ModelRenderPass &p = attM->passes[i];

					if (p.init(attM)) {
						surfaceCounter++;

						// Surface name
						surfName = attM->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));
						if (surfName.Len() == 0)
							surfName = wxString::Format(_T("Material_%03i"),p.tex);
						Vec4D color = p.ocol;
						float reflect = 0.0f;
						if (p.useEnvMap)
							reflect = 0.5f;

						wxString cmnt = attM->TextureList[p.tex].BeforeLast(_T('.')) + _T(".TGA");

						// If double-sided...
						bool cull = true;
						if (p.cull == false)
							cull = false;
						LW_WriteSurface(f,surfName,color,reflect,cull,false,surfaceCounter,cmnt,fileLen);
					}
				}
			}
		}
		for (uint32 i=0; i<att->children.size(); i++) {
			Attachment *att2 = att->children[i];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							surfaceCounter++;

							// Comment
							wxString cmnt = mAttChild->TextureList[p.tex].BeforeLast(_T('.')) + _T(".TGA");

							// Surface name
							surfName = mAttChild->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));
							int thisslot = att2->children[j]->slot;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								if (surfName == _T("Cape")) {
									wxString tex = mAttChild->name.AfterLast(SLASH).BeforeLast(_T('.'));
									if (tex.Len() > 0){
										surfName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),tex.c_str());
										cmnt = mAttChild->name.BeforeLast(_T('.')) + _T(".tga");
									}else{
										surfName = wxString::Format(_T("%s - Surface"),slots[thisslot].c_str());
										cmnt = _T("Surface");
									}
								}else if (surfName != wxEmptyString){
									surfName = wxString::Format(_T("%s - %s"),slots[thisslot].c_str(),surfName.c_str());
								}else{
									surfName = wxString::Format(_T("%s - Material %02i"),slots[thisslot].c_str(),p.tex);
									cmnt = surfName;
								}
							}
							if (surfName.Len() == 0)
								surfName = wxString::Format(_T("Child %02i - Material %03i"), j, p.tex);

							Vec4D color = p.ocol;
							float reflect = 0.0f;
							if (p.useEnvMap)
								reflect = 0.5f;

							// If double-sided...
							bool cull = true;
							if (p.cull == false)
								cull = false;

							LW_WriteSurface(f,surfName,color,reflect,cull,false,surfaceCounter,cmnt,fileLen);
						}
					}
				}
			}
		}
	}
	// ================
#ifdef _DEBUG
	wxLogMessage(_T("M2 Surface Data Written for %s"),m->modelname.c_str());
#endif

	f.SeekO(4, wxFromStart);
	u32 = MSB4<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();
	wxLogMessage(_T("M2 %s Successfully written!"),m->modelname.c_str());

	ExportM2toScene(m,fn,init);
}

//---------------------------------------------
// --==WMOs==--
//---------------------------------------------

/*
	This will export Lights & Doodads (as nulls) into a Lightwave Scene file.
*/
void ExportWMOObjectstoLWO(WMO *m, const char *fn){
	// Should we generate a scene file?
	// Wll only generate if there are doodads or lights.
	bool doreturn = false;
	if (((modelExport_LW_ExportLights == false) || (m->nLights == 0)) && ((modelExport_LW_ExportDoodads == false) || (m->nDoodads == 0))){
		doreturn = true;
	}
	if (doreturn == true)
		return;

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast(_T('.'));
	SceneName << _T(".lws");

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);

		MakeDirs(Path,_T("Scenes"));

		SceneName.Empty();
		SceneName << Path << SLASH << _T("Scenes") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		SceneName.Empty();
		SceneName << Path1 << SLASH << Path2 << SLASH << Name;
	}

	ofstream fs(SceneName.fn_str(), ios_base::out | ios_base::trunc);

	if (!fs.is_open()) {
		wxMessageBox(_T("Unable to open the scene file for exporting."),_T("Scene Export Failure"));
		wxLogMessage(_T("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	SceneName = SceneName.AfterLast(SLASH);

	/*
		Lightwave Scene files are simple text files. New Lines (\n) are need to add a new variable for the scene to understand.
		Lightwave files are pretty sturdy. Variables not already in a scene or model file, will usually be generated when opened.
		As such, we don't need to declare EVERY variable for the scene file, but I'm gonna add what I think is pertinent.
	*/

	// File Top
	fs << _T("LWSC\n");
	fs << _T("5\n\n"); // I think this is a version-compatibility number...

	// Scene File Basics
	fs << _T("RenderRangeType 0\nFirstFrame 1\nLastFrame 60\n");
	fs << _T("FrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary 1-60\n");
	fs << _T("PreviewFirstFrame 0\nPreviewLastFrame 60\nPreviewFrameStep 1\nCurrentFrame 0\nFramesPerSecond 30\nChangeScene 0\n\n");

	uint32 mcount = 0; // Model Count
	uint32 lcount = 0; // Light Count
	Vec3D ZeroPos(0,0,0);
	Vec3D ZeroRot(0,0,0);
	Vec3D OneScale(1,1,1);

	uint32 DoodadLightArrayID[3000];
	uint32 DoodadLightArrayDDID[3000];
	uint32 DDLArrCount = 0;

	// Objects/Doodads go here

	// Exported Object
	int ModelID = mcount;
	wxString Obj = wxString(fn, wxConvUTF8).AfterLast(SLASH);
	wxString objFilename = wxEmptyString;
	if (modelExport_LW_PreserveDir == true){
		objFilename << _T("Objects") << SLASH;
	}
	if (modelExport_PreserveDir == true){
		objFilename += wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		objFilename << SLASH;
		objFilename.Replace(_T("\\"),_T("/"));
	}
	objFilename += Obj;

	AnimationData ObjData;
	ObjData.Push(ZeroPos,ZeroRot,OneScale,0);

	WriteLWSceneObject(fs,objFilename,ObjData,mcount);

	if (modelExport_LW_ExportDoodads ==  true){
		// Doodads
		for (uint32 ds=0;ds<m->nDoodadSets;ds++){			
			m->showDoodadSet(ds);
			WMODoodadSet *DDSet = &m->doodadsets[ds];
			wxString DDSetName;
			DDSetName << wxString(m->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) << wxString(_T(" DoodadSet ")) << wxString(DDSet->name, wxConvUTF8);
			int DDSID = mcount;

			AnimationData DDData;
			DDData.Push(ZeroPos,(ZeroRot+(float)(PI/2)),OneScale,0);
			WriteLWSceneObject(fs,DDSetName,DDData,mcount,7,true,ModelID);

			for (uint32 dd=DDSet->start;dd<(DDSet->start+DDSet->size);dd++){
				WMOModelInstance *doodad = &m->modelis[dd];
				wxString name = wxString(doodad->filename.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.'));
				// Position
				Vec3D Pos = doodad->pos;
				// Heading, Pitch & Bank.
				Vec3D Rot = QuaternionToXYZ(doodad->dir,doodad->w);
				Rot.x -= (float)(PI/2);

				int DDID = mcount;
				bool isNull = true;
				if (modelExport_LW_DoodadsAs > 0)
					isNull = false;
				
				if (!doodad->model){
					isNull = true;
				}

				if (isNull == false){
					wxString pathdir = wxEmptyString;
					if (modelExport_LW_PreserveDir == true){
						pathdir << _T("Objects") << SLASH;
					}
					name = pathdir << wxString(doodad->filename.c_str(), wxConvUTF8).BeforeLast(_T('.')) << _T(".lwo");
					name.Replace(_T("\\"),_T("/"));
				}

				AnimationData DoodadData;
				DoodadData.Push(Pos,Rot,Vec3D(doodad->sc,doodad->sc,doodad->sc),0);

				WriteLWSceneObject(fs,name,DoodadData,mcount,7,isNull,DDSID,wxString(doodad->filename.c_str(), wxConvUTF8));
				wxLogMessage(_T("Export: Finished writing the Doodad to the Scene File."));

				// Doodad Lights
				// Apparently, Doodad Lights must be parented to the Doodad for proper placement.
				if ((doodad->model) && (doodad->model->header.nLights > 0)){
					wxLogMessage(_T("Export: Doodad Lights found for %s, Number of lights: %i"), doodad->filename.c_str(), doodad->model->header.nLights);
					DoodadLightArrayDDID[DDLArrCount] = DDID;
					DoodadLightArrayID[DDLArrCount] = dd;
					DDLArrCount++;
				}
			}
		}
	}

	// Lighting Basics
	fs << _T("AmbientColor 1 1 1\nAmbientIntensity 0.25\nDoubleSidedAreaLights 1\n\n");

	// Lights
	if (modelExport_LW_ExportLights == true){
		// WMO Lights
		for (int i=0;i<m->nLights;i++){
			WMOLight *light = &m->lights[i];

			Vec3D color;
			color.x=light->fcolor.x;
			color.y=light->fcolor.y;
			color.z=light->fcolor.z;
			float intense = light->intensity;
			bool useAtten = false;
			float AttenEnd = light->attenEnd;

			if (light->useatten > 0){
				useAtten = true;
			}

			WriteLWSceneLight(fs,lcount,light->pos,light->type,color,intense,useAtten,AttenEnd,2.5);
		}

		// Doodad Lights
		for (unsigned int i=0;i<DDLArrCount;i++){
			
			WMOModelInstance *doodad = &m->modelis[DoodadLightArrayID[i]];
			ModelLight *light = &doodad->model->lights[i];

			if (light->type < 0){
				continue;
			}
			
			Vec3D color = light->diffColor.getValue(0,0);
			float intense = light->diffIntensity.getValue(0,0);
			bool useAtten = false;
			float AttenEnd = light->AttenEnd.getValue(0,0);
			wxString name = wxString(doodad->filename.c_str(), wxConvUTF8).AfterLast(SLASH);

			if (light->UseAttenuation.getValue(0,0) > 0){
				useAtten = true;
			}
			
			WriteLWSceneLight(fs,lcount,light->pos,light->type,color,intense,useAtten,AttenEnd,5,name,DoodadLightArrayDDID[i]);
		}
	}

	// Camera data (if we want it) goes here.
	// Yes, we can export flying cameras to Lightwave!
	// Just gotta add them back into the listing...
	fs << _T("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nZoomFactor 2.316845\nZoomType 2\n\n");
	WriteLWScenePlugin(fs,_T("CameraHandler"),1,_T("Perspective"));	// Make the camera a Perspective camera

	// Backdrop Settings
	// Add this if viewing a skybox, or using one as a background?

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << _T("RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n");

	fs.close();

	// Export Doodad Files
	wxString cWMOName = m->name;
	if (modelExport_LW_ExportDoodads ==  true){
		if (modelExport_LW_DoodadsAs == 1){
			// Copy Model-list into an array
			wxArrayString modelarray = m->models;

			// Remove the WMO
			wxDELETE(g_modelViewer->canvas->wmo);
			g_modelViewer->canvas->wmo = NULL;
			g_modelViewer->isWMO = false;
			g_modelViewer->isChar = false;
			
			// Export Individual Doodad Models
			for (unsigned int x=0;x<modelarray.size();x++){
				g_modelViewer->isModel = true;
				wxString cModelName(modelarray[x].c_str(),wxConvUTF8);

				wxLogMessage(_T("Export: Attempting to export doodad model: %s"),cModelName.c_str());
				wxString dfile = wxString(fn,wxConvUTF8).BeforeLast(SLASH) << SLASH << cModelName.AfterLast(SLASH);
				dfile = dfile.BeforeLast(_T('.')) << _T(".lwo");

				g_modelViewer->canvas->LoadModel(cModelName);
				ExportM2toLWO(NULL, g_modelViewer->canvas->model, dfile.fn_str(), true);

				wxLogMessage(_T("Export: Finished exporting doodad model: %s\n\n"),cModelName.c_str());

				// Delete the loaded model
				g_modelViewer->canvas->clearAttachments();
				g_modelViewer->canvas->model = NULL;
				g_modelViewer->isModel = false;
			}
		}
		//texturemanager.clear();

		// Reload our original WMO file.
		//wxLogMessage("Reloading original WMO file: %s",cWMOName.c_str());

		// Load the WMO


	}
}

// -----------------------------------------
// New Lightwave Stuff
//
// Under construction, only visible/usable while in Debug Mode.
// -----------------------------------------


// Seperated out the Writing function, so we don't have to write it all out every time we want to export something.
// Should probably do something similar with the other exporting functions as well...
bool WriteLWObject(wxString filename, LWObject Object) {

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.


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
			VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size (2 * 4 bytes).
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
				VMPA		// Discontinuous Vertex Map Parameters (See the one in the Points section for details)
					VMAD		// Discontinuous Vector Map Section. Best if used only for UV Maps. Difference between VMAP & VMAD: VMAPs are connected to points, while VMADs are connected to Polys.
						APSL	// Adaptive Pixel Subdivision Level. Only needed for sub-patched models, so just ignore it for our outputs.
						TXUV	// Defines UV Vector Map
						NORM	// Define's a poly's Normals
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
		VCOL	// 2 bytes, size | 1.0f (4bytes) | Zero (2 bytes) | Map Type | MapName
			RGB		// Vertex Color Map Type without Alpha
			RGBA	// Vertex Color Map Type with an Alpha
		GVAL	// Glow
		SHRP	// Diffuse Sharpness

		SMAN	// Smoothing Amount
		NORM	// Specifies the Normal Map to use

		RFOP	// Reflection Options: 0-Backdrop Only (default), 1-Raytracing + Backdrop, 2 - Spherical Map, 3 - Raytracing + Spherical Map
		TROP	// Same as RFOP, but for Refraction.
		SIDE	// Is it Double-Sided?
		NVSK	// Exclude from VStack

		CMNT // Surface Comment. 2bytes: Size. Simple Text line for this surface. Make sure it doesn't end on an odd byte! VERS must be 931 or 950!
		VERS // Version Compatibility mode, including what it's compatible with. 2 bytes (int16, value 4), 4 bytes (int32, value is 850 for LW8.5 Compatability, 931 for LW9.3.1, and 950 for Default)

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

	// Check to see if we have any data to generate this file.
	if ((Object.Layers.size() < 1) || (Object.Layers[0].Points.size() < 1)){
		wxMessageBox(_T("No Layer Data found.\nUnable to write object file."),_T("Error"));
		wxLogMessage(_T("Error: No Layer Data. Unable to write object file."));
		return false;
	}

   	// -----------------------------------------
	// Initial Variables
	// -----------------------------------------

	// File Length
	unsigned int fileLen = 0;

	// Other Declares
	int off_T;
	uint16 zero = 0;

	// Needed Numbers
	uint16 TagCount = (uint16)Object.PartNames.size() + (uint16)Object.Surfaces.size();

	// Open Model File
	wxString file = wxString(filename, wxConvUTF8);
/*
	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,"Objects");

		file.Empty();
		file << Path << SLASH<<_T("Objects")<<SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);
		Path2 << wxString(filename).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << SLASH << Path2 << SLASH << Name;
	}
*/
	wxFFileOutputStream f(file, _T("w+b"));

	if (!f.IsOk()) {
		wxMessageBox(_T("Unable to open file. Could not export model."),_T("Error"));
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file.c_str());
		return false;
	}

	// ===================================================
	// FORM		// Format Declaration
	//
	// Always exempt from the length of the file!
	// ===================================================
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);

	// ===================================================
	// LWO2
	//
	// Declares this is the Lightwave Object 2 file format.
	// LWOB is the first format. It doesn't have a lot of the cool stuff LWO2 has...
	// ===================================================
	f.Write("LWO2", 4);
	fileLen += 4;

	// ===================================================
	// TAGS
	//
	// Used for various Strings. Known string types, in order:
	//		Sketch Color Names
	//		Part Names
	//		Surface Names
	// ===================================================

	if (TagCount > 0) {
		f.Write("TAGS", 4);
		uint32 tagsSize = 0;
		f.Write(reinterpret_cast<char *>(&tagsSize), 4);
		fileLen += 8;

		// Parts
		for (uint32 i=0; i<Object.PartNames.size(); i++){
			wxString PartName = Object.PartNames[i];

			PartName.Append(_T('\0'));
			if (fmod((float)PartName.length(), 2.0f) > 0)
				PartName.Append(_T('\0'));
			f.Write(PartName.data(), PartName.length());
			tagsSize += (uint32)PartName.length();
		}
		// Surfaces
		for (uint32 i=0; i<Object.Surfaces.size(); i++){
			wxString SurfName = Object.Surfaces[i].Name;

			SurfName.Append(_T('\0'));
			if (fmod((float)SurfName.length(), 2.0f) > 0)
				SurfName.Append(_T('\0'));
			f.Write(SurfName.data(), SurfName.length());
			tagsSize += (uint32)SurfName.length();
		}

		// Correct TAGS Length
		off_T = -4-tagsSize;
		f.SeekO(off_T, wxFromCurrent);
		u32 = MSB4<uint32>(tagsSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.SeekO(0, wxFromEnd);

		fileLen += tagsSize;
	}

	// -------------------------------------------------
	// Generate our Layers
	//
	// Point, Poly & Vertex Map data will be nested in
	// our layers.
	// -------------------------------------------------
	for (uint16 l=0;l<(uint16)Object.Layers.size();l++){
		LWLayer cLyr = Object.Layers[l];
		// Define a Layer & It's data
		if (cLyr.Name.length() > 0)
			cLyr.Name.Append(_T('\0'));
		if (fmod((float)cLyr.Name.length(), 2.0f) > 0)
			cLyr.Name.Append(_T('\0'));
		uint16 LayerNameSize = (uint16)cLyr.Name.length();
		uint32 LayerSize = 16+LayerNameSize;
		if ((cLyr.ParentLayer)&&(cLyr.ParentLayer>-1))
			LayerSize += 2;
		f.Write("LAYR", 4);
		u32 = MSB4<uint32>(LayerSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;

		// Layer Number
		u16 = MSB2(l);
		f.Write(reinterpret_cast<char *>(&u16), 2);
		// Flags
		f.Write(reinterpret_cast<char *>(&zero), 2);
		// Pivot
		f.Write(reinterpret_cast<char *>(&zero), 4);
		f.Write(reinterpret_cast<char *>(&zero), 4);
		f.Write(reinterpret_cast<char *>(&zero), 4);
		// Name
		if (LayerNameSize>0){
			f.Write(cLyr.Name, LayerNameSize);
		}
		// Parent
		if ((cLyr.ParentLayer)&&(cLyr.ParentLayer>-1)){
			int pLyr = MSB2(cLyr.ParentLayer);
			f.Write(reinterpret_cast<char *>(&pLyr), 2);
		}
		fileLen += LayerSize;

		// -------------------------------------------------
		// Points Chunk
		//
		// There will be new Point Chunk for every Layer, so if we go
		// beyond 1 Layer, this should be nested.
		// -------------------------------------------------

		uint32 pointsSize = (uint32)cLyr.Points.size()*12;
		f.Write("PNTS", 4);
		u32 = MSB4<uint32>(pointsSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8 + pointsSize;	// Corrects the filesize...

		// Writes the point data
		for (uint32 i=0; i<(uint32)cLyr.Points.size(); i++) {
			Vec3D Points = cLyr.Points[i].PointData;
			MakeModelFaceForwards(Points,false);		// Face the model Forwards
			Vec3D vert;
			vert.x = MSB4<float>(Points.x);
			vert.y = MSB4<float>(Points.y);
			vert.z = MSB4<float>(Points.z);

			f.Write(reinterpret_cast<char *>(&vert.x), 4);
			f.Write(reinterpret_cast<char *>(&vert.y), 4);
			f.Write(reinterpret_cast<char *>(&vert.z), 4);
		}

		// --== Bounding Box Info ==--
		// Skipping for now.


		// --== Vertex Mapping ==--
		// UV, Weights, Vertex Color Maps, etc.

		// ===================================================
		//VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size, then Num Vars (2) * 4 bytes.
					// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
					// Sketch Color: 0-12; 6-Default Gray
		// ===================================================
		f.Write("VMPA", 4);
		u32 = MSB4<uint32>(8);	// We got 2 Paramaters, * 4 Bytes.
		f.Write(reinterpret_cast<char *>(&u32), 4);
		u32 = 4;				// Across Discontinuous Edges UV Sub Type
		f.Write(reinterpret_cast<char *>(&u32), 4);
		u32 = MSB4<uint32>(6);	// Default Gray
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 16;

		// ===================================================
		// Point UV Data (VMAP)
		// ===================================================
		uint32 vmapSize = 0;
		f.Write("VMAP", 4);
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;
		f.Write("TXUV", 4);
		u16 = MSB2(2);
		f.Write(reinterpret_cast<char *>(&u16), 2);
		f.Write(_T("Texture"), 7);
		f.Write(reinterpret_cast<char *>(&zero), 1);
		vmapSize += 14;

		for (uint32 i=0; i<(uint32)cLyr.Points.size(); i++) {
			LW_WriteVX(f,i,vmapSize);

			Vec2D vert;
			vert.x = MSB4<float>(cLyr.Points[i].UVData.x);
			vert.y = MSB4<float>(-cLyr.Points[i].UVData.y);
			f.Write(reinterpret_cast<char *>(&vert.x), 4);
			f.Write(reinterpret_cast<char *>(&vert.y), 4);
			vmapSize += 8;
		}
		fileLen += vmapSize;
		off_T = -4-vmapSize;
		f.SeekO(off_T, wxFromCurrent);
		u32 = MSB4<uint32>(vmapSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		f.SeekO(0, wxFromEnd);

		// ===================================================
		// Vertex Map
		// ===================================================
		if (cLyr.HasVectorColors == true){
			wxLogMessage(_T("Has Vector Colors"));

			f.Write("VMPA", 4);
			u32 = MSB4<uint32>(8);	// We got 2 Paramaters, * 4 Bytes.
			f.Write(reinterpret_cast<char *>(&u32), 4);
			u32 = 4;				// Across Discontinuous Edges UV Sub Type
			f.Write(reinterpret_cast<char *>(&u32), 4);
			u32 = MSB4<uint32>(6);	// Default Gray
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 16;

			uint32 vmapSize = 0;
			f.Write("VMAP", 4);
			u32 = MSB4<uint32>(vmapSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;
			f.Write("RGBA", 4);
			u16 = MSB2(4);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write("Colors", 6);
			f.Write(reinterpret_cast<char *>(&zero), 2);
			vmapSize += 14;

			for (uint32 i=0;i<(uint32)cLyr.Points.size();i++) {
				float rf,gf,bf,af;
				rf = (float)(cLyr.Points[i].VertexColors.r/255.0f);
				gf = (float)(cLyr.Points[i].VertexColors.g/255.0f);
				bf = (float)(cLyr.Points[i].VertexColors.b/255.0f);
				af = (float)((255.0f-cLyr.Points[i].VertexColors.a)/255.0f);
				//wxLogMessage(_T("Point: %i - R:%f(%i), G:%f(%i), B:%f(%i), A:%f(%i)"),i,rf,cLyr.Points[i].VertexColors.r,gf,cLyr.Points[i].VertexColors.g,bf,cLyr.Points[i].VertexColors.b,af,cLyr.Points[i].VertexColors.a);

				LW_WriteVX(f,i,vmapSize);

				Vec4D vert;
				vert.w = MSB4<float>(rf);
				vert.x = MSB4<float>(gf);
				vert.y = MSB4<float>(bf);
				vert.z = MSB4<float>(af);
				f.Write(reinterpret_cast<char *>(&vert.w), 4);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				vmapSize += 16;
			}
			fileLen += vmapSize;
			off_T = -4-vmapSize;
			f.SeekO(off_T, wxFromCurrent);
			u32 = MSB4<uint32>(vmapSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);
		}


		// --== Polygons ==--
		if (cLyr.Polys.size() > 0){

			// -------------------------------------------------
			// Polygon Chunk
			// -------------------------------------------------
			f.Write("POLS", 4);
			uint32 polySize = 4;
			u32 = MSB4<uint32>(polySize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8; // FACE is handled in the PolySize
			f.Write("FACE", 4);

			for (unsigned int x=0;x<cLyr.Polys.size();x++){
				PolyChunk32 PolyData = cLyr.Polys[x].PolyData;
				uint16 nverts = MSB2(PolyData.numVerts);
				polySize += 2;
				f.Write(reinterpret_cast<char *>(&nverts),2);
				for (int y=0;y<3;y++){
					LW_WriteVX(f,PolyData.indice[y],polySize);
				}
			}

			off_T = -4-polySize;
			f.SeekO(off_T, wxFromCurrent);
			u32 = MSB4<uint32>(polySize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);
			fileLen += polySize;

			// The PTAG chunk associates tags with polygons. In this case, it identifies which part or surface is assigned to each polygon. 
			// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
			// index into the TAGS chunk.

			// NOTE: Every PTAG type needs a seperate PTAG call!

			// -------------------------------------------------
			// Parts PolyTag
			// -------------------------------------------------
			if (Object.PartNames.size() > 0){
				f.Write(_T("PTAG"), 4);
				uint32 ptagSize = 4;
				u32 = MSB4<uint32>(ptagSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				fileLen += 8;
				f.Write(_T("PART"), 4);

				for (unsigned int x=0;x<cLyr.Polys.size();x++){
					LWPoly Poly = cLyr.Polys[x];
					LW_WriteVX(f,x,ptagSize);

					u16 = MSB2(Poly.PartTagID);
					f.Write(reinterpret_cast<char *>(&u16), 2);
					ptagSize += 2;
				}
				fileLen += ptagSize;

				off_T = -4-ptagSize;
				f.SeekO(off_T, wxFromCurrent);
				u32 = MSB4<uint32>(ptagSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				f.SeekO(0, wxFromEnd);
			}

			// -------------------------------------------------
			// Surface PolyTag
			// -------------------------------------------------
			if (Object.Surfaces.size() > 0){
				f.Write(_T("PTAG"), 4);
				uint32 ptagSize = 4;
				u32 = MSB4<uint32>(ptagSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				fileLen += 8;
				f.Write(_T("SURF"), 4);

				for (unsigned int x=0;x<cLyr.Polys.size();x++){
					LWPoly Poly = cLyr.Polys[x];
					LW_WriteVX(f,x,ptagSize);

					u16 = MSB2(Poly.SurfTagID);
					f.Write(reinterpret_cast<char *>(&u16), 2);
					ptagSize += 2;
				}
				fileLen += ptagSize;

				off_T = -4-ptagSize;
				f.SeekO(off_T, wxFromCurrent);
				u32 = MSB4<uint32>(ptagSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				f.SeekO(0, wxFromEnd);
			}

			// --== Poly-Based Vertex Mapping ==--
			if (cLyr.Polys[0].Normals.NormalMapName != wxEmptyString){
				// ===================================================
				//VMPA		// Vertex Map Parameters, Always Preceeds a VMAP & VMAD. 4bytes: Size, then Num Vars (2) * 4 bytes.
							// UV Sub Type: 0-Linear, 1-Subpatched, 2-Linear Corners, 3-Linear Edges, 4-Across Discontinuous Edges.
							// Sketch Color: 0-12; 6-Default Gray
				// ===================================================
				f.Write("VMPA", 4);
				u32 = MSB4<uint32>(8);	// We got 2 Paramaters, * 4 Bytes.
				f.Write(reinterpret_cast<char *>(&u32), 4);
				u32 = 0;				// Linear SubType
				f.Write(reinterpret_cast<char *>(&u32), 4);
				u32 = MSB4<uint32>(6);	// Default Gray
				f.Write(reinterpret_cast<char *>(&u32), 4);
				fileLen += 16;

				uint32 vmadSize = 0;
				f.Write("VMAD", 4);
				u32 = MSB4<uint32>(vmadSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				fileLen += 8;

				f.Write("NORM", 4);
				u16 = MSB2(3);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				vmadSize += 6;
				wxString NormMapName = cLyr.Polys[0].Normals.NormalMapName;
				NormMapName += _T('\0');
				if (fmod((float)NormMapName.length(), 2.0f) > 0)
					NormMapName.Append(_T('\0'));
				f.Write(NormMapName.data(), NormMapName.length());
				vmadSize += (uint32)NormMapName.length();

				for (unsigned int x=0;x<cLyr.Polys.size();x++){
					PolyNormal cNorm = cLyr.Polys[x].Normals;
					for (int n=0;n<3;n++){
						LW_WriteVX(f,cNorm.indice[n],vmadSize);
						LW_WriteVX(f,cNorm.polygon,vmadSize);

						f32 = MSB4<float>(cNorm.direction[n].x);
						f.Write(reinterpret_cast<char *>(&f32), 4);
						f32 = MSB4<float>(cNorm.direction[n].y);
						f.Write(reinterpret_cast<char *>(&f32), 4);
						f32 = MSB4<float>(cNorm.direction[n].z);
						f.Write(reinterpret_cast<char *>(&f32), 4);
						vmadSize += 12;
					}
				}
				fileLen += vmadSize;

				off_T = -4-vmadSize;
				f.SeekO(off_T, wxFromCurrent);
				u32 = MSB4<uint32>(vmadSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				f.SeekO(0, wxFromEnd);
			}
		}
	}

	// --== Clips (Images) ==--
	if (Object.Images.size() > 0){
		for (uint32 x=0;x<Object.Images.size();x++){
			LWClip cImg = Object.Images[x];

			int clipSize = 0;
			f.Write("CLIP", 4);
			u32 = MSB4<uint32>(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			u32 = MSB4<uint32>(cImg.TagID);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write("STIL", 4);
			clipSize += 8;

			wxString ImgName = wxEmptyString;
			if (modelExport_LW_PreserveDir == true){
				ImgName += _T("Images/");
			}
			ImgName += wxString(cImg.Source + SLASH + cImg.Filename + wxString(_T(".tga")));
			ImgName += _T('\0');
			ImgName.Replace(_T("\\"),_T("/"));

			if (fmod((float)ImgName.length(), 2.0f) > 0)
					ImgName.Append(_T('\0'));

			u16 = MSB2((unsigned short)ImgName.length());
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(ImgName.data(), ImgName.length());
			clipSize += (2+(int)ImgName.length());

			// update the chunks length
			off_T = -4-clipSize;
			f.SeekO(off_T, wxFromCurrent);
			u32 = MSB4<uint32>(clipSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			fileLen += clipSize;
		}
	}

	// --== Surfaces ==--
	if (Object.Surfaces.size() > 0){
		for (uint32 x=0;x<Object.Surfaces.size();x++){
			LWSurface cSurf = Object.Surfaces[x];

			// Temp Values
			Vec4D Color = Vec4D(1,1,1,1);
			float reflect = 0.0f;

			LW_WriteSurface(f,cSurf.Name, Color, reflect, cSurf.isDoubleSided, cSurf.hasVertColors, (uint32)Object.PartNames.size() + x, cSurf.Comment, fileLen);
		}
	}

	// Correct File Length
	f.SeekO(4, wxFromStart);
	u32 = MSB4<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();

	// If we've gotten this far, then the file is good!
	return true;
}


// No longer writes data to a LWO file. Instead, it collects the data, and sends it to a seperate function that writes the actual file.
void ExportM2toLWO2(Attachment *att, Model *m, const char *fn, bool init){
	wxString filename(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		filename.Empty();
		filename << Path << SLASH << _T("Objects") << SLASH << Name;
	}
	if (m->modelType != MT_CHAR){
		if (modelExport_PreserveDir == true){
			wxString Path1, Path2, Name;
			Path1 << filename.BeforeLast(SLASH);
			Name << filename.AfterLast(SLASH);
			Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

			MakeDirs(Path1,Path2);

			filename.Empty();
			filename << Path1 << SLASH << Path2 << SLASH << Name;
		}
	}

	// Object Data
	LWObject Object = GatherM2forLWO(att,m,init,fn);
	if (Object.SourceType == wxEmptyString){
		wxMessageBox(_T("Error gathering information for export."),_T("Export Error"));
		wxLogMessage(_T("Failure gathering information for export."));
		return;
	}
	wxLogMessage(_T("Sending M2 Data to LWO Writing Function..."));
	WriteLWObject(filename, Object);

	// Scene Data
	// NYI
}

// Gathers and returns the Lightwave Object data of a M2 file.
LWObject GatherM2forLWO(Attachment *att, Model *m, bool init, const char *fn, bool announce){
	LWObject Object;
	if (!m)
		return Object;

	Object.SourceType = _T("M2");
	if (announce == true)
		LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),Object.SourceType);

	// Main Object
	LWLayer Layer;
	Layer.Name = wxString(m->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	Layer.ParentLayer = -1;

	// Bounding Box for the Layer
	/*if (m->bounds[0]){
		Layer.BoundingBox1 = m->bounds[0];
		Layer.BoundingBox2 = m->bounds[1];
	}*/

	//uint32 PolyCounter = 0;
	uint32 SurfCounter = 0;

	// Mesh & Slot names
	wxString meshes[19] = {_T("Hairstyles"), _T("Facial1"), _T("Facial2"), _T("Facial3"), _T("Braces"), _T("Boots"), wxEmptyString, _T("Ears"), _T("Wristbands"),  _T("Kneepads"), _T("Pants"), _T("Pants"), _T("Tarbard"), _T("Trousers"), wxEmptyString, _T("Cape"), wxEmptyString, _T("Eyeglows"), _T("Belt") };
	//wxString slots[15] = {_T("Helm"), wxEmptyString, _T("Shoulder"), wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, _T("Right Hand Item"), _T("Left Hand Item"), wxEmptyString, wxEmptyString, _T("Quiver") };

	// Build Part Names
	// Seperated from the rest of the build for various reasons.
	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)){
			// Main Model
			int g = p.geoset;
			bool isFound = false;
			wxString partName, matName;
			
			// Part Names
			int mesh = m->geosets[g].id / 100;
			if (m->modelType == MT_CHAR && mesh < 19 && meshes[mesh] != wxEmptyString){
				partName = wxString::Format(_T("Geoset %03i - %s"),g,meshes[mesh].c_str());
			}else{
				partName = wxString::Format(_T("Geoset %03i"),g);
			}
			for (uint32 x=0;x<Object.PartNames.size();x++){
				if (Object.PartNames[x] == partName){
					isFound = true;
					break;
				}
			}
			if (isFound == false)
				Object.PartNames.push_back(partName);
		}
	}

	// Process Passes
	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)){
			// Main Model
			int g = p.geoset;
			bool isFound = false;
			int partID = i;
			int surfID = i;
			uint32 *Vert2Point = new uint32[p.vertexEnd];

			wxString partName, matName;
			
			// Part Names
			int mesh = m->geosets[g].id / 100;
			if (m->modelType == MT_CHAR && mesh < 19 && meshes[mesh] != wxEmptyString){
				partName = wxString::Format(_T("Geoset %03i - %s"),g,meshes[mesh].c_str());
			}else{
				partName = wxString::Format(_T("Geoset %03i"),g);
			}
			for (uint32 x=0;x<Object.PartNames.size();x++){
				if (Object.PartNames[x] == partName){
					partID = x;
					break;
				}
			}

				// Surface Name
				matName = m->TextureList[p.tex].AfterLast(SLASH).BeforeLast(_T('.'));
				if (matName.Len() == 0)
					matName = wxString::Format(_T("Material_%03i"), p.tex);

			for (uint32 x=0;x<Object.Surfaces.size();x++){
				if (Object.Surfaces[x].Name == matName){
					isFound = true;
					surfID = x;
					break;
				}
			}

			bool doubesided = (p.cull?true:false);
			//wxLogMessage(_T("Doublesided: %s, P.Cull: %s"),(doubesided?_T("true"):_T("false")),(p.cull?_T("true"):_T("false")));

			// Add Images to Model
			LWClip ClipImage;
			// Image Filename
			wxString Texture = wxString(m->TextureList[p.tex].c_str(), wxConvUTF8);
			wxString TexturePath = Texture.BeforeLast(SLASH);
			wxString texName = Texture.BeforeLast('.');

			if (m->modelType == MT_CHAR){
				texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_") + texName.AfterLast(SLASH);
			}else if ((texName.Find(SLASH) <= 0)&&(texName == _T("Cape"))){
				texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_Replacable");
				TexturePath = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			}else if (texName.Find(SLASH) <= 0){
				texName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast(_T('.')) + _T("_") + texName;
				TexturePath = wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			}else{
				texName = texName.AfterLast(SLASH);
			}

			// Image Data
			ClipImage.Filename = texName;
			ClipImage.Source = TexturePath;
			ClipImage.TagID = (uint32)Object.PartNames.size() + SurfCounter;
			Object.Images.push_back(ClipImage);

			wxString ExportName = wxString(fn, wxConvUTF8).BeforeLast(SLASH) + SLASH + texName + _T(".tga");
			//wxLogMessage(_T("PrePath ExportName: %s, fn Path: %s"),ExportName,wxString(fn, wxConvUTF8).BeforeLast(SLASH));
			if (modelExport_LW_PreserveDir == true){
				wxString Path, Name;

				Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
				Name << ExportName.AfterLast(SLASH);

				MakeDirs(Path,_T("Images"));

				ExportName.Empty();
				ExportName << Path << SLASH<<_T("Images")<<SLASH << Name;
			}
			if (m->modelType != MT_CHAR){
				if (modelExport_PreserveDir == true){
					wxString Path1, Path2, Name;
					Path1 << ExportName.BeforeLast(SLASH);
					Name << texName.AfterLast(SLASH);
					Path2 << TexturePath;

					MakeDirs(Path1,Path2);

					ExportName.Empty();
					ExportName << Path1 << SLASH << Path2 << SLASH << Name;
				}
			}
			ExportName << _T(".tga");
			//wxLogMessage(_T("Image ExportName: %s"),ExportName);
			SaveTexture(ExportName);
			//SaveTexture2(ClipImage.Filename,ClipImage.Source,wxString(_T("LWO")),wxString(_T("tga")));

			LWSurface Surface(matName,wxString(m->TextureList[p.tex].c_str(), wxConvUTF8),ClipImage.TagID,-1,-1,doubesided);
			Object.Surfaces.push_back(Surface);

			// Points
			for (uint32 v=p.vertexStart; v<p.vertexEnd; v++) {
				// --== Point Data ==--
				LWPoint Point;
				uint32 pointnum = (uint32)Layer.Points.size();

				// Points
				Vec3D vert;
				if ((init == false)&&(m->vertices)) {
					vert = m->vertices[v];
				} else {
					vert = m->origVertices[v].pos;
				}
				Point.PointData = vert;
				Point.UVData = m->origVertices[v].texcoords;	// UV Data
				// Weight Data NYI

				Vert2Point[v] = pointnum;
				Layer.Points.push_back(Point);
			}

			// Polys
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				// --== Polygon Data ==--	
				LWPoly Poly;
				Poly.PolyData.numVerts = 3;
				for (int x=0;x<3;x++){
					// Polygon Indice
					uint32 a = p.indexStart + k + x;
					uint32 b = m->IndiceToVerts[a];
					Poly.PolyData.indice[x] = Vert2Point[b];

					// Normal Indice
					/*Poly.Normals.indice[x] = Vert2Point[b];
					Poly.Normals.direction[x].x = m->normals[b].x;
					Poly.Normals.direction[x].y = m->normals[b].z;
					Poly.Normals.direction[x].z = -m->normals[b].y;*/
				}
				Poly.PartTagID = partID;
				Poly.SurfTagID = (uint32)Object.PartNames.size() + SurfCounter;
				//Poly.Normals.polygon = (uint32)Layer.Polys.size();
				Poly.Normals.NormalMapName = wxEmptyString; //Layer.Name + wxString(_T("_NormalMap"));
				Layer.Polys.push_back(Poly);
			}
			SurfCounter++;
		}
	}

	// --== Attachments ==--



	Object.Layers.push_back(Layer);

	return Object;
}

void ExportWMOtoLWO(WMO *m, const char *fn){
	wxString filename(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		filename.Empty();
		filename << Path << SLASH << _T("Objects") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		filename.Empty();
		filename << Path1 << SLASH << Path2 << SLASH << Name;
	}

	// Object Data
	LWObject Object = GatherWMOforLWO(m,fn);
	if (Object.SourceType == wxEmptyString){
		wxMessageBox(_T("Error gathering information for export."),_T("Export Error"));
		wxLogMessage(_T("Failure gathering information for export."));
		return;
	}
	wxLogMessage(_T("Sending WMO Data to LWO Writing Function..."));
	WriteLWObject(filename, Object);

	// Scene Data

	// Export Lights & Doodads
	if ((modelExport_LW_ExportDoodads == true)||(modelExport_LW_ExportLights == true)){
		ExportWMOObjectstoLWO(m,fn);
	}
}

LWObject GatherWMOforLWO(WMO *m, const char *fn){
	wxString FileName(fn, wxConvUTF8);
	LWObject Object;

	if (!m)
		return Object;

	Object.SourceType = _T("WMO");
	LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),Object.SourceType);

	// Main Object
	LWLayer Layer;
	Layer.Name = wxString(m->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	Layer.ParentLayer = -1;

	// Bounding Box for the Layer
	Layer.BoundingBox1 = m->v1;
	Layer.BoundingBox2 = m->v2;

	uint32 SurfCounter = 0;

	// Process Groups
	for (int g=0;g<m->nGroups; g++) {
		WMOGroup *group = &m->groups[g];
		//uint32 GPolyCounter = 0;

		//wxLogMessage(_T("\nGroup %i Info:\n   Batches: %i\n   Indices: %i\n   Vertices: %i"),g,group->nBatches,group->nIndices,group->nVertices);
		Object.PartNames.push_back(wxString(group->name.c_str(), wxConvUTF8));

		Layer.HasVectorColors = group->hascv;

		// Points Batches
		for (uint32 b=0; b<group->nBatches; b++){
			WMOBatch *batch = &group->batches[b];
			//wxLogMessage(_T("\nBatch %i Info:\n   Indice-Start: %i\n   Indice-Count: %i\n   Vert-Start: %i\n   Vert-End: %i"),b,batch->indexStart,batch->indexCount,batch->vertexStart,batch->vertexEnd);
			uint32 *Vert2Point = new uint32[group->nVertices];

			uint32 t = batch->texture;
			WMOMaterial *mat = &m->mat[t];
			bool doubesided = ((mat->flags & WMO_MATERIAL_CULL)?false:true);

			// Add Images to Model
			LWClip ClipImage;
			wxString Texture = m->textures[t];
			wxLogMessage(_T("Texture: %s"),Texture.c_str());

			ClipImage.Filename = Texture.AfterLast(SLASH).BeforeLast('.');
			ClipImage.Source = Texture.BeforeLast(SLASH);
			ClipImage.TagID = m->nGroups + SurfCounter;
			Object.Images.push_back(ClipImage);

			wxString LWFilename = Texture;
			if (modelExport_LW_PreserveDir == true){
				wxString Path = FileName.BeforeLast(SLASH);
				wxString Name = Texture.AfterLast(SLASH);

				MakeDirs(Path,_T("Images"));

				LWFilename.Empty();
				LWFilename <<Path<<SLASH<<_T("Images")<<SLASH<<Name;
			}
			if (modelExport_PreserveDir == true){
				wxString Path1(LWFilename.BeforeLast(SLASH));
				wxString Path2(Texture.BeforeLast(SLASH));
				wxString Name(LWFilename.AfterLast(SLASH));
				//wxLogMessage(_T("Path1 (root): %s, Path2(Img Path): %s, Name (Img Name): %s"),Path1,Path2,Name);

				MakeDirs(Path1,Path2);

				LWFilename.Empty();
				LWFilename << Path1<<SLASH<<Path2<<SLASH<<Name;
			}
			//LWFilename = LWFilename.BeforeLast('.') + _T(".tga");
			SaveTexture2(Texture,FileName.BeforeLast(SLASH),_T("LWO"),_T("tga"));

			LWSurface Surface(ClipImage.Filename,Texture,ClipImage.TagID,-1,-1,doubesided,Layer.HasVectorColors);
			Object.Surfaces.push_back(Surface);

			// Process Verticies
			for (uint32 v=batch->vertexStart; v<=batch->vertexEnd; v++) {
				// --== Point Data ==--
				LWPoint Point;
				uint32 pointnum = (uint32)Layer.Points.size();

				// Points
				// Using straight verts causes the model to come out on it's side.
				Point.PointData.x = group->vertices[v].x;
				Point.PointData.y = group->vertices[v].z;
				Point.PointData.z = group->vertices[v].y;
				Point.UVData = group->texcoords[v];		// UVs
				// Weight Data not needed for WMOs

				// Vertex Colors
				LWVertexColor vc;
				if (group->hascv) {
					vc.r = group->VertexColors[v].r;
					vc.g = group->VertexColors[v].g;
					vc.b = group->VertexColors[v].b;
					vc.a = group->VertexColors[v].a;
				}
				Point.VertexColors = LWVertexColor(vc.r,vc.g,vc.b,vc.a);

				Vert2Point[v] = pointnum;
				//wxLogMessage(_T("Vert %i = Point %i"),v,pointnum);
				Layer.Points.push_back(Point);
			}

			// Process Indices
			for (uint32 i=0; i<batch->indexCount; i+=3) {
				// --== Polygon Data ==--	
				LWPoly Poly;
				Poly.PolyData.numVerts = 3;
				for (int x=0;x<3;x++){
					// Mod is needed, cause otherwise the polys will be generated facing the wrong way. 
					// Proper order: 0, 2, 1
					int mod = 0;
					if (x==1){
						mod = 1;
					}else if (x==2){
						mod = -1;
					}

					// Polygon Indice
					uint32 a = batch->indexStart + i + x + mod;
					uint32 b = group->IndiceToVerts[a];
					//wxLogMessage(_T("Group: %i, a: %i, b:%i, Final Indice: %i"),g,a,b,Vert2Point[b]);
					Poly.PolyData.indice[x] = Vert2Point[b];

					// Normal Indice
					Poly.Normals.indice[x] = Vert2Point[b];
					Poly.Normals.direction[x].x = group->normals[b].x;
					Poly.Normals.direction[x].y = group->normals[b].z;
					Poly.Normals.direction[x].z = -group->normals[b].y;
				}
				Poly.PartTagID = g;
				Poly.SurfTagID = m->nGroups + SurfCounter;
				Poly.Normals.polygon = (uint32)Layer.Polys.size();
				Poly.Normals.NormalMapName = Layer.Name + wxString(_T("_NormalMap"));
				Layer.Polys.push_back(Poly);
			}
			SurfCounter++;
		}
	}

	if (Layer.HasVectorColors == true){
		for (uint32 i=0;i<Object.Surfaces.size();i++){
			Object.Surfaces[i].hasVertColors = true;
		}
	}

	Object.Layers.push_back(Layer);
	wxLogMessage(_T("Completed WMO Gathering. Checking Doodads..."));
/*
	Need to get this working with layers before we enable it.
	For now, use the old method in the main function.	

	if (modelExport_LW_ExportDoodads == true){
		if (modelExport_LW_DoodadsAs == 0){
		}elseif (modelExport_LW_DoodadsAs == 1){
		}elseif (modelExport_LW_DoodadsAs == 2){
			// Doodads as seperate Layers here.
			for (uint32 ds=0;ds<m->nDoodadSets;ds++){
				wxLogMessage(_T("Processing Doodadset %i: %s"),ds,m->doodadsets[ds].name);

				for (uint32 dd=m->doodadsets[ds].start;dd<(m->doodadsets[ds].start+m->doodadsets[ds].size);dd++){
					wxLogMessage(_T("Processing Doodad %i: %s"),dd,m->modelis[dd].filename);
					WMOModelInstance *model = &m->modelis[dd];
					LWObject Doodad = GatherM2forLWO(NULL,model->model,true,wxString(fn, wxConvUTF8),false);
					//wxLogMessage(_T("Finished Gathering Doodad %i with #%i Layers."),dd,Doodad.Layers.size());

					// Move, rotate & scale Doodad
					for (uint32 i=0;i<Doodad.Layers[0].Points.size();i++){
						//Doodad.Layers[0].Points[i].PointData = (Doodad.Layers[0].Points[i].PointData * model->sc) + model->pos;
						Vec3D AgentSmith = Doodad.Layers[0].Points[i].PointData;

						Matrix Neo;
						Vec3D Trinity(model->sc,model->sc,model->sc);
						Quaternion Niobe(model->dir,model->w);
						Quaternion Morphius = Niobe;
						// Fix for WoW Coordinates.
						Morphius.x = Niobe.y;
						Morphius.y = Niobe.x;
						Morphius.z = Niobe.z;

						Neo.quaternionRotate(Morphius);
						Neo.translation(AgentSmith);
						Neo.scale(Trinity);

						Neo *= Matrix::newTranslation(model->pos);		// Position
						Neo *= Matrix::newQuatRotate(Morphius);			// Rotation
						Neo *= Matrix::newScale(Trinity);				// Scale

						Doodad.Layers[0].Points[i].PointData = Neo * AgentSmith;
					}

					wxLogMessage(_T("Adding Doodad to Layer %i..."),ds+1);
					wxString ddPrefix(_T("Doodad "));
					ddPrefix << (unsigned int)dd << _T("_");
					//wxLogMessage(_T("Doodad Prefix: %s"),ddPrefix);
					Object.Plus(Doodad,ds+1,ddPrefix);
				}
				Object.Layers[ds+1].Name = wxString(m->doodadsets[ds].name);
				Object.Layers[ds+1].ParentLayer = 0;		// Set Parent to main WMO model.
			}
	//	}
	}
*/
	return Object;
}


void ExportADTtoLWO(MapTile *m, const char *fn){
	wxString file = wxString(fn, wxConvUTF8);

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);

		MakeDirs(Path,_T("Objects"));

		file.Empty();
		file << Path << SLASH<<_T("Objects")<<SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;
		Path1 << file.BeforeLast(SLASH);
		Name << file.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

		MakeDirs(Path1,Path2);

		file.Empty();
		file << Path1 << SLASH << Path2 << SLASH << Name;
	}
	file = fixMPQPath(file);
	wxFFileOutputStream f(file, _T("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), file.c_str());
		wxMessageDialog(g_modelViewer,_T("Could not open file for exporting."),_T("Exporting Error..."));
		return;
	}
	LogExportData(_T("LWO"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("ADT"));

	wxLogMessage(_T("Starting Lightwave ADT Model Export Function..."));
	unsigned int fileLen = 0;
	int off_T;

	// ===================================================
	// FORM		// Format Declaration
	//
	// Always exempt from the length of the file!
	// ===================================================
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);

	// ===================================================
	// LWO2
	//
	// Declares this is the Lightwave Object 2 file format.
	// LWOB is the first format. It doesn't have a lot of the cool stuff LWO2 has...
	// ===================================================
	f.Write("LWO2", 4);
	fileLen += 4;

	// ===================================================
	// TAGS
	//
	// Used for various Strings. Known string types, in order:
	//		Sketch Color Names
	//		Part Names
	//		Surface Names
	// ===================================================
	/*
	f.Write("TAGS", 4);
	uint32 tagsSize = 0;
	u32 = 0;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	uint32 counter=0;
	uint32 TagCounter=0;
	uint16 PartCounter=0;
	uint16 SurfCounter=0;
	unsigned int numVerts = 0;
	unsigned int numGroups = 0;

	// Build Surface Name Database
	wxArrayString surfarray;
	wxArrayString sfix;
	std::vector<uint16> sfix2;
	std::vector<uint16> check;
	for (uint16 t=0;t<m->nTextures;t++){
		wxString tex = m->textures[t].BeforeLast(_T('.'));
		surfarray.push_back((char *)tex.c_str());
		//sfix.push_back(tex.c_str());
		sfix2.push_back(t);
		check.push_back(0);
	}
	// Rename duplicate names
	for (uint16 t=0;t<surfarray.size();t++){
		uint16 mod = 0;
		for (uint16 k=0;k<surfarray.size();k++){
			if ((t!=k)&&(surfarray[sfix2[t]] == surfarray[sfix2[k]])&&(check[k]==0)){
				//sfix[t] = wxString(surfarray[t] + wxString::Format(_T("_v%02i"),mod));
				//sfix[k] = wxString(surfarray[k] + wxString::Format(_T("_v%02i"),mod+1));
				sfix2[k] = sfix2[t];
				//sfix2[t] = t-1;
				mod++;
				check[k] = 1;
				*/
				// This code can erase the extra surface names.
				// Not used because we don't have a (sucessful) way to convert the deleted name's IDs to the other ID.
				/*for (int g=t;g>0;g--){
					if ((sfix2[g] - 1)>-1){
						sfix2[g] -= 1;
					}
				}*/
				//surfarray.erase(surfarray.begin() + k);//, surfarray.begin() + k+1);
				//wxLogMessage("Deleting duplicate.");
				//k++;
	/*
			}
		}
	}


#ifdef _DEBUG
	wxLogMessage(_T("Texture List"));
	for (uint16 x=0;x<m->nTextures;x++){
		wxLogMessage(_T("[ID:%02i] = %s"),x,m->textures[x].data());
	}

	wxLogMessage(_T("Surface List"));
	for (uint16 x=0;x<surfarray.size();x++){
		wxLogMessage(_T("[ID:%02i] = %s"),x,surfarray[x].c_str());
	}
#endif
	*/

	// ===================================================
	// LAYR
	//
	// Specifies the start of a new layer. Each layer has it's own Point & Poly
	// chunk, which tells it what data is on what layer. It's probably best
	// to only have 1 layer for now.
	// ===================================================
	f.Write("LAYR", 4);
	u32 = MSB4<uint32>(18);
	fileLen += 8;
	f.Write(reinterpret_cast<char *>(&u32), 4);
	ub = 0;
	for(int i=0; i<18; i++) {
		f.Write(reinterpret_cast<char *>(&ub), 1);
	}
	fileLen += 18;

	// ===================================================
	// PNTS Chunk
	//
	// Point data
	// ===================================================
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	uint16 point = 0;
	for (int c1=0;c1<16;c1++){
		for (int c2=0;c2<16;c2++){
			for (int num=0;num<145;num++){
				MapChunk *chunk = &m->chunks[c1][c2];
				//wxLogMessage("Vert Coords: x:%f, y:%f, z:%f",chunk->tv[num].x,chunk->tv[num].y, chunk->tv[num].z);
				Vec3D vert;
				vert.x = MSB4<float>(chunk->tv[num].x);
				vert.y = MSB4<float>(chunk->tv[num].y); // We don't switch this, because it's already correct.
				vert.z = MSB4<float>(chunk->tv[num].z);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				pointsSize += 12;

				point++;
			}
		}
	}

#ifdef _DEBUG
	wxLogMessage(_T("ADT Point Count: %i"),point);
#endif


	fileLen += pointsSize;
	off_T = -4-pointsSize;
	f.SeekO(off_T, wxFromCurrent);
	u32 = MSB4<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	// ===================================================

	f.SeekO(4, wxFromStart);
	u32 = MSB4<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);

	f.Close();

	wxLogMessage(_T("ADT Export completed."));
}
