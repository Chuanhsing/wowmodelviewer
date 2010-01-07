// Copied from Settings.cpp

#include "modelexportoptions.h"
#include "enums.h"
//#include "util.h"

//#include "globalvars.h"

IMPLEMENT_CLASS(ModelExportOptions_General, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_Lightwave, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_Control, wxWindow)

BEGIN_EVENT_TABLE(ModelExportOptions_General, wxWindow)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_PRESERVE_DIR, ModelExportOptions_General::OnCheck)
	/*
	EVT_BUTTON(ID_SETTINGS_UP, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_DOWN, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_ADD, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_REMOVE, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_CLEAR, ModelExportOptions_General::OnButton)
	EVT_CHECKBOX(ID_SETTINGS_RANDOMSKIN, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_HIDEHELMET, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_SHOWPARTICLE, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_LOCALFILES, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_KNIGHTEYEGLOW, ModelExportOptions_General::OnCheck)
	*/
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ModelExportOptions_Lightwave, wxWindow)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_PRESERVE_LWDIR, ModelExportOptions_Lightwave::OnCheck)
	
	// EVT_BUTTON(ID_SETTINGS_APPLY, ModelExportOptions_Lightwave::OnButton)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ModelExportOptions_Control, wxWindow)
	
END_EVENT_TABLE()


ModelExportOptions_General::ModelExportOptions_General(wxWindow* parent, wxWindowID id)
{
	if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _("ModelExportOptions_General")) == false) {
		wxLogMessage(_T("GUI Error: ModelExportOptions_General"));
		return;
	}

	chkbox[MEO_CHECK_PRESERVE_DIR] = new wxCheckBox(this, ID_EXPORTOPTIONS_PRESERVE_DIR, _("Preserve Directory Structure"), wxPoint(5,5), wxDefaultSize, 0);
}


void ModelExportOptions_General::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();
}

void ModelExportOptions_General::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_PRESERVE_DIR){
		modelExport_PreserveDir = event.IsChecked();
	}
}

void ModelExportOptions_General::Update()
{
	chkbox[MEO_CHECK_PRESERVE_DIR]->SetValue(modelExport_PreserveDir);
}

ModelExportOptions_Lightwave::ModelExportOptions_Lightwave(wxWindow* parent, wxWindowID id)
{
	if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_Lightwave")) == false) {
		wxLogMessage(_T("GUI Error: ModelExportOptions_Lightwave"));
		return;
	}

	chkbox[MEO_CHECK_PRESERVE_LWDIR] = new wxCheckBox(this, ID_EXPORTOPTIONS_PRESERVE_LWDIR, _("Build Content Directories"), wxPoint(5,5), wxDefaultSize, 0);
}

void ModelExportOptions_Lightwave::Update()
{
	chkbox[MEO_CHECK_PRESERVE_LWDIR]->SetValue(modelExport_PreserveLWDir);
}

void ModelExportOptions_Lightwave::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();
	
}

void ModelExportOptions_Lightwave::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_PRESERVE_LWDIR){
		modelExport_PreserveLWDir = event.IsChecked();
	}
}

ModelExportOptions_Control::ModelExportOptions_Control(wxWindow* parent, wxWindowID id)
{
	wxLogMessage(_T("Creating Model Export Options Control..."));
	
	if (Create(parent, id, wxDefaultPosition, wxSize(405,440), wxDEFAULT_FRAME_STYLE, _T("ModelExportOptions_ControlFrame")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create the window for our ModelExportOptions_Control!"));
		return;
	}

	notebook = new wxNotebook(this, ID_EXPORTOPTIONS_TABS, wxPoint(0,0), wxSize(400,420), wxNB_TOP|wxNB_FIXEDWIDTH|wxNB_NOPAGETHEME);
	
	page1 = new ModelExportOptions_General(notebook, ID_EXPORTOPTIONS_PAGE_GENERAL);
	page2 = new ModelExportOptions_Lightwave(notebook, ID_EXPORTOPTIONS_PAGE_LIGHTWAVE);

	notebook->AddPage(page1, _T("General"), false, -1);
	notebook->AddPage(page2, _T("Lightwave"), false);
}


ModelExportOptions_Control::~ModelExportOptions_Control()
{
	page1->Destroy();
	page2->Destroy();
	notebook->Destroy();
}


void ModelExportOptions_Control::Open()
{
	Show(true);

	page1->Update();
	page2->Update();
}

void ModelExportOptions_Control::Close()
{
	
}

// --
