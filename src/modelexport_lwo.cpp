#include "modelexport_lwo.h"

//---------------------------------------------
// --== Master Object Writing Function ==--
//---------------------------------------------

// Write Lightwave Object data to a file.
bool WriteLWObject(wxString filename, LWObject Object) {

	/* LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.

	LWO Model Format, as layed out in official LWO2 files.( I Hex Edited to find most of this information from files I made/saved in Lightwave. -Kjasi)

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
		wxMessageBox(wxT("No Layer Data found.\nUnable to write object file."),wxT("Error"));
		wxLogMessage(wxT("Error: No Layer Data. Unable to write object file."));
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
	size_t TagCount = Object.PartNames.size() + Object.Surfaces.size();

	// Open Model File
	wxString file = wxString(filename, wxConvUTF8);
	wxFFileOutputStream f(file, wxT("w+b"));

	if (!f.IsOk()) {
		wxMessageBox(wxT("Unable to open file. Could not export model."),wxT("Error"));
		wxLogMessage(wxT("Error: Unable to open file '%s'. Could not export model."), file.c_str());
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
	// LWO2 is the modern format, LWOB is the first format. LWOB doesn't have a lot of the capabilities of LWO2, which we need for exporting.
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
		for (size_t i=0; i<Object.PartNames.size(); i++){
			wxString PartName = Object.PartNames[i];

			PartName.Append(wxT('\0'));
			if (fmod((float)PartName.Len(), 2.0f) > 0)
				PartName.Append(wxT('\0'));
			f.Write(PartName.data(), PartName.Len());
			tagsSize += (uint32)PartName.Len();
		}
		// Surfaces
		for (size_t i=0; i<Object.Surfaces.size(); i++){
			wxString SurfName = Object.Surfaces[i].Name;

			SurfName.Append(wxT('\0'));
			if (fmod((float)SurfName.Len(), 2.0f) > 0)
				SurfName.Append(wxT('\0'));
			f.Write(SurfName.data(), SurfName.Len());
			tagsSize += (uint32)SurfName.Len();
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
	for (size_t l=0;l<Object.Layers.size();l++){
		LWLayer cLyr = Object.Layers[l];
		// Define a Layer & It's data
		if (cLyr.Name.length() > 0)
			cLyr.Name.Append(wxT('\0'));
		if (fmod((float)cLyr.Name.length(), 2.0f) > 0)
			cLyr.Name.Append(wxT('\0'));
		uint16 LayerNameSize = (uint16)cLyr.Name.length();
		uint32 LayerSize = 16+LayerNameSize;
		if ((cLyr.ParentLayer)&&(cLyr.ParentLayer>-1))
			LayerSize += 2;
		f.Write("LAYR", 4);
		u32 = MSB4<uint32>(LayerSize);
		f.Write(reinterpret_cast<char *>(&u32), 4);
		fileLen += 8;

		// Layer Number
		u16 = MSB2((uint16)l);
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
		f.Write(wxT("Texture"), 7);
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
		// Vertex Colors Map
		// ===================================================
		if (cLyr.HasVectorColors == true){
			wxLogMessage(wxT("Has Vector Colors"));

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
				//wxLogMessage(wxT("Point: %i - R:%f(%i), G:%f(%i), B:%f(%i), A:%f(%i)"),i,rf,cLyr.Points[i].VertexColors.r,gf,cLyr.Points[i].VertexColors.g,bf,cLyr.Points[i].VertexColors.b,af,cLyr.Points[i].VertexColors.a);

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

		// ===================================================
		// Weight Maps
		// ===================================================
		if (cLyr.Weights.size() > 0){
			wxLogMessage(wxT("Has Weight Maps"));
			for (size_t w=0;w<cLyr.Weights.size();w++){
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
				f.Write("WGHT", 4);
				u16 = MSB2(1);		// Always 1
				f.Write(reinterpret_cast<char *>(&u16), 2);
				vmapSize += 6;

				// Weightmap Name, /0 Terminated, evened out.
				wxString name = cLyr.Weights[w].WeightMapName;
				name.Append(wxT('\0'));
				if (fmod((float)name.Len(), 2.0f) > 0)
					name.Append(wxT('\0'));
				f.Write(name, name.Len());
				vmapSize += (uint32)name.Len();

				for (size_t p=0;p<cLyr.Weights[w].PData.size();p++){
					// Point number (VX Format)
					size_t pID = cLyr.Weights[w].PData[p].PointID;
					LW_WriteVX(f,pID,vmapSize);

					// Float value of weight map
					float value = MSB4<float>(cLyr.Weights[w].PData[p].Value);
					f.Write(reinterpret_cast<char *>(&value), 4);
					vmapSize += 4;
				}

				fileLen += vmapSize;
				off_T = -4-vmapSize;
				f.SeekO(off_T, wxFromCurrent);
				u32 = MSB4<uint32>(vmapSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				f.SeekO(0, wxFromEnd);
			}
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
				PolyChunk PolyData = cLyr.Polys[x].PolyData;
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
				f.Write(wxT("PTAG"), 4);
				uint32 ptagSize = 4;
				u32 = MSB4<uint32>(ptagSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				fileLen += 8;
				f.Write(wxT("PART"), 4);

				for (unsigned int x=0;x<cLyr.Polys.size();x++){
					LWPoly Poly = cLyr.Polys[x];
					LW_WriteVX(f,x,ptagSize);

					u16 = MSB2((uint16)Poly.PartTagID);
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
				f.Write(wxT("PTAG"), 4);
				uint32 ptagSize = 4;
				u32 = MSB4<uint32>(ptagSize);
				f.Write(reinterpret_cast<char *>(&u32), 4);
				fileLen += 8;
				f.Write(wxT("SURF"), 4);

				for (unsigned int x=0;x<cLyr.Polys.size();x++){
					LWPoly Poly = cLyr.Polys[x];
					LW_WriteVX(f,x,ptagSize);

					u16 = MSB2((uint16)Poly.SurfTagID);
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
				NormMapName += wxT('\0');
				if (fmod((float)NormMapName.length(), 2.0f) > 0)
					NormMapName.Append(wxT('\0'));
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

			u32 = MSB4<uint32>((uint32)cImg.TagID);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.Write("STIL", 4);
			clipSize += 8;

			wxString ImgName = wxEmptyString;
			if (modelExport_LW_PreserveDir == true){
				ImgName += wxT("Images/");
			}
			ImgName += wxString(cImg.Source + SLASH + cImg.Filename + wxString(wxT(".tga")));
			ImgName += wxT('\0');
			ImgName.Replace(wxT("\\"),wxT("/"));

			if (fmod((float)ImgName.length(), 2.0f) > 0)
					ImgName.Append(wxT('\0'));

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

			//LW_WriteSurface(f, cSurf, fileLen);
			#ifdef _DEBUG
				//wxLogMessage(wxT("LW Write Surface Vars:\nSurfName: %s\nColor: %f/%f/%f\nReflect Value: %f\nCulling: %s\nSurfaceID: %i\nComment: \"%s\""),Surface.Name.c_str(),Surface.Surf_Color.x,Surface.Surf_Color.y,Surface.Surf_Color.z,Surface.Surf_Reflect,(Surface.isDoubleSided?wxT("True"):wxT("False")),Surface.Image_Color,Surface.Comment);
			#endif
			int off_T;

			uint32 surfaceDefSize = 0;
			f.Write(wxT("SURF"), 4);
			u32 = MSB4<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			fileLen += 8;

			wxString surfName = cSurf.Name;
			surfName.Append(wxT('\0'));
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append(wxT('\0'));

			surfName.Append(wxT('\0')); // ""
			surfName.Append(wxT('\0')); // Evens out the Code.
			f.Write(surfName.data(), (int)surfName.length());

			surfaceDefSize += (uint32)surfName.length();

			// Surface Attributes
			// COLOUR, size 4, bytes 2
			f.Write(wxT("COLR"), 4);
			u16 = 14; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			// value
			f32 = MSB4<float>(cSurf.Surf_Color.x);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f32 = MSB4<float>(cSurf.Surf_Color.y);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			f32 = MSB4<float>(cSurf.Surf_Color.z);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 20;

			// LUMI
			f.Write(wxT("LUMI"), 4);
			u16 = 6; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = cSurf.Surf_Lum;
			f32 = MSB4<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// DIFF
			f.Write(wxT("DIFF"), 4);
			u16 = 6; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = cSurf.Surf_Diff;
			f32 = MSB4<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// SPEC
			f.Write(wxT("SPEC"), 4);
			u16 = 6; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = cSurf.Surf_Spec;
			f32 = MSB4<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// REFL
			f.Write(wxT("REFL"), 4);
			u16 = 6; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = cSurf.Surf_Reflect;
			f32 = MSB4<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// TRAN
			f.Write(wxT("TRAN"), 4);
			u16 = 6; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = MSB4<float>(cSurf.Surf_Trans);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 12;

			// GLOSSINESS
			f.Write(wxT("GLOS"), 4);
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

			if (cSurf.hasVertColors == true){
				// VCOL (Vector Colors)
				f.Write(wxT("VCOL"), 4);
				u16 = MSB2(18); // size
				uint16 zero = 0;
				f.Write(reinterpret_cast<char *>(&u16), 2);
				// Unknown Values
				f32 = MSB4<float>(1.0f);
				f.Write(reinterpret_cast<char *>(&f32), 4);
				f.Write(reinterpret_cast<char *>(&zero), 2);
				// RGBA Map Name
				f.Write(wxT("RGBA"), 4);
				f.Write(wxT("Colors"), 6);
				f.Write(reinterpret_cast<char *>(&zero), 2);
				surfaceDefSize += 24;
			}

			// SMAN (Smoothing)
			f.Write(wxT("SMAN"), 4);
			u16 = 4; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			// Smoothing is done in radiens. PI = 180 degree smoothing.
			f32 = (float)PI;
			f32 = MSB4<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			surfaceDefSize += 10;

			// RFOP
			f.Write(wxT("RFOP"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// TROP
			f.Write(wxT("TROP"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// SIDE
			f.Write(wxT("SIDE"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			if (cSurf.isDoubleSided == false){
				u16 = 3;
			}
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			surfaceDefSize += 8;

			// --
			// BLOK
			uint16 blokSize = 0;
			f.Write(wxT("BLOK"), 4);
			f.Write(reinterpret_cast<char *>(&blokSize), 2);
			surfaceDefSize += 6;

			// IMAP
			f.Write(wxT("IMAP"), 4);
			u16 = 50-8; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0x80;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CHAN
			f.Write(wxT("CHAN"), 4);
			u16 = 4; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(wxT("COLR"), 4);
			blokSize += 10;

			// OPAC
			f.Write(wxT("OPAC"), 4);
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
			f.Write(wxT("ENAB"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 1;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// NEGA
			f.Write(wxT("NEGA"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;
			
			// AXIS
			// This is only needed for Planar images. Everything but ADTs uses UV data.
			if (cSurf.Image_Color.Axis == LW_TEXTUREAXIS_PLANAR){
				f.Write(wxT("AXIS"), 4);
				u16 = 2; // size
				u16 = MSB2(u16);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				u16 = 1;
				u16 = MSB2(u16);
				f.Write(reinterpret_cast<char *>(&u16), 2);
				blokSize += 8;
			}
			// TMAP
			f.Write(wxT("TMAP"), 4);
			u16 = 98; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 6;

			// CNTR
			f.Write(wxT("CNTR"), 4);
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
			f.Write(wxT("SIZE"), 4);
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
			f.Write(wxT("ROTA"), 4);
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
			f.Write(wxT("FALL"), 4);
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
			f.Write(wxT("OREF"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// CSYS
			f.Write(wxT("CSYS"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 0;
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// end TMAP

			// PROJ
			f.Write(wxT("PROJ"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 5;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// AXIS
			f.Write(wxT("AXIS"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = 2;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// IMAG
			f.Write(wxT("IMAG"), 4);
			u16 = 2; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			u16 = cSurf.Image_Color.ID;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			blokSize += 8;

			// WRAP
			f.Write(wxT("WRAP"), 4);
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
			f.Write(wxT("WRPW"), 4);
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
			f.Write(wxT("WRPH"), 4);
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
			f.Write(wxT("VMAP"), 4);
			u16 = 8; // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			wxString t = wxT("Texture");
			t.Append(wxT('\0'));
			f.Write(t.data(), t.length());
			blokSize += 14;

			// AAST
			f.Write(wxT("AAST"), 4);
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
			f.Write(wxT("PIXB"), 4);
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
			f.Write(wxT("CMNT"), 4);
			wxString comment = cSurf.Comment;
			comment.Append(wxT('\0'));
			if (fmod((float)comment.length(), 2.0f) > 0)
				comment.Append(wxT('\0'));
			u16 = (uint16)comment.length(); // size
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(comment.data(), comment.length());
			surfaceDefSize += 6 + (uint32)comment.length();

			f.Write(wxT("VERS"), 4);
			u16 = 4;
			u16 = MSB2(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f32 = 950;	// Surface Compatability Number. 950 = Lightwave 9.5
			f32 = MSB4<int32>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);
			surfaceDefSize += 10;
					
			// Fix Surface Size
			fileLen += surfaceDefSize;
			off_T = -4-surfaceDefSize;
			f.SeekO(off_T, wxFromCurrent);
			u32 = MSB4<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);
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


//---------------------------------------------
// Scene Writing Helper Functions
//---------------------------------------------

// Writes an Object's Bone to the scene file.
void WriteLWSceneBone(ofstream &fs, LWBones BoneData)
{
	int active = 1;
	if ((BoneData.Active == false)||(BoneData.WeightMap_Name != BoneData.Name)||(BoneData.WeightMap_Name == wxEmptyString)){	// Add Or if weightmap != bone name when we can get weightmap info.
		active = 0;
	}

	fs << wxT("AddBone 4") << wxString::Format(wxT("%03x"),BoneData.BoneID) << wxT("0000\nBoneName " << BoneData.Name << "\n");
	fs << wxT("ShowBone 1 -1 0.376471 0.878431 0.941176\nBoneActive " << active << "\n");
	fs << wxT("BoneStrength 1\n");
	if (BoneData.WeightMap_Name != wxEmptyString){
		fs << wxT("BoneWeightMapName " << BoneData.WeightMap_Name << "\nBoneWeightMapOnly " << (BoneData.WeightMap_Only?1:0) << "\nBoneNormalization " << (BoneData.WeightMap_Normalize?1:0) << "\n");
	}
	fs << wxT("ScaleBoneStrength 1" << "\n");
	fs << wxT("BoneRestPosition "<<BoneData.RestPos.x<<" "<<BoneData.RestPos.y<<" "<<BoneData.RestPos.z<< "\n");
	fs << wxT("BoneRestDirection "<<BoneData.RestRot.x<<" "<<BoneData.RestRot.y<<" "<<BoneData.RestRot.z<< "\n");
	fs << wxT("BoneRestLength "<<BoneData.Length<< "\n");
	fs << wxT("BoneType " << BoneData.BoneType << "\n");
	fs << wxT("BoneMotion\nNumChannels 9\n");
	
	LW_WriteMotionArray(fs,BoneData.AnimData);

	fs << wxT("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
	fs << wxT("ParentItem " << BoneData.ParentType);
	if (BoneData.ParentType == LW_ITEMTYPE_BONE){
		fs << wxString::Format(wxT("%03x"),BoneData.ParentID) << wxT("0000");
	}else{	// Assume LW_ITEMTYPE_OBJECT
		fs << wxString::Format(wxT("%07x"),BoneData.ParentID);
	}
	fs << wxT("\nIKInitialState 0\n");
}

// Writes an Object or Null, including Bones, to the scene file.
void WriteLWSceneObject(ofstream &fs, LWSceneObj Object)
{
	if (Object.isNull == true){
		fs << wxT("AddNullObject");
	}else{
		fs << wxT("LoadObjectLayer ") << Object.LayerID;
	}
	fs << wxT(" 1" << wxString::Format(wxT("%07x"),Object.ObjectID) << " " << Object.Name << "\nChangeObject 0\n");

	if (Object.ObjectTags != wxEmptyString)
		fs << wxT("// " << Object.ObjectTags << "\n");
	fs << wxT("ShowObject 7 -1 0.376471 0.878431 0.941176 \nGroup 0\nObjectMotion\nNumChannels 9\n");

	LW_WriteMotionArray(fs,Object.AnimData);

	fs << wxT("PathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");

	if (Object.ParentType > LW_ITEMTYPE_NOPARENT){
		fs << wxT("ParentItem " << Object.ParentType);
		if (Object.ParentType == LW_ITEMTYPE_BONE){
			fs << wxString::Format(wxT("%03x"),Object.ParentID) << wxT("0000");
		}else{
			fs << wxString::Format(wxT("%07x"),Object.ParentID) << wxT("\n");
		}
		fs << wxT("\n");
	}
	fs << wxT("IKInitialState 0\nSubPatchLevel 3 3\nShadowOptions 7\n\n");

	// Bones
	if (modelExport_LW_ExportBones == true){
		if (Object.Bones.size() > 0){
			fs << wxT("BoneFalloffType 5\nFasterBones 0\n");
			for (size_t x=0;x<Object.Bones.size();x++){
				WriteLWSceneBone(fs, Object.Bones[x]);
			}
		}
	}
}

// Write a Light to the Scene File
void WriteLWSceneLight(ofstream &fs, LWLight Light) //uint32 &lcount, Vec3D LPos, uint32 Ltype, Vec3D Lcolor, float Lintensity, bool useAtten, float AttenEnd, float defRange = 2.5, wxString prefix = wxEmptyString, uint32 ParentNum = -1)
{
	fs << wxT("AddLight 2" << wxString::Format(wxT("%07x"),Light.LightID) << "\n");
	//modelname[0] = toupper(modelname[0]);
	fs << wxT("LightName " << Light.Name << "\nShowLight 1 -1 0.941176 0.376471 0.941176\nLightMotion\nNumChannels 9\n");
	LW_WriteMotionArray(fs,Light.AnimData);

	if (Light.ParentType > LW_ITEMTYPE_NOPARENT){
		fs << wxT("ParentItem " << Light.ParentType);
		if (Light.ParentType == LW_ITEMTYPE_BONE){
			fs << wxString::Format(wxT("%03x"),Light.ParentID) << wxT("0000");
		}else{
			fs << wxString::Format(wxT("%07x"),Light.ParentID) << wxT("\n");
		}
		fs << wxT("\n");
	}

	// Light Color Reducer
	// Some lights have a color channel greater than 255. This reduces all the colors, but increases the intensity, which should keep it looking the way Blizzard intended.
	Vec3D LColor = Light.Color;
	float LIntensity = Light.Intensity;
	while ((LColor.x > 1.0f)||(LColor.y > 1.0f)||(LColor.z > 1.0f)) {
		LColor.x *= 0.99f;
		LColor.y *= 0.99f;
		LColor.z *= 0.99f;
		LIntensity /= 0.99f;
	}
	fs << wxT("LightColor " << LColor.x << " " << LColor.y << " " << LColor.z << "\nLightIntensity " << LIntensity << "\n");

	// Process Light type & output!
	switch (Light.LightType) {
		// Omni Lights
		case 1:
		default:
			// Default to an Omni (Point) light.
			fs << wxT("LightType " << LW_LIGHTTYPE_POINT << "\n");

			if (Light.FalloffRadius > 0.0f) {
				// Use Inverse Distance for the default Light Falloff Type. Should better simulate WoW Lights, until I can write a WoW light plugin for Lightwave...
				fs << wxT("LightFalloffType 2\nLightRange " << Light.FalloffRadius << "\n");
			}else{
				// Default to these settings, which look pretty close...
				fs << wxT("LightFalloffType 2\nLightRange 2.5\n");
			}
			fs << wxT("ShadowType 1\nShadowColor 0 0 0\n");
			WriteLWScenePlugin(fs,wxT("LightHandler"),1,wxT("PointLight"));
	}
	fs << wxT("\n");
}


//---------------------------------------------
// --== Scene Writing Function ==--
//---------------------------------------------

// Write Lightwave Scene data to a file.
// Currently incomplete.
bool WriteLWScene(LWScene *SceneData){
	wxLogMessage(wxT("Export Lightwave Scene Function running."));

	if ((SceneData->Objects.size() == 0) && (SceneData->Lights.size() == 0) && (SceneData->Cameras.size() == 0)){
		wxLogMessage(wxT("No Scene Data found. Unable to write scene file."));
		return false;
	}
	if ((modelExport_LW_ExportLights == false) && (modelExport_LW_ExportDoodads == false) && (modelExport_LW_ExportCameras == false) && (modelExport_LW_ExportBones == false)){
		wxString errormsg = wxT("Global variables don't allow for scene generation. Unable to write scene file.");
		wxLogMessage(errormsg);
		wxMessageBox(errormsg,wxT("Scene Export Failure"));
		return false;
	}

	wxString SceneName = SceneData->FilePath + SceneData->FileName;

	ofstream fs(SceneName.fn_str(), ios_base::out | ios_base::trunc);
	if (!fs.is_open()) {
		wxMessageBox(wxT("Unable to open the scene file for exporting."),wxT("Scene Export Failure"));
		wxLogMessage(wxT("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return false;
	}
	wxLogMessage(wxT("Opened %s for writing..."),SceneName.c_str());
	SceneName = SceneName.AfterLast(SLASH);

	/*
	Lightwave Scene Data Order:
	
	Format Data
	Render Range Data
	Objects
		Bones
	Global Light Data
	Lights
	Cameras
	GlobalSettings
	Antialasing Settings
	Backdrop
	Fog Data
	Render Settings
	View Configuration
	Options
	Current Items

	*/

	// File Top
	fs << wxT("LWSC\n");
	fs << wxT("5\n\n"); // This is a version-compatibility number...
	
	// Scene Length Data
	size_t Frame_First = SceneData->FirstFrame;
	size_t Frame_Last = SceneData->LastFrame;
	if ((Frame_First == 0)&&(Frame_Last == 1)){
		Frame_First = 0;
		Frame_Last = 60;
	}
	fs << wxT("RenderRangeType 0\nFirstFrame " << Frame_First << "\nLastFrame " << Frame_Last << "\nFrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary " << Frame_First << "-" << Frame_Last << "\n");
	fs << wxT("PreviewFirstFrame " << Frame_First << "\nPreviewLastFrame " << Frame_Last << "\nPreviewFrameStep 1\nCurrentFrame " << Frame_First << "\nFramesPerSecond 30\nChangeScene 0\n");

	// Objects & Bones
	for (size_t o=0;o<SceneData->Objects.size();o++){
		WriteLWSceneObject(fs,SceneData->Objects[o]);
	}

	// Global Light Options
	if (!SceneData->AmbientIntensity){
		SceneData->AmbientIntensity = 0.5f;		// Default to 50%
	}
	fs << wxT("AmbientColor 1 1 1\nAmbientIntensity ") << SceneData->AmbientIntensity << wxT("\nDoubleSidedAreaLights 1\nRadiosityType 2\nRadiosityInterpolated 1\nRadiosityTransparency 0\nRadiosityIntensity 1\nRadiosityTolerance 0.2928932\nRadiosityRays 64\nSecondaryBounceRays 16\nRadiosityMinPixelSpacing 4\nRadiosityMaxPixelSpacing 100\nRadiosityMultiplier 1\nRadiosityDirectionalRays 0\nRadiosityUseGradients 0\nRadiosityUseBehindTest 1\nBlurRadiosity 1\nRadiosityFlags 0\nRadiosityCacheModulus 1\nRadiosityCacheFilePath radiosity.cache\nPixelFilterForceMT 0\n\n");

	if (modelExport_LW_ExportLights == true){
		for (size_t l=0;l<SceneData->Lights.size();l++){
			WriteLWSceneLight(fs,SceneData->Lights[l]);
		}
	}

	if (modelExport_LW_ExportCameras == true){
		for (size_t c=0;c<SceneData->Cameras.size();c++){
			
		}
	}

	// Successfully wrote the scene file!
	return true;
}


//---------------------------------------------
// --== Scene Gathering Functions ==--
//---------------------------------------------

// Exports a Model to a Lightwave Scene File.
void ExportM2toScene(Attachment *att, Model *m, const char *fn, bool init){
	/*
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
		wxLogMessage(wxT("M2 Scene Export: Did not find any reason to export a scene. Stopping Scene export."));
		return;
	}
	wxLogMessage(wxT("M2 Scene Export Values:\n  nLights: %i\n  nBones: %i\n  hasCamera: %s\n  Use WMV Pos/Rot: %s\n  doreturn: %s"),m->header.nLights,m->header.nBones,(m->hasCamera?wxT("true"):wxT("false")),(modelExport_UseWMVPosRot?wxT("true"):wxT("false")),(doreturn?wxT("true"):wxT("false")));

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast(wxT('.'));
	SceneName << wxT(".lws");

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);

		MakeDirs(Path,wxT("Scenes"));

		SceneName.Empty();
		SceneName << Path << SLASH << wxT("Scenes") << SLASH << Name;
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
		wxMessageBox(wxT("Unable to open the scene file for exporting."),wxT("Scene Export Failure"));
		wxLogMessage(wxT("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	wxLogMessage(wxT("Opened %s for writing..."),SceneName.c_str());
	SceneName = SceneName.AfterLast(SLASH);

	// File Top
	fs << wxT("LWSC\n");
	fs << wxT("5\n\n"); // I think this is a version-compatibility number...

	uint32 RangeEnd = 0;

	uint32 mcount = 0; // Model Count
	uint32 lcount = 0; // Light Count
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
		objFilename << wxT("Objects") << SLASH;
	}
	if (modelExport_PreserveDir == true){
		objFilename += wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		objFilename << SLASH;
		objFilename.Replace(wxT("\\"),wxT("/"));
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
	ObjData.Push(ObjPos,(ObjRot/(float)RADIAN),OneScale);

	//WriteLWSceneObject(fs,objFilename,ObjData,mcount);

	// Export Bones
	if (modelExport_LW_ExportBones == true){
		if (m->header.nBones > 0){
			fs << wxT("BoneFalloffType 5\nFasterBones 1\n");
			for (size_t x=0;x<m->header.nBones;x++){
				LWBones lwBone((uint32)x);
				Bone *cbone = &m->bones[x];
				AnimationData a;

				Vec3D Pos = cbone->pivot;
				if (cbone->parent > -1){
					Pos -= m->bones[cbone->parent].pivot;
					lwBone.ParentType = LW_ITEMTYPE_BONE;
					lwBone.ParentID = cbone->parent;
				}else{
					lwBone.Active = false;		// Disable Non-Parented bones.
					lwBone.ParentType = LW_ITEMTYPE_OBJECT;
					lwBone.ParentID = ModelID;
				}
				if (init == false){
					Pos = cbone->transPivot;
					if (cbone->parent > -1){
						Pos -= m->bones[cbone->parent].transPivot;
						lwBone.Active = true;
					}
				}
				Pos.z = -Pos.z;
				MakeModelFaceForwards(Pos,false);
				wxString bone_name = wxString::Format(wxT("Bone_%03i"), x);
				for (int j=0; j<BONE_MAX; ++j) {
					if (m->keyBoneLookup[j] == (uint16)x) {
						bone_name = Bone_Names[j];
						break;
					}
				}

				a.Push(Pos,Vec3D(),Vec3D(1,1,1));
				lwBone.RestPos = Pos;
				lwBone.AnimData = a;
				lwBone.Name = bone_name;

				WriteLWSceneBone(fs, lwBone);
			}
		}
	}

	// Lighting Basics
	// Defaulting to WoW-Like settings
	fs << wxT("AmbientColor 1 1 1\nAmbientIntensity 0.50\nDoubleSidedAreaLights 1\n\n");

	// Lights
	if (modelExport_LW_ExportLights == true){
		uint32 nLights = m->header.nLights;
		for (size_t x=0;x<nLights;x++){
			ModelLight *l = &m->lights[x];

			Vec3D color = l->diffColor.getValue(0,0);
			float intense = l->diffIntensity.getValue(0,0);
			bool useAtten = false;
			float AttenEnd = l->AttenEnd.getValue(0,0);

			if (l->UseAttenuation.getValue(0,0) > 0){
				useAtten = true;
			}

//			WriteLWSceneLight(fs,lcount,l->pos,l->type,color,intense,useAtten,AttenEnd,2.5);
		}
		if (att != NULL){
			if (att->model){
				wxLogMessage(wxT("Att Model found."));
			}
			for (uint32 c=0; c<att->children.size(); c++) {
				Attachment *att2 = att->children[c];
				for (uint32 j=0; j<att2->children.size(); j++) {
					Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

					if (mAttChild){
						for (size_t x=0;x<mAttChild->header.nLights;x++){
							ModelLight *l = &mAttChild->lights[x];

							Vec3D color = l->diffColor.getValue(0,0);
							float intense = l->diffIntensity.getValue(0,0);
							bool useAtten = false;
							float AttenEnd = l->AttenEnd.getValue(0,0);

							if (l->UseAttenuation.getValue(0,0) > 0){
								useAtten = true;
							}

//							WriteLWSceneLight(fs,lcount,l->pos,l->type,color,intense,useAtten,AttenEnd,2.5);
						}
					}
				}
			}
		}
	}

	// Export Cameras
	if (modelExport_LW_ExportCameras == true){
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
					//CamData.Push(a,ZeroRot,OneScale,ctime/30);
					if (ctime > RangeEnd)
						RangeEnd = ctime;
				}
			}else{
				CamData.Push(cam->target,ZeroRot,OneScale);
			}

			//WriteLWSceneObject(fs,wxT("Camera Target"),CamData,mcount,0,true,ModelID);

			fs << wxT("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nCameraMotion\nNumChannels 6\n");

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

			fs << wxT("ParentItem 1" << wxString::Format(wxT("%07x"),ModelID) << "\n");
			fs << wxT("IKInitCustomFrame 0\nGoalStrength 1\nIKFKBlending 0\nIKSoftMin 0.25\nIKSoftMax 0.75\nCtrlPosItemBlend 1\nCtrlRotItemBlend 1\nCtrlScaleItemBlend 1\n\n");
			fs << wxT("HController 1\nPController 1\nPathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
			fs << wxT("TargetItem 1"<<wxString::Format(wxT("%07x"),CameraTargetID)<<"\n");
			fs << wxT("ZoomFactor "<<(cam->fov*3.6)<<"\nZoomType 2\n");
			WriteLWScenePlugin(fs,wxT("CameraHandler"),1,wxT("Perspective"));	// Make the camera a Perspective camera
		}
	}

	// Scene File Basics
	fs << wxT("RenderRangeType 0\nFirstFrame 1\nLastFrame "<<RangeEnd<<"\n");
	fs << wxT("FrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary 1-"<<RangeEnd<<"\n");
	fs << wxT("PreviewFirstFrame 0\nPreviewLastFrame "<<RangeEnd<<"\nPreviewFrameStep 1\nCurrentFrame 0\nFramesPerSecond 30\nChangeScene 0\n\n");

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << wxT("RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n");

	fs.close();
	*/
}

// Export a WMO's Objects & Lights to a Scene file.
void ExportWMOtoScene(WMO *m, const char *fn){
	// Should we generate a scene file?
	// Wll only generate if there are doodads or lights.
	/*
	bool doreturn = false;

	if ((m->nDoodads == 0) && (m->nLights == 0)){
		wxLogMessage(wxT("No Scene Data found. Unable to write scene file."));
		doreturn = true;
	}
	if ((modelExport_LW_ExportLights == false) && (modelExport_LW_ExportDoodads == false)){
		wxLogMessage(wxT("Global variables don't allow for scene generation. Unable to write scene file."));
		doreturn = true;
	}
	if (doreturn == true)
		return;

	// Open file
	wxString SceneName = wxString(fn, wxConvUTF8).BeforeLast(wxT('.'));
	SceneName << wxT(".lws");

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << SceneName.BeforeLast(SLASH);
		Name << SceneName.AfterLast(SLASH);

		MakeDirs(Path,wxT("Scenes"));

		SceneName.Empty();
		SceneName << Path << SLASH << wxT("Scenes") << SLASH << Name;
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
		wxMessageBox(wxT("Unable to open the scene file for exporting."),wxT("Scene Export Failure"));
		wxLogMessage(wxT("Error: Unable to open file \"%s\". Could not export the scene."), SceneName.c_str());
		return;
	}
	SceneName = SceneName.AfterLast(SLASH);
*/
	/*
		Lightwave Scene files are simple text files. New Lines (\n) are need to add a new variable for the scene to understand.
		Lightwave files are pretty sturdy. Variables not already in a scene or model file, will usually be generated when opened.
		As such, we don't need to declare EVERY variable for the scene file, but I'm gonna add what I think is pertinent.
	*/
/*
	// File Top
	fs << wxT("LWSC\n");
	fs << wxT("5\n\n"); // I think this is a version-compatibility number...

	// Scene File Basics
	fs << wxT("RenderRangeType 0\nFirstFrame 1\nLastFrame 60\n");
	fs << wxT("FrameStep 1\nRenderRangeObject 0\nRenderRangeArbitrary 1-60\n");
	fs << wxT("PreviewFirstFrame 0\nPreviewLastFrame 60\nPreviewFrameStep 1\nCurrentFrame 0\nFramesPerSecond 30\nChangeScene 0\n\n");

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
		objFilename << wxT("Objects") << SLASH;
	}
	if (modelExport_PreserveDir == true){
		objFilename += wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		objFilename << SLASH;
		objFilename.Replace(wxT("\\"),wxT("/"));
	}
	objFilename += Obj;

	AnimationData ObjData;
//	ObjData.Push(ZeroPos,ZeroRot,OneScale);

	//WriteLWSceneObject(fs,objFilename,ObjData,mcount);

	if ((modelExport_LW_ExportDoodads ==  true)&&(modelExport_LW_DoodadsAs > 1)){
		// Doodads
		for (uint32 ds=0;ds<m->nDoodadSets;ds++){			
			m->showDoodadSet(ds);
			WMODoodadSet *DDSet = &m->doodadsets[ds];
			wxString DDSetName;
			DDSetName << wxString(m->name.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(wxT('.')) << wxString(wxT(" DoodadSet ")) << wxString(DDSet->name, wxConvUTF8);
			int DDSID = mcount;

			AnimationData DDData;
			DDData.Push(ZeroPos,(ZeroRot+(float)(PI/2)),OneScale,0);
			//WriteLWSceneObject(fs,DDSetName,DDData,mcount,7,true,ModelID);

			for (uint32 dd=DDSet->start;dd<(DDSet->start+DDSet->size);dd++){
				WMOModelInstance *doodad = &m->modelis[dd];
				wxString name = wxString(doodad->filename.c_str(), wxConvUTF8).AfterLast(SLASH).BeforeLast(wxT('.'));
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
						pathdir << wxT("Objects") << SLASH;
					}
					name = pathdir << wxString(doodad->filename.c_str(), wxConvUTF8).BeforeLast(wxT('.')) << wxT(".lwo");
					name.Replace(wxT("\\"),wxT("/"));
				}

				AnimationData DoodadData;
				DoodadData.Push(Pos,Rot,Vec3D(doodad->sc,doodad->sc,doodad->sc),0);

				//WriteLWSceneObject(fs,name,DoodadData,mcount,7,isNull,DDSID,wxString(doodad->filename.c_str(), wxConvUTF8));
				wxLogMessage(wxT("Export: Finished writing the Doodad to the Scene File."));

				// Doodad Lights
				// Apparently, Doodad Lights must be parented to the Doodad for proper placement.
				if ((doodad->model) && (doodad->model->header.nLights > 0)){
					wxLogMessage(wxT("Export: Doodad Lights found for %s, Number of lights: %i"), doodad->filename.c_str(), doodad->model->header.nLights);
					DoodadLightArrayDDID[DDLArrCount] = DDID;
					DoodadLightArrayID[DDLArrCount] = dd;
					DDLArrCount++;
				}
			}
		}
	}

	// Lighting Basics
	fs << wxT("AmbientColor 1 1 1\nAmbientIntensity 0.25\nDoubleSidedAreaLights 1\n\n");

	// Lights
	if (modelExport_LW_ExportLights == true){
		// WMO Lights
		for (unsigned int i=0;i<m->nLights;i++){
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

//			WriteLWSceneLight(fs,lcount,light->pos,light->type,color,intense,useAtten,AttenEnd,2.5);
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
			
//			WriteLWSceneLight(fs,lcount,light->pos,light->type,color,intense,useAtten,AttenEnd,5,name,DoodadLightArrayDDID[i]);
		}
	}

	// Camera data (if we want it) goes here.
	// Yes, we can export flying cameras to Lightwave!
	// Just gotta add them back into the listing...
	fs << wxT("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nZoomFactor 2.316845\nZoomType 2\n\n");
	WriteLWScenePlugin(fs,wxT("CameraHandler"),1,wxT("Perspective"));	// Make the camera a Perspective camera

	// Backdrop Settings
	// Add this if viewing a skybox, or using one as a background?

	// Rendering Options
	// Raytrace Shadows enabled.
	fs << wxT("RenderMode 2\nRayTraceEffects 1\nDepthBufferAA 0\nRenderLines 1\nRayRecursionLimit 16\nRayPrecision 6\nRayCutoff 0.01\nDataOverlayLabel  \nSaveRGB 0\nSaveAlpha 0\n");

	fs.close();

	// Export Doodad Files
	wxString cWMOName = m->name;
	if ((modelExport_LW_ExportDoodads ==  true)&&(modelExport_LW_DoodadsAs == 1)){
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

			wxLogMessage(wxT("Export: Attempting to export doodad model: %s"),cModelName.c_str());
			wxString dfile = wxString(fn,wxConvUTF8).BeforeLast(SLASH) << SLASH << cModelName.AfterLast(SLASH);
			dfile = dfile.BeforeLast(wxT('.')) << wxT(".lwo");

			g_modelViewer->canvas->LoadModel(cModelName);
			ExportLWO_M2(NULL, g_modelViewer->canvas->model, dfile.fn_str(), true);

			wxLogMessage(wxT("Export: Finished exporting doodad model: %s\n\n"),cModelName.c_str());

			// Delete the loaded model
			g_modelViewer->canvas->clearAttachments();
			g_modelViewer->canvas->model = NULL;
			g_modelViewer->isModel = false;
		}
	}
	//texturemanager.clear();

	// Reload our original WMO file.
	//wxLogMessage("Reloading original WMO file: %s",cWMOName.c_str());
	*/
}


//---------------------------------------------
// --== Object Info Gathering Functions ==--
//---------------------------------------------

// Gather M2 Data
LWObject GatherM2forLWO(Attachment *att, Model *m, bool init, const char *fn, LWScene &scene, bool announce){
	LWObject Object;
	if (!m){
		return Object;
		Object.~LWObject();
	}

	wxString filename(fn, wxConvUTF8);
	wxString scfilename = wxString(fn, wxConvUTF8).BeforeLast('.').Append(wxT(".lws"));

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		// Object
		Path << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Objects"));
		filename.Empty();
		filename << Path << SLASH << wxT("Objects") << SLASH << Name;
	}
	if (m->modelType != MT_CHAR){
		if (modelExport_PreserveDir == true){
			wxString Path1, Path2, Name;

			// Objects
			Path1 << filename.BeforeLast(SLASH);
			Name << filename.AfterLast(SLASH);
			Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			MakeDirs(Path1,Path2);
			filename.Empty();
			filename << Path1 << SLASH << Path2 << SLASH << Name;
		}
	}

	LWSceneObj SceneObj(filename,LW_ObjCount.GetPlus(),0);

	Object.SourceType = wxT("M2");
	if (announce == true)
		LogExportData(wxT("LWO"),m->modelname,filename);

	uint32 SurfCounter = 0;
	int32 cAnim = 0;
	size_t cFrame = 0;
	bool vertMsg = false;

	if ((m->animated == true) && (init == false)){
		cAnim = m->currentAnim;
		cFrame = m->animManager->GetFrame();
	}

	// Main Object
	LWLayer Layer;
	Layer.Name = wxString(m->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	Layer.ParentLayer = -1;
	std::vector<wxString> surfnamearray;

	// Bounding Box for the Layer
	/*if (m->bounds[0]){
		Layer.BoundingBox1 = m->bounds[0];
		Layer.BoundingBox2 = m->bounds[1];
	}*/

	wxLogMessage(wxT("M2 Texture List:"));
	for (size_t x=0;x<m->TextureList.size();x++){
		wxLogMessage(wxT("Image %i: %s"),x,m->TextureList[x].c_str());
	}

	// Mesh & Slot names
	wxString meshes[19] = {wxT("Hairstyles"), wxT("Facial1"), wxT("Facial2"), wxT("Facial3"), wxT("Braces"), wxT("Boots"), wxEmptyString, wxT("Ears"), wxT("Wristbands"),  wxT("Kneepads"), wxT("Pants"), wxT("Pants"), wxT("Tarbard"), wxT("Trousers"), wxEmptyString, wxT("Cape"), wxEmptyString, wxT("Eyeglows"), wxT("Belt") };
	wxString slots[15] = {wxT("Helm"), wxEmptyString, wxT("Shoulder"), wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxEmptyString, wxT("Right Hand Item"), wxT("Left Hand Item"), wxEmptyString, wxEmptyString, wxT("Quiver") };

	// Build Part Names
	// Seperated from the rest of the build for various reasons.
	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)){
			// Main Model
			int g = p.geoset;
			bool isFound = false;
			wxString partName;
			
			// Part Names
			int mesh = m->geosets[g].id / 100;
			if (m->modelType == MT_CHAR && mesh < 19 && meshes[mesh] != wxEmptyString){
				partName = wxString::Format(wxT("Geoset %03i - %s"),g,meshes[mesh].c_str());
			}else{
				partName = wxString::Format(wxT("Geoset %03i"),g);
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
	// Parts for Attached Objects
	if (att != NULL){
		if (att->model){
			wxLogMessage(wxT("Att Model found."));
		}
		for (uint32 c=0; c<att->children.size(); c++) {
			Attachment *att2 = att->children[c];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					wxLogMessage(wxT("AttChild Model found."));
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							bool isFound = false;
							wxString partName;

							int thisslot = att2->children[j]->slot;
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								partName = wxString::Format(wxT("Child %02i - %s"),j,slots[thisslot].c_str());
							}else{
								partName = wxString::Format(wxT("Child %02i - Slot %02i"),j,att2->children[j]->slot);
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
				}
			}
		}
	}

	// Process Passes
	for (unsigned short i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)){
			// Main Model
			int g = p.geoset;
			bool isFound = false;
			uint32 partID = i;
			uint32 surfID = i;
			uint32 *Vert2Point = new uint32[p.vertexEnd];
			float Surf_Diff = 1.0f;
			float Surf_Lum = 0.0f;

			wxString partName, matName;
			
			// Part Names
			int mesh = m->geosets[g].id / 100;
			if (m->modelType == MT_CHAR && mesh < 19 && meshes[mesh] != wxEmptyString){
				partName = wxString::Format(wxT("Geoset %03i - %s"),g,meshes[mesh].c_str());
			}else{
				partName = wxString::Format(wxT("Geoset %03i"),g);
			}
			for (uint32 x=0;x<Object.PartNames.size();x++){
				if (Object.PartNames[x] == partName){
					partID = x;
					break;
				}
			}

			// Surface Name
			matName = m->TextureList[p.tex].AfterLast(SLASH).BeforeLast(wxT('.'));
			if (matName.Len() == 0)
				matName = wxString::Format(wxT("Material_%03i"), p.tex);
/*
			if (p.useSpec == true){
				matName = matName + wxT("_Spec");
			}

			if (p.trans == true){
				matName = matName + wxT("_Trans");
			}
*/
			// If Luminous...
			if (p.unlit == true) {
				Surf_Diff = 0.0f;
				Surf_Lum = 1.0f;
				// Add Lum, just in case there's a non-luminous surface with the same name.
				matName = matName + wxT("_Lum");
			}

			for (size_t x=0;x<Object.Surfaces.size();x++){
				if (Object.Surfaces[x].Name == matName){
					isFound = true;
					surfID = (uint32)x;
					break;
				}
			}

			bool doubesided = (p.cull?true:false);
			//wxLogMessage(wxT("Doublesided: %s, P.Cull: %s"),(doubesided?wxT("true"):wxT("false")),(p.cull?wxT("true"):wxT("false")));

			// Add Images to Model
			LWClip ClipImage;
			// Image Filename
			wxString Texture = wxString(m->TextureList[p.tex].c_str(), wxConvUTF8);
			wxString TexturePath = Texture.BeforeLast(MPQ_SLASH);
			wxString texName = Texture.AfterLast(MPQ_SLASH).BeforeLast(wxT('.'));

			if (m->modelType == MT_CHAR){
				wxString charname = filename.AfterLast(SLASH).BeforeLast(wxT('.')) + wxT("_");
				if (texName.Find(MPQ_SLASH) != wxNOT_FOUND){
					texName = charname + Texture.AfterLast(MPQ_SLASH).BeforeLast(wxT('.'));
					TexturePath = wxT("");
				}
				if (texName.Find(wxT("Body")) != wxNOT_FOUND){
					texName = charname + wxT("Body");
				}
			}
			//wxLogMessage(wxT("Texture: %s\n\tTexPath: %s\n\ttexName: %s"),Texture,TexturePath,texName);

			// Image Data
			ClipImage.Filename = texName;
			ClipImage.Source = TexturePath;
			ClipImage.TagID = (uint32)Object.PartNames.size() + SurfCounter;
			Object.Images.push_back(ClipImage);
			LWSurf_Image SurfImage_Color(ClipImage.TagID, LW_TEXTUREAXIS_UV, 0, 0);

			// ExportName = Base Path + Texture name without paths.
			wxString ExportName = wxString(fn, wxConvUTF8).BeforeLast(SLASH) + SLASH + texName;
			//wxLogMessage(wxT("PrePath ExportName: %s, fn Path: %s"),ExportName,wxString(fn, wxConvUTF8).BeforeLast(SLASH));
			if (modelExport_LW_PreserveDir == true){
				wxString Path, Name;

				Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
				Name << ExportName.AfterLast(SLASH);

				MakeDirs(Path,wxT("Images"));

				ExportName.Empty();
				ExportName << Path << SLASH<<wxT("Images")<<SLASH << Name;
			}
			if ((modelExport_PreserveDir == true)&&(TexturePath.Len() > 0)){
				wxString Path1, Path2, Name;
				Path1 << ExportName.BeforeLast(SLASH);
				Name << texName;
				Path2 << TexturePath;

				MakeDirs(Path1,Path2);

				ExportName.Empty();
				ExportName << Path1 << SLASH << Path2 << SLASH << Name;
			}
			ExportName << wxT(".tga");
			//wxLogMessage(wxT("Image ExportName: %s"),ExportName);
			SaveTexture(ExportName);
			//SaveTexture2(ClipImage.Filename,ClipImage.Source,wxString(wxT("LWO")),wxString(wxT("tga")));

			LWSurface Surface(matName,wxString(m->TextureList[p.tex].c_str(), wxConvUTF8),SurfImage_Color,LWSurf_Image(),LWSurf_Image(),NULL,Surf_Diff,Surf_Lum,doubesided);
			Object.Surfaces.push_back(Surface);

			// Points
			for (uint32 v=p.vertexStart; v<p.vertexEnd; v++) {
				// --== Point Data ==--
				LWPoint Point;
				uint32 pointnum = (uint32)Layer.Points.size();

				// Points
				Vec3D vert;
				if ((m->animated == true) && (init == false) && (m->vertices)) {
					if (vertMsg == false){
						wxLogMessage(wxT("Using Verticies"));
						vertMsg = true;
					}
					vert = m->vertices[v];
				} else {
					if (vertMsg == false){
						wxLogMessage(wxT("Using Original Verticies"));
						vertMsg = true;
					}
					vert = m->origVertices[v].pos;
				}
				vert = Vec3D(vert.x,vert.y,-vert.z);	// Fixes X flipped verts

				Point.PointData = vert;
				Point.UVData = m->origVertices[v].texcoords;	// UV Data
				// Weight Data NYI

				Vert2Point[v] = pointnum;
				Layer.Points.push_back(Point);
			}

			// Weight Data
			for (size_t x=0;x<m->header.nBones;x++){
				bool useWMap = false;
				LWWeightMap wMap;
				//Bone *cbone = &m->bones[x];

				// Weight Map Name = Bone Name
				wxString bone_name = wxString::Format(wxT("Bone_%03i"), x);
				for (int j=0; j<BONE_MAX; ++j) {
					if (m->keyBoneLookup[j] == (int16)x) {
						bone_name = Bone_Names[j];
						break;
					}
				}

				wMap.WeightMapName = bone_name;
				wMap.BoneID = (uint32)x;
			
				// Weight Map Point & Value Data
				for (size_t v=p.vertexStart;v<p.vertexEnd;v++){
					ModelVertex& vertex = m->origVertices[v];

					for (size_t j=0;j<4;j++){
						if ((vertex.bones[j] != x)&&((vertex.bones[j] == 0)&&(vertex.weights[j] == 0)))
							continue;

						// Else...
						LWWeightInfo a;
						a.Value = (vertex.weights[j] / 255);
						a.PointID = (uint32)v;
						if (a.Value != 0.0f){
							useWMap = true;
							wMap.PData.push_back(a);
						}
					}
				}
				if (useWMap == true)
					Layer.Weights.push_back(wMap);
			}

			// Polys
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				// --== Polygon Data ==--	
				LWPoly Poly;
				Poly.PolyData.numVerts = 3;
				for (int x=0;x<3;x++){
					// Modify to Flip Polys
					int mod = 0;
					if (x == 1){
						mod = 1;
					}else if (x == 2){
						mod = -1;
					}

					// Polygon Indice
					uint32 a = p.indexStart + k + x + mod;
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
				Poly.Normals.NormalMapName = wxEmptyString; //Layer.Name + wxString(wxT("_NormalMap"));
				Layer.Polys.push_back(Poly);
			}
			SurfCounter++;
		}
	}

	// --== Attachments ==--
	if (att != NULL){
		/* Have yet to find an att->model, so skip it until we do.
		if (att->model){
		} */
		for (uint32 c=0; c<att->children.size(); c++) {
			Attachment *att2 = att->children[c];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					int boneID = -1;
					Model *mParent = NULL;

					wxLogMessage(wxT("Attached Child Model: %s"),mAttChild->name.c_str());
					wxLogMessage(wxT("Texture List:"));
					for (size_t x=0;x<mAttChild->TextureList.size();x++){
						wxLogMessage(wxT("Image %i: %s"),x,mAttChild->TextureList[x].c_str());
					}

					if (att2->parent) {
						mParent = static_cast<Model*>(att2->children[j]->parent->model);
						if (mParent)
							boneID = mParent->attLookup[att2->children[j]->id];
					}

					// Model Movement, Roation & Scale Data
					Vec3D mPos(0,0,0);
					Quaternion mRot(Vec4D(0,0,0,0));
					Vec3D mScale(1,1,1);

					Vec3D Seraph(1,1,1);
					Quaternion Niobe(Vec4D(0,0,0,0));

					if (boneID>-1) {
						Bone cbone = mParent->bones[mParent->atts[boneID].bone];
						Matrix mat = cbone.mat;
						Matrix rmat = cbone.mrot;

						// InitPose is a reference to the HandsClosed animation (#15), which is the closest to the Initial pose.
						// By using this animation, we'll get the proper scale for the items when in Init mode.
						int InitPose = 15;

						if (init == true){
							mPos = mParent->atts[boneID].pos;
							mScale = cbone.scale.getValue(InitPose,0);
							mRot = cbone.rot.getValue(InitPose,0);
						}else{
							// Rotations aren't working correctly... Not sure why.
							rmat.quaternionRotate(cbone.rot.getValue(cAnim,(uint32)cFrame));
							mat.scale(cbone.scale.getValue(cAnim,(uint32)cFrame));
							mat.translation(cbone.transPivot);

							mPos.x = mat.m[0][3];
							mPos.y = mat.m[1][3];
							mPos.z = mat.m[2][3];

							mScale = cbone.scale.getValue(cAnim,(uint32)cFrame);
							mRot = rmat.GetQuaternion();
						}
						if (mScale.x == 0 && mScale.y == 0 && mScale.z == 0){
							mScale = Vec3D(1,1,1);
						}
					}
/*
					wxLogMessage(wxT("mRot: X: %f, Y: %f, Z: %f, W: %f"),mRot.x,mRot.y,mRot.z,mRot.w);
					wxLogMessage(wxT("mPos: X: %f, Y: %f, Z: %f"),mPos.x,mPos.y,mPos.z);
					wxLogMessage(wxT("mScale: X: %f, Y: %f, Z: %f"),mScale.x,mScale.y,mScale.z);
*/
					for (uint32 i=0; i<mAttChild->passes.size(); i++) {
						ModelRenderPass &p = mAttChild->passes[i];

						if (p.init(mAttChild)) {
							bool isFound = false;
							uint32 partID = i;
							uint32 surfID = i;
							uint32 *Vert2Point = new uint32[p.vertexEnd];
							float Surf_Diff = 1.0f;
							float Surf_Lum = 0.0f;
							wxString partName, matName;
							
							int thisslot = att2->children[j]->slot;

							// Part Names
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								partName = wxString::Format(wxT("Child %02i - %s"),j,slots[thisslot].c_str());
							}else{
								partName = wxString::Format(wxT("Child %02i - Slot %02i"),j,att2->children[j]->slot);
							}
							for (uint32 x=0;x<Object.PartNames.size();x++){
								if (Object.PartNames[x] == partName){
									partID = x;
									break;
								}
							}

							// Surface Name
							matName = mAttChild->TextureList[p.tex].AfterLast(MPQ_SLASH).BeforeLast(wxT('.'));
							if (thisslot < 15 && slots[thisslot]!=wxEmptyString){
								if (matName != wxEmptyString){
									matName = wxString::Format(wxT("%s - %s"),slots[thisslot].c_str(),matName.c_str());
								}else {
									matName = wxString::Format(wxT("%s - Material %02i"),slots[thisslot].c_str(),p.tex);
								}
							}


							if (matName.Len() == 0)
								matName = wxString::Format(wxT("Child %02i - Material %03i"), j, p.tex);

							// If Luminous...
							if (p.unlit == true) {
								Surf_Diff = 0.0f;
								Surf_Lum = 1.0f;
								// Add Lum, just in case there's a non-luminous surface with the same name.
								matName = matName + wxT("_Lum");
							}

							for (size_t x=0;x<Object.Surfaces.size();x++){
								if (Object.Surfaces[x].Name == matName){
									isFound = true;
									surfID = (uint32)x;
									break;
								}
							}
							bool doubesided = (p.cull?true:false);

							// Add Images to Model
							LWClip ClipImage;
							// Image Filename
							wxString Texture = wxString(mAttChild->TextureList[p.tex].c_str(), wxConvUTF8);
							wxString TexturePath = Texture.BeforeLast(MPQ_SLASH);
							wxString texName = Texture.AfterLast(MPQ_SLASH).BeforeLast(wxT('.'));

							//wxLogMessage("Texture: %s\n\tTexurePath: %s\n\ttexName: %s",Texture,TexturePath,texName);

							// Image Data
							ClipImage.Filename = texName;
							ClipImage.Source = TexturePath;
							ClipImage.TagID = (uint32)Object.PartNames.size() + SurfCounter;
							Object.Images.push_back(ClipImage);
							LWSurf_Image SurfImage_Color(ClipImage.TagID, LW_TEXTUREAXIS_UV, 0, 0);

							wxString ExportName = wxString(fn, wxConvUTF8).BeforeLast(SLASH) + SLASH + texName;
							if (modelExport_LW_PreserveDir == true){
								wxString Path, Name;

								Path << wxString(fn, wxConvUTF8).BeforeLast(SLASH);
								Name << ExportName.AfterLast(SLASH);

								MakeDirs(Path,wxT("Images"));

								ExportName.Empty();
								ExportName << Path << SLASH<<wxT("Images")<<SLASH << Name;
							}
							if (modelExport_PreserveDir == true){
								wxString Path1, Path2, Name;
								Path1 << ExportName.BeforeLast(SLASH);
								Name << texName.AfterLast(SLASH);
								Path2 << TexturePath;

								MakeDirs(Path1,Path2);

								ExportName.Empty();
								ExportName << Path1 << SLASH << Path2 << SLASH << Name;
							}
							ExportName <<  wxT(".tga");

							SaveTexture(ExportName);

							LWSurface Surface(matName,Texture.BeforeLast('.'),SurfImage_Color,LWSurf_Image(),LWSurf_Image(),NULL,Surf_Diff,Surf_Lum,doubesided);
							Object.Surfaces.push_back(Surface);

							// Points
							for (uint32 v=p.vertexStart; v<p.vertexEnd; v++) {
								// --== Point Data ==--
								LWPoint Point;
								uint32 pointnum = (uint32)Layer.Points.size();

								// Points
								Vec3D vert;
								if ((init == false)&&(mAttChild->vertices)) {
									vert = mAttChild->vertices[v];
								} else {
									vert = mAttChild->origVertices[v].pos;
								}
								Matrix Neo;

								Neo.translation(vert);							// Set Original Position
								Neo.quaternionRotate(Niobe);					// Set Original Rotation
								Neo.scale(Seraph);								// Set Original Scale

								Neo *= Matrix::newTranslation(mPos);			// Apply New Position
								Neo *= Matrix::newQuatRotate(mRot);				// Apply New Rotation
								Neo *= Matrix::newScale(mScale);				// Apply New Scale

								Vec3D mVert = Neo * vert;
								mVert = Vec3D(mVert.x,mVert.y,-mVert.z);		// Fixes X flipped verts

								Point.PointData = mVert;
								Point.UVData = mAttChild->origVertices[v].texcoords;	// UV Data
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
									// Modify to Flip Polys
									int mod = 0;
									if (x == 1){
										mod = 1;
									}else if (x == 2){
										mod = -1;
									}

									// Polygon Indice
									uint32 a = p.indexStart + k + x + mod;
									uint32 b = mAttChild->IndiceToVerts[a];
									Poly.PolyData.indice[x] = Vert2Point[b];

									// Normal Indice
									/*Poly.Normals.indice[x] = Vert2Point[b];
									Poly.Normals.direction[x].x = mAttChild->normals[b].x;
									Poly.Normals.direction[x].y = mAttChild->normals[b].z;
									Poly.Normals.direction[x].z = -mAttChild->normals[b].y;*/
								}
								Poly.PartTagID = partID;
								Poly.SurfTagID = (uint32)Object.PartNames.size() + SurfCounter;
								//Poly.Normals.polygon = (uint32)Layer.Polys.size();
								Poly.Normals.NormalMapName = wxEmptyString; //Layer.Name + wxString(wxT("_NormalMap"));
								Layer.Polys.push_back(Poly);
							}
							SurfCounter++;
						}
					}
				}
			}
		}
	}
	Object.Layers.push_back(Layer);

	// --== Scene Data ==--
	int modelExport_LW_ExportAnim = 0;	// Temp here until I build the interface.
	//bool animExportError = false;
	// Object Placement
	AnimationData a(animValue0,animValue0,animValue1);
	SceneObj.AnimData = a;

	// Bones
	if (m->header.nBones > 0){
		for (size_t x=0;x<m->header.nBones;x++){
			LWBones lwBone((uint32)x);
			Bone *cbone = &m->bones[x];

			Vec3D Pos = cbone->pivot;	// For Animation & Init Only
			Vec3D Pos_i = Pos;			// For Non-Init Only Exporting
			if (cbone->parent > LW_ITEMTYPE_NOPARENT){
				Pos -= m->bones[cbone->parent].pivot;
				lwBone.ParentType = LW_ITEMTYPE_BONE;
				lwBone.ParentID = cbone->parent;
			}else{
				lwBone.Active = false;		// Disable Non-Parented bones.
				lwBone.ParentType = LW_ITEMTYPE_OBJECT;
				lwBone.ParentID = LW_ObjCount.GetValue();
			}
			Pos_i = Pos;
			if (init == false){
				Pos_i = cbone->transPivot;
				if (cbone->parent > LW_ITEMTYPE_NOPARENT){
					Pos_i -= m->bones[cbone->parent].transPivot;
					lwBone.Active = true;
				}
				Pos_i.z = -Pos_i.z;
				MakeModelFaceForwards(Pos_i,false);
				lwBone.RestPos = Pos_i;
			}else{
				Pos.z = -Pos.z;
				MakeModelFaceForwards(Pos,false);
				lwBone.RestPos = Pos;
				lwBone.RestRot = Vec3D();
			}
			lwBone.RestRot = Vec3D();
			AnimVec3D animPos_i = AnimVec3D(AnimVector(Pos_i.x,0),AnimVector(Pos_i.y,0),AnimVector(Pos_i.z,0));
			AnimVec3D animPos, animRot, animSca;
			if (modelExport_LW_ExportAnim > 0){
				// Position
				AnimVector PosX, PosY, PosZ;
				for (size_t i=0;i<cbone->trans.data[cAnim].size();i++){
					uint32 aTime = cbone->trans.times[cAnim][i];
					float fTime = (float)aTime;
					Vec3D aPos = cbone->pivot; //cbone->trans.getValue(cAnim,aTime);
					if (cbone->parent > LW_ITEMTYPE_NOPARENT)
						aPos -= m->bones[cbone->parent].pivot;
					//aPos.z = -aPos.z;
					//MakeModelFaceForwards(aPos,false);
					if (fTime/FRAMES_PER_SECOND < scene.FirstFrame){
						scene.FirstFrame = fTime/FRAMES_PER_SECOND;
					}
					if (fTime/FRAMES_PER_SECOND > scene.LastFrame){
						scene.LastFrame = fTime/FRAMES_PER_SECOND;
					}
					PosX.Push(aPos.x,fTime,cbone->trans.type);
					PosY.Push(aPos.y,fTime,cbone->trans.type);
					PosZ.Push(aPos.z,fTime,cbone->trans.type);
				}
				if (PosX.Size() > 0){
					animPos = AnimVec3D(PosX,PosY,PosZ);
				}else{
					animPos = AnimVec3D(AnimVector(Pos.x,0),AnimVector(Pos.y,0),AnimVector(Pos.z,0));
				}

				// Rotation
				AnimVector RotX, RotY, RotZ;
				/*for (size_t i=0;i<cbone->rot.data[cAnim].size();i++){
					uint32 aTime = cbone->rot.times[cAnim][i];
					float fTime = (float)aTime;
					Vec3D HPB;
					Quaternion q = cbone->rot.getValue(cAnim,aTime);
					Quaternion tq;
					tq.x = q.w; tq.y = q.x; tq.z = q.y; tq.w = q.z;
					Matrix Neo;
					QuaternionToRotationMatrix(tq, Neo);
					RotationMatrixToEulerAnglesXYZ(Neo, HPB.x, HPB.y, HPB.z);

					HPB.x = HPB.x * (180.0f / (float)PI);
					HPB.y = HPB.y * (180.0f / (float)PI);
					HPB.z = HPB.z * (180.0f / (float)PI);


					if (fTime/FRAMES_PER_SECOND < scene.FirstFrame){
						scene.FirstFrame = fTime/FRAMES_PER_SECOND;
					}
					if (fTime/FRAMES_PER_SECOND > scene.LastFrame){
						scene.LastFrame = fTime/FRAMES_PER_SECOND;
					}
					RotX.Push(HPB.x,fTime,cbone->rot.type);
					RotY.Push(HPB.y,fTime,cbone->rot.type);
					RotZ.Push(HPB.z,fTime,cbone->rot.type);
				}
				if (RotX.Size() > 0){
					animRot = AnimVec3D(RotX,RotY,RotZ);
				}else{*/
					animRot = animValue0;
			//	}

				// Scale
				AnimVector ScaX, ScaY, ScaZ;
				for (size_t i=0;i<cbone->scale.data[cAnim].size();i++){
					uint32 aTime = cbone->scale.times[cAnim][i];
					float fTime = (float)aTime;

					Vec3D Sc = cbone->scale.getValue(cAnim,aTime);

					if (fTime/FRAMES_PER_SECOND < scene.FirstFrame){
						scene.FirstFrame = fTime/FRAMES_PER_SECOND;
					}
					if (fTime/FRAMES_PER_SECOND > scene.LastFrame){
						scene.LastFrame = fTime/FRAMES_PER_SECOND;
					}
					ScaX.Push(Sc.x,fTime,cbone->scale.type);
					ScaY.Push(Sc.y,fTime,cbone->scale.type);
					ScaZ.Push(Sc.z,fTime,cbone->scale.type);
				}
				if (ScaX.Size() > 0){
					animSca = AnimVec3D(ScaX,ScaY,ScaZ);
				}else{
					animSca = animValue1;
				}
			}else{
				animPos = AnimVec3D(AnimVector(Pos.x,0),AnimVector(Pos.y,0),AnimVector(Pos.z,0));
				animRot = animValue0;
				animSca = animValue1;
			}

			if (init == false)
				lwBone.AnimData = AnimationData(animPos_i,animValue0,animValue1);
			else if (modelExport_LW_ExportAnim > 0)
				lwBone.AnimData = AnimationData(animPos,animRot,animSca);
			else
				lwBone.AnimData = AnimationData(animPos,animValue0,animValue1);

			wxString bone_name = wxString::Format(wxT("Bone_%03i"), x);
			for (size_t j=0; j<BONE_MAX; ++j) {
				if (m->keyBoneLookup[j] == (uint16)x) {
					bone_name = Bone_Names[j];
					break;
				}
			}
			lwBone.Name = bone_name;
			for (size_t j=0;j<Object.Layers[0].Weights.size();j++){
				if (Object.Layers[0].Weights[j].WeightMapName == bone_name){
					lwBone.WeightMap_Name = bone_name;
					break;
				}
			}
			

			SceneObj.Bones.push_back(lwBone);
		}
	}

	// Lights
	uint32 nLights = m->header.nLights;
	for (size_t x=0;x<nLights;x++){
		ModelLight *l = &m->lights[x];
		LWLight Light;

		Light.LightID = LW_LightCount.GetPlus();
		Light.Name = m->name.AfterLast('\\').BeforeLast('.');
		Light.Name << wxString::Format(wxT(" Light %02i"), Light.LightID);
		if (l->parent > LW_ITEMTYPE_NOPARENT){
			Light.ParentType = LW_ITEMTYPE_BONE;
			Light.ParentID = l->parent;
		}else{
			Light.ParentType = LW_ITEMTYPE_OBJECT;
			Light.ParentID = LW_ObjCount.GetValue();
		}

		Light.Color = l->diffColor.getValue(0,0);
		Light.Intensity = l->diffIntensity.getValue(0,0);
		Light.UseAttenuation = false;
		Light.FalloffRadius = l->AttenEnd.getValue(0,0);

		AnimVec3D animPos = AnimVec3D(AnimVector(l->pos.x,0),AnimVector(l->pos.y,0),AnimVector(l->pos.z,0));
		Light.AnimData = AnimationData(animPos,animValue0,animValue1);

		if (l->UseAttenuation.getValue(0,0) > 0){
			Light.UseAttenuation = true;
		}

		scene.Lights.push_back(Light);
	}
	/*
	if (att != NULL){
		if (att->model){
			wxLogMessage(wxT("Att Model found."));
		}
		for (uint32 c=0; c<att->children.size(); c++) {
			Attachment *att2 = att->children[c];
			for (uint32 j=0; j<att2->children.size(); j++) {
				Model *mAttChild = static_cast<Model*>(att2->children[j]->model);

				if (mAttChild){
					for (size_t x=0;x<mAttChild->header.nLights;x++){
						ModelLight *l = &mAttChild->lights[x];

						Vec3D color = l->diffColor.getValue(0,0);
						float intense = l->diffIntensity.getValue(0,0);
						bool useAtten = false;
						float AttenEnd = l->AttenEnd.getValue(0,0);

						if (l->UseAttenuation.getValue(0,0) > 0){
							useAtten = true;
						}

						//WriteLWSceneLight(fs,lcount,l->pos,l->type,color,intense,useAtten,AttenEnd,2.5);
					}
				}
			}
		}
	}
	*/
	// Cameras
	/*
	if (m->hasCamera == true){
		ModelCamera *cam = &m->cam;
		LWCamera C;

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
				//CamData.Push(a,ZeroRot,OneScale,ctime/30);
				if (ctime > RangeEnd)
					RangeEnd = ctime;
			}
		}else{
			CamData.Push(cam->target,ZeroRot,OneScale);
		}

		//WriteLWSceneObject(fs,wxT("Camera Target"),CamData,mcount,0,true,ModelID);

		fs << wxT("AddCamera 30000000\nCameraName Camera\nShowCamera 1 -1 0.125490 0.878431 0.125490\nCameraMotion\nNumChannels 6\n");

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

		fs << wxT("ParentItem 1" << wxString::Format(wxT("%07x"),ModelID) << "\n");
		fs << wxT("IKInitCustomFrame 0\nGoalStrength 1\nIKFKBlending 0\nIKSoftMin 0.25\nIKSoftMax 0.75\nCtrlPosItemBlend 1\nCtrlRotItemBlend 1\nCtrlScaleItemBlend 1\n\n");
		fs << wxT("HController 1\nPController 1\nPathAlignLookAhead 0.033\nPathAlignMaxLookSteps 10\nPathAlignReliableDist 0.001\n");
		fs << wxT("TargetItem 1"<<wxString::Format(wxT("%07x"),CameraTargetID)<<"\n");
		fs << wxT("ZoomFactor "<<(cam->fov*3.6)<<"\nZoomType 2\n");
		WriteLWScenePlugin(fs,wxT("CameraHandler"),1,wxT("Perspective"));	// Make the camera a Perspective camera
	} */

	scene.Objects.push_back(SceneObj);

	return Object;
	Object.~LWObject();
}

// Gather WMO Data
LWObject GatherWMOforLWO(WMO *m, const char *fn, LWScene &scene){
	wxString RootDir(fn, wxConvUTF8);
	wxString FileName(fn, wxConvUTF8);

	LWObject Object;

	if (!m){
		return Object;
		Object.~LWObject();
	}

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << FileName.BeforeLast(SLASH);
		Name << FileName.AfterLast(SLASH);
		MakeDirs(Path,wxT("Objects"));
		FileName.Empty();
		FileName << Path << SLASH << wxT("Objects") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;

		Path1 << FileName.BeforeLast(SLASH);
		Name << FileName.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		MakeDirs(Path1,Path2);
		FileName.Empty();
		FileName << Path1 << SLASH << Path2 << SLASH << Name;
	}

	Object.SourceType = wxT("WMO");
	LogExportData(wxT("LWO"),m->name,FileName);

	// Main Object
	LWLayer Layer;
	Layer.Name = wxString(m->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	Layer.ParentLayer = -1;

	// Bounding Box for the Layer
	Layer.BoundingBox1 = m->v1;
	Layer.BoundingBox2 = m->v2;

	uint32 SurfCounter = 0;

	// Process Groups
	for (unsigned int g=0;g<m->nGroups; g++) {
		WMOGroup *group = &m->groups[g];
		//uint32 GPolyCounter = 0;

		//wxLogMessage(wxT("\nGroup %i Info:\n   Batches: %i\n   Indices: %i\n   Vertices: %i"),g,group->nBatches,group->nIndices,group->nVertices);
		Object.PartNames.push_back(wxString(group->name.c_str(), wxConvUTF8));

		Layer.HasVectorColors = group->hascv;

		// Points Batches
		for (uint32 b=0; b<group->nBatches; b++){
			WMOBatch *batch = &group->batches[b];
			//wxLogMessage(wxT("\nBatch %i Info:\n   Indice-Start: %i\n   Indice-Count: %i\n   Vert-Start: %i\n   Vert-End: %i"),b,batch->indexStart,batch->indexCount,batch->vertexStart,batch->vertexEnd);
			uint32 *Vert2Point = new uint32[group->nVertices];

			uint32 t = batch->texture;
			WMOMaterial *mat = &m->mat[t];
			bool doubesided = ((mat->flags & WMO_MATERIAL_CULL)?false:true);

			// Add Images to Model
			LWClip ClipImage;
			wxString Texture = m->textures[t];
			wxLogMessage(wxT("Texture: %s"),Texture.c_str());

			ClipImage.Filename = Texture.AfterLast(SLASH).BeforeLast('.');
			ClipImage.Source = Texture.BeforeLast(SLASH);
			ClipImage.TagID = m->nGroups + SurfCounter;
			Object.Images.push_back(ClipImage);

			wxString LWFilename = Texture;
			if (modelExport_LW_PreserveDir == true){
				wxString Path = RootDir.BeforeLast(SLASH);
				wxString Name = Texture.AfterLast(SLASH);

				MakeDirs(Path,wxT("Images"));

				LWFilename.Empty();
				LWFilename <<Path<<SLASH<<wxT("Images")<<SLASH<<Name;
			}
			if (modelExport_PreserveDir == true){
				wxString Path1(LWFilename.BeforeLast(SLASH));
				wxString Path2(Texture.BeforeLast(SLASH));
				wxString Name(LWFilename.AfterLast(SLASH));
				//wxLogMessage(wxT("Path1 (root): %s, Path2(Img Path): %s, Name (Img Name): %s"),Path1,Path2,Name);

				MakeDirs(Path1,Path2);

				LWFilename.Empty();
				LWFilename << Path1<<SLASH<<Path2<<SLASH<<Name;
			}
			//LWFilename = LWFilename.BeforeLast('.') + wxT(".tga");
			SaveTexture2(Texture,RootDir.BeforeLast(SLASH),wxT("LWO"),wxT("tga"));

			LWSurf_Image SurfColor_Image = LWSurf_Image(ClipImage.TagID,LW_TEXTUREAXIS_UV,0.0f,0.0f);

			LWSurface Surface(ClipImage.Filename,Texture,SurfColor_Image,LWSurf_Image(),LWSurf_Image(),Vec3D(0.75,0.75,0.75),1,0,doubesided,Layer.HasVectorColors);
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
				//wxLogMessage(wxT("Vert %i = Point %i"),v,pointnum);
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
					//wxLogMessage(wxT("Group: %i, a: %i, b:%i, Final Indice: %i"),g,a,b,Vert2Point[b]);
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
				Poly.Normals.NormalMapName = Layer.Name + wxString(wxT("_NormalMap"));
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
	wxLogMessage(wxT("Completed WMO Gathering. Building Basic Scene Data..."));

	// Scene Data
	LWSceneObj scObject(RootDir,(uint32)scene.Objects.size(),-1);
	scene.Objects.push_back(scObject);
	if ((modelExport_LW_ExportLights == true) && (m->nLights > 0)){
		for (size_t x=0;x<m->nLights;x++){
			size_t LightID = scene.Lights.size();
			LWLight l;
			WMOLight cl = m->lights[x];
			Vec3D Lcolor(cl.fcolor.x,cl.fcolor.y, cl.fcolor.z);
			float Lint = cl.intensity;

			while ((Lcolor.x > 1.0f)||(Lcolor.y > 1.0f)||(Lcolor.z > 1.0f)) {
				Lcolor.x = Lcolor.x * 0.99;
				Lcolor.y = Lcolor.y * 0.99;
				Lcolor.z = Lcolor.z * 0.99;
				Lint = Lint / 0.99;
			}
			
			l.LightID = (uint32)LightID;
			l.Color = Lcolor;
			l.Intensity = Lint;
//			l.AnimData.Push(cl.pos,Vec3D(0,0,0),Vec3D(1,1,1),0);
			l.FalloffRadius = 2.5f;
			if (cl.useatten > 0) {
				l.FalloffRadius = cl.attenEnd;
			}
			l.LightType = LW_LIGHTTYPE_POINT;
			l.ParentType = LW_ITEMTYPE_OBJECT;
			l.ParentID = 0;		// These lights will always be parented to the WMO object.
			wxString lNum,liNum;
			lNum << (unsigned int)LightID;
			liNum << (unsigned int)m->nLights;
			while (lNum.Len() < liNum.Len()){
				lNum = wxString(wxT("0")) + lNum;
			}
			wxString lName(m->name.AfterLast('\\').BeforeLast('.'));
			l.Name = lName << wxT(" Light ") << (wxChar)LightID;

			scene.Lights.push_back(l);
		}
	}

	// Doodads
	wxLogMessage(wxT("Compiled Scene Data. Checking Doodads..."));
/*
	Need to get this working with layers before we enable it.
	For now, use the old method in the main function.	
*/
	/*
	if (modelExport_LW_ExportDoodads == true){
		Vec3D Vec3d_0 = Vec3D(0,0,0);
		Vec3D Vec3d_1 = Vec3D(1,1,1);
		if (modelExport_LW_DoodadsAs == 0){			// Doodads as Nulls...
			for (uint32 ds=0;ds<m->nDoodadSets;ds++){
				size_t ddSetID = scene.Objects.size();
				LWSceneObj doodadset(m->doodadsets[ds].name,(uint32)ddSetID,0,LW_ITEMTYPE_OBJECT,true);
				doodadset.AnimData.Push(Vec3d_0,Vec3d_0,Vec3d_1,0);
				scene.Objects.push_back(doodadset);

				wxLogMessage(wxT("Processing Doodadset %i: %s"),ds,m->doodadsets[ds].name);
				for (uint32 dd=m->doodadsets[ds].start;dd<(m->doodadsets[ds].start+m->doodadsets[ds].size);dd++){
					WMOModelInstance *ddinstance = &m->modelis[dd];
					size_t ddID = scene.Objects.size();

					LWSceneObj doodad(ddinstance->filename,(uint32)ddID,(int32)ddSetID,LW_ITEMTYPE_OBJECT,true);
					Vec3D rot = QuaternionToXYZ(ddinstance->dir,ddinstance->w);
					rot.x -= (float)(PI/2);
					Vec3D scale = Vec3D(ddinstance->sc,ddinstance->sc,ddinstance->sc);
					doodad.AnimData.Push(ddinstance->pos,rot,scale,0);

					scene.Objects.push_back(doodad);

					uint32 ddnLights = ddinstance->model->header.nLights;
					Model *ddm = ddinstance->model;
					if ((modelExport_LW_ExportLights == true) && (ddnLights > 0)){
						for (size_t x=0;x<ddnLights;x++){
							size_t LightID = scene.Lights.size();
							LWLight l;
							ModelLight cl = ddm->lights[x];
							Vec3D diffColor = cl.diffColor.getValue(0,0);
							Vec3D Lcolor(diffColor.x, diffColor.y, diffColor.z);
							float Lint = cl.diffIntensity.getValue(0,0);

							while ((Lcolor.x > 1.0f)||(Lcolor.y > 1.0f)||(Lcolor.z > 1.0f)) {
								Lcolor.x = Lcolor.x * 0.99;
								Lcolor.y = Lcolor.y * 0.99;
								Lcolor.z = Lcolor.z * 0.99;
								Lint = Lint / 0.99;
							}
							
							l.LightID = (uint32)LightID;
							l.Color = Lcolor;
							l.Intensity = Lint;
							if (cl.type == MODELLIGHT_DIRECTIONAL){
								l.LightType = LW_LIGHTTYPE_DISTANT;
							}else{
								l.LightType = LW_LIGHTTYPE_POINT;
							}
							l.AnimData.Push(cl.pos,Vec3D(0,0,0),Vec3D(1,1,1),0);

							l.FalloffRadius = 2.5f;
							if (cl.UseAttenuation.getValue(0,0) > 0) {
								l.FalloffRadius = cl.AttenEnd.getValue(0,0);
							}
							l.ParentType = LW_ITEMTYPE_OBJECT;
							l.ParentID = (int32)ddID;
							wxString lNum,liNum;
							lNum << (unsigned int)LightID;
							liNum << (unsigned int)m->nLights;
							while (lNum.Len() < liNum.Len()){
								lNum = wxString(wxT("0")) + lNum;
							}
							wxString lName(m->name.AfterLast('\\').BeforeLast('.'));
							l.Name = lName << wxT(" Light ") << (wxChar)LightID;

							LightID++;
							scene.Lights.push_back(l);
						}
					}
				}
			}
		}else if (modelExport_LW_DoodadsAs == 1){	// Doodads as Scene Objects...
			LWObject Doodad = GatherM2forLWO(NULL,ddinstance->model,true,FileName,false);
			if (Doodad.SourceType == wxEmptyString){
				wxMessageBox(wxT("Error gathering information for export."),wxT("Export Error"));
				wxLogMessage(wxT("Failure gathering information for export."));
				return;
			}
			WriteLWObject(filename, Object);
		}else if (modelExport_LW_DoodadsAs == 2){	// Doodad Sets as Seperate Layers...
			for (uint32 ds=0;ds<m->nDoodadSets;ds++){
				wxLogMessage(wxT("Processing Doodadset %i: %s"),ds,m->doodadsets[ds].name);
				bool doodadAdded = false;

				wxLogMessage(wxT("Adding %i Doodads to Layer %i..."), m->doodadsets[ds].size, ds+1);
				for (uint32 dd=m->doodadsets[ds].start;dd<(m->doodadsets[ds].start+m->doodadsets[ds].size);dd++){
					WMOModelInstance *ddinstance = &m->modelis[dd];
					wxLogMessage(wxT("Processing Doodad %i: %s"),dd,ddinstance->filename);

					wxLogMessage(wxT("Doodad Instance is Animated: %s"),(ddinstance->model->animated?wxT("true"):wxT("false")));

					LWObject Doodad = GatherM2forLWO(NULL,ddinstance->model,true,FileName,scene,false);

					// --== Model Debugger ==--
					// Exports the model immediately after gathering, to help determine if a problem is with the gathering function or the doodad-placement functions.
					wxString doodadname = wxString(fn, wxConvUTF8).BeforeLast('.') << wxT("_") << ddinstance->filename.AfterLast('\\').BeforeLast('.') << wxT(".lwo");
					if (modelExport_LW_PreserveDir == true){
						wxString Path, Name;

						Path << doodadname.BeforeLast(SLASH);
						Name << doodadname.AfterLast(SLASH);

						MakeDirs(Path,wxT("Objects"));

						doodadname.Empty();
						doodadname << Path << SLASH << wxT("Objects") << SLASH << Name;
					}
					if (modelExport_PreserveDir == true){
						wxString Path1, Path2, Name;
						Path1 << doodadname.BeforeLast(SLASH);
						Name << doodadname.AfterLast(SLASH);
						Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);

						MakeDirs(Path1,Path2);

						doodadname.Empty();
						doodadname << Path1 << SLASH << Path2 << SLASH << Name;
					}
					wxLogMessage(wxT("Exporting Doodad Test Model: %s"),doodadname);
					WriteLWObject(doodadname, Doodad);
					// End Model Debugger

					if (Doodad.SourceType == wxEmptyString){
						wxLogMessage(wxT("Error Gathering Doodad Model."));
						continue;
					}
					//wxLogMessage(wxT("Finished Gathering Doodad %i with #%i Layers."),dd,Doodad.Layers.size());

					// Move, rotate & scale Doodad
					Vec3D TheArchitect = ddinstance->pos;
					Vec3D Trinity(ddinstance->sc,-(ddinstance->sc),-(ddinstance->sc));
					Vec3D Seraph(1,1,1);
					Vec3D Oracle(-(ddinstance->dir.z),ddinstance->dir.x,ddinstance->dir.y);
					Quaternion Morphius(Oracle,ddinstance->w);
					Quaternion Niobe(Vec4D(0,0,0,0));

					for (uint32 i=0;i<Doodad.Layers[0].Points.size();i++){
						Vec3D AgentSmith = Doodad.Layers[0].Points[i].PointData;

						Matrix Neo;

						Neo.translation(AgentSmith);					// Set Original Position
						Neo.quaternionRotate(Niobe);					// Set Original Rotation
						Neo.scale(Seraph);								// Set Original Scale

						Neo *= Matrix::newTranslation(TheArchitect);	// Apply New Position
						Neo *= Matrix::newQuatRotate(Morphius);			// Apply New Rotation
						Neo *= Matrix::newScale(Trinity);				// Apply New Scale

						Doodad.Layers[0].Points[i].PointData = Neo * AgentSmith;
					}

					wxString ddPrefix(wxT("Doodad "));
					wxString ddnum, numdds;
					ddnum << (unsigned int)dd;
					numdds << (unsigned int)m->doodadsets[ds].size;
					while (ddnum.Length() < numdds.Length()) {
						ddnum = wxString(wxT("0")) << ddnum;
					}					
					ddPrefix << ddnum << wxT("_") << m->modelis[dd].filename.AfterLast('\\').BeforeLast('.') << wxT("_");
					//wxLogMessage(wxT("Doodad Prefix: %s"),ddPrefix);
					Object.Plus(Doodad,ds+1,ddPrefix);
					doodadAdded = true;
					Doodad.~LWObject();

					uint32 ddID = (uint32)scene.Objects.size();

					LWSceneObj ddlyr(FileName,ddID,0,LW_ITEMTYPE_OBJECT,false,(ds+2));
					scene.Objects.push_back(ddlyr);

					uint32 ddnLights = ddinstance->model->header.nLights;
					Model *ddm = ddinstance->model;
					if ((modelExport_LW_ExportLights == true) && (ddnLights > 0)){
						for (size_t x=0;x<ddnLights;x++){
							size_t LightID = scene.Lights.size();
							LWLight l;
							ModelLight cl = ddm->lights[x];
							Vec3D diffColor = cl.diffColor.getValue(0,0);
							Vec3D Lcolor(diffColor.x, diffColor.y, diffColor.z);
							float Lint = cl.diffIntensity.getValue(0,0);

							while ((Lcolor.x > 1.0f)||(Lcolor.y > 1.0f)||(Lcolor.z > 1.0f)) {
								Lcolor.x = Lcolor.x * 0.99;
								Lcolor.y = Lcolor.y * 0.99;
								Lcolor.z = Lcolor.z * 0.99;
								Lint = Lint / 0.99;
							}

							l.LightID = (uint32)LightID;
							l.Color = Lcolor;
							l.Intensity = Lint;
							if (cl.type == MODELLIGHT_DIRECTIONAL){
								l.LightType = LW_LIGHTTYPE_DISTANT;
							}else{
								l.LightType = LW_LIGHTTYPE_POINT;
							}
							l.AnimData.Push(cl.pos,Vec3D(0,0,0),Vec3D(1,1,1),0);

							l.FalloffRadius = 2.5f;
							if (cl.UseAttenuation.getValue(0,0) > 0) {
								l.FalloffRadius = cl.AttenEnd.getValue(0,0);
							}
							l.ParentType = LW_ITEMTYPE_OBJECT;
							l.ParentID = (int32)ddID;
							wxString lNum,liNum;
							lNum << (unsigned int)LightID;
							liNum << (unsigned int)m->nLights;
							while (lNum.Len() < liNum.Len()){
								lNum = wxString(wxT("0")) + lNum;
							}
							wxString lName(m->name.AfterLast('\\').BeforeLast('.'));
							l.Name = lName << wxT(" Light ") << (wxChar)LightID;

							LightID++;
							scene.Lights.push_back(l);
						}
					}
				}
				if (doodadAdded == true){
					Object.Layers[ds+1].Name = wxString(m->doodadsets[ds].name);
					Object.Layers[ds+1].ParentLayer = 0;		// Set Parent to main WMO model.
				}
			}
		}else if (modelExport_LW_DoodadsAs == 3){	// All Doodads as a Single Layer...
		}else if (modelExport_LW_DoodadsAs == 4){	// Each Group's Doodads as a Single Layer...
		}
	}
	*/
	return Object;
	Object.~LWObject();
}



// Gather ADT Data
LWObject GatherADTforLWO(MapTile *m, const char *fn, LWScene &scene){
	wxString FileName(fn, wxConvUTF8);
	LWObject Object;

	if (!m){
		return Object;
		Object.~LWObject();
	}

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		Path << FileName.BeforeLast(SLASH);
		Name << FileName.AfterLast(SLASH);
		MakeDirs(Path,wxT("Objects"));
		FileName.Empty();
		FileName << Path << SLASH << wxT("Objects") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;

		Path1 << FileName.BeforeLast(SLASH);
		Name << FileName.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		MakeDirs(Path1,Path2);
		FileName.Empty();
		FileName << Path1 << SLASH << Path2 << SLASH << Name;
	}

	Object.SourceType = wxT("ADT");
	LogExportData(wxT("LWO"),m->name,FileName);

	// Main Object
	LWLayer Layer;
	Layer.Name = wxString(m->name.c_str(), wxConvUTF8).AfterLast('\\').BeforeLast('.');
	Layer.ParentLayer = -1;

	// Bounding Box for the Layer
	//Layer.BoundingBox1 = m->v1;
	//Layer.BoundingBox2 = m->v2;

	for (int c1=0;c1<16;c1++){
		for (int c2=0;c2<16;c2++){
			MapChunk *chunk = &m->chunks[c1][c2];
			for (int num=0;num<145;num++){
				LWPoint a;
				a.PointData = chunk->tv[num];
				Layer.Points.push_back(a);
			}
		}
	}
	Object.Layers.push_back(Layer);

	return Object;
	Object.~LWObject();
}

//---------------------------------------------
// --== Global Exporting Functions ==--
//---------------------------------------------

// Export M2s
void ExportLWO_M2(Attachment *att, Model *m, const char *fn, bool init){
	wxString filename(fn, wxConvUTF8);
	wxString scfilename = wxString(fn, wxConvUTF8).BeforeLast('.').Append(wxT(".lws"));
	LW_ObjCount.Reset();
	LW_LightCount.Reset();
	LW_BoneCount.Reset();
	LW_CamCount.Reset();

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		// Object
		Path << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Objects"));
		filename.Empty();
		filename << Path << SLASH << wxT("Objects") << SLASH << Name;

		// Scene
		Path.Empty();
		Name.Empty();
		Path << scfilename.BeforeLast(SLASH);
		Name << scfilename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Scenes"));
		scfilename.Empty();
		scfilename << Path << SLASH << wxT("Scenes") << SLASH << Name;
	}
	if (m->modelType != MT_CHAR){
		if (modelExport_PreserveDir == true){
			wxString Path1, Path2, Name;

			// Objects
			Path1 << filename.BeforeLast(SLASH);
			Name << filename.AfterLast(SLASH);
			Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			MakeDirs(Path1,Path2);
			filename.Empty();
			filename << Path1 << SLASH << Path2 << SLASH << Name;

			// Scene
			Path1.Empty();
			Path2.Empty();
			Name.Empty();
			Path1 << scfilename.BeforeLast(SLASH);
			Name << scfilename.AfterLast(SLASH);
			Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
			MakeDirs(Path1,Path2);
			scfilename.Empty();
			scfilename << Path1 << SLASH << Path2 << SLASH << Name;
		}
	}

	// Scene Data
	LWScene Scene(scfilename.AfterLast(SLASH),scfilename.BeforeLast(SLASH));

	// Object Data
	LWObject Object = GatherM2forLWO(att,m,init,fn,Scene);
	if (Object.SourceType == wxEmptyString){
		wxMessageBox(wxT("Error gathering information for export."),wxT("Export Error"));
		wxLogMessage(wxT("Failure gathering information for export."));
		return;
	}
	wxLogMessage(wxT("Sending M2 Data to LWO Writing Function..."));
	if (WriteLWObject(filename, Object) == false){
		wxString msg = wxT("Error Writing the M2 file to a Lightwave Object.");
		wxMessageBox(msg,wxT("Writing Error"));
		wxLogMessage(msg);
	}else{
		wxLogMessage(wxT("LWO Object \"%s\" Writing Complete."),filename.c_str());
	}
	Object.~LWObject();

	// Scene Data
	WriteLWScene(&Scene);

	// Old Version
	//ExportM2toScene(att,m,fn,init);
}

// Export WMOs
void ExportLWO_WMO(WMO *m, const char *fn){
	wxString filename(fn, wxConvUTF8);
	wxString scfilename = wxString(fn, wxConvUTF8).BeforeLast('.').Append(wxT(".lws"));
	LW_ObjCount.Reset();
	LW_LightCount.Reset();
	LW_BoneCount.Reset();
	LW_CamCount.Reset();

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		// Object
		Path << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Objects"));
		filename.Empty();
		filename << Path << SLASH << wxT("Objects") << SLASH << Name;

		// Scene
		Path.Empty();
		Name.Empty();
		Path << scfilename.BeforeLast(SLASH);
		Name << scfilename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Scenes"));
		filename.Empty();
		filename << Path << SLASH << wxT("Scenes") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;

		// Objects
		Path1 << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		MakeDirs(Path1,Path2);
		filename.Empty();
		filename << Path1 << SLASH << Path2 << SLASH << Name;

		// Scene
		Path1.Empty();
		Path2.Empty();
		Name.Empty();
		Path1 << scfilename.BeforeLast(SLASH);
		Name << scfilename.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		MakeDirs(Path1,Path2);
		scfilename.Empty();
		scfilename << Path1 << SLASH << Path2 << SLASH << Name;
	}

	// Scene Data
	LWScene Scene(scfilename.AfterLast(SLASH),scfilename.BeforeLast(SLASH));

	// Object Data
	LWObject Object = GatherWMOforLWO(m,fn,Scene);
	if (Object.SourceType == wxEmptyString){
		wxMessageBox(wxT("Error gathering information for export."),wxT("Export Error"));
		wxLogMessage(wxT("Failure gathering information for export."));
		return;
	}

	// Write LWO File
	wxLogMessage(wxT("Sending WMO Data to LWO Writing Function..."));
	if (WriteLWObject(filename, Object) == false){
		wxString msg = wxT("Error Writing the WMO file to a Lightwave Object.");
		wxMessageBox(msg,wxT("Writing Error"));
		wxLogMessage(msg);
	}else{
		wxLogMessage(wxT("LWO Object Writing Complete."));

		// Write Scene
	}
	Object.~LWObject();
	Scene.~LWScene();

	// Scene Data

	// Export Lights & Doodads
	if ((modelExport_LW_ExportDoodads == true)||(modelExport_LW_ExportLights == true)){
		ExportWMOtoScene(m,fn);
	}
}

// ADT Exporter
void ExportLWO_ADT(MapTile *m, const char *fn){
	wxString filename(fn, wxConvUTF8);
	wxString scfilename = wxString(fn, wxConvUTF8).BeforeLast('.').Append(wxT(".lws"));
	LW_ObjCount.Reset();
	LW_LightCount.Reset();
	LW_BoneCount.Reset();
	LW_CamCount.Reset();

	if (modelExport_LW_PreserveDir == true){
		wxString Path, Name;

		// Object
		Path << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Objects"));
		filename.Empty();
		filename << Path << SLASH << wxT("Objects") << SLASH << Name;

		// Scene
		Path.Empty();
		Name.Empty();
		Path << scfilename.BeforeLast(SLASH);
		Name << scfilename.AfterLast(SLASH);
		MakeDirs(Path,wxT("Scenes"));
		scfilename.Empty();
		scfilename << Path << SLASH << wxT("Scenes") << SLASH << Name;
	}
	if (modelExport_PreserveDir == true){
		wxString Path1, Path2, Name;

		// Objects
		Path1 << filename.BeforeLast(SLASH);
		Name << filename.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		MakeDirs(Path1,Path2);
		filename.Empty();
		filename << Path1 << SLASH << Path2 << SLASH << Name;

		// Scene
		Path1.Empty();
		Path2.Empty();
		Name.Empty();
		Path1 << scfilename.BeforeLast(SLASH);
		Name << scfilename.AfterLast(SLASH);
		Path2 << wxString(m->name.c_str(), wxConvUTF8).BeforeLast(SLASH);
		MakeDirs(Path1,Path2);
		scfilename.Empty();
		scfilename << Path1 << SLASH << Path2 << SLASH << Name;
	}

	// Scene Data
	LWScene Scene(scfilename.AfterLast(SLASH),scfilename.BeforeLast(SLASH));

	// Object Data
	LWObject Object = GatherADTforLWO(m,fn,Scene);
	if (Object.SourceType == wxEmptyString){
		wxMessageBox(wxT("Error gathering information for export."),wxT("Export Error"));
		wxLogMessage(wxT("Failure gathering information for export."));
		return;
	}

	// Write LWO File
	wxLogMessage(wxT("Sending ADT Data to LWO Writing Function..."));
	if (WriteLWObject(filename, Object) == false){
		wxString msg = wxT("Error Writing the ADT file to a Lightwave Object.");
		wxMessageBox(msg,wxT("Writing Error"));
		wxLogMessage(msg);
	}else{
		wxLogMessage(wxT("LWO Object Writing Complete."));

		// Write Scene
	}
	Object.~LWObject();
	Scene.~LWScene();
}
