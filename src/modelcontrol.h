#ifndef MODELCONTROL_H
#define MODELCONTROL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "model.h"
#include "mpq.h"
#include "modelcanvas.h"
#include "animcontrol.h"

#include "enums.h"

// ModelName
// LevelOfDetail
// Opacity
// Bones
// Bounding Box
// Render
// Wireframe
// Geosets
// Future Additions:
//		- Pos
//		- Rotation
//		- Scale
//		- Attach model

class ModelControl: public wxWindow
{
	DECLARE_CLASS(ModelControl)
	DECLARE_EVENT_TABLE()
	
	wxComboBox *lod, *modelname;
	wxSlider *alpha, *scale;
	wxCheckBox *bones, *box, *render, *wireframe, *texture, *particles;
	wxCheckListBox *geosets;
	wxStaticText *lblGeosets, *lblLod, *lblScale, *lblAlpha;
	wxTextCtrl *txtX, *txtY, *txtZ;

	// List of models in the scene.
	//std::vector<Model*> models;
	std::vector<Attachment*> attachments;

	bool init;
	
public:
	Model *model;	// Currently 'active' model.
	Attachment *att; // Currently 'active' attachment.
	AnimControl *animControl;

	ModelControl(wxWindow* parent, wxWindowID id);
	~ModelControl();

	void UpdateModel(Attachment *a);
	void Update();
	void RefreshModel(Attachment *root);
	void OnCheck(wxCommandEvent &event);
	void OnCombo(wxCommandEvent &event);
	void OnList(wxCommandEvent &event);
	void OnSlider(wxScrollEvent &event);

};


#endif

