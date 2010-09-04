// Copied from Settings.cpp

#include "globalvars.h"
#include "modelexportoptions.h"
#include "exporters.h"
#include "enums.h"

//#include "globalvars.h"


// All IDs & Vars should follow the naming structure similar to "ExportOptions_(3D Format)_(Option name)"

IMPLEMENT_CLASS(ModelExportOptions_Control, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_General, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_Lightwave, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_X3D, wxWindow)
IMPLEMENT_CLASS(ModelExportOptions_M3, wxWindow)

BEGIN_EVENT_TABLE(ModelExportOptions_General, wxWindow)
	EVT_COMBOBOX(ID_EXPORTOPTIONS_PERFERED_EXPORTER,ModelExportOptions_General::OnComboBox)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_PRESERVE_DIR, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_USE_WMV_POSROT, ModelExportOptions_General::OnCheck)
	/*
	EVT_BUTTON(ID_SETTINGS_UP, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_DOWN, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_ADD, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_REMOVE, ModelExportOptions_General::OnButton)
	EVT_BUTTON(ID_SETTINGS_CLEAR, ModelExportOptions_General::OnButton)
	EVT_CHECKBOX(ID_SETTINGS_RANDOMSKIN, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_HIDEHELMET, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_SHOWPARTICLE, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_ZEROPARTICLE, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_LOCALFILES, ModelExportOptions_General::OnCheck)
	EVT_CHECKBOX(ID_SETTINGS_KNIGHTEYEGLOW, ModelExportOptions_General::OnCheck)
	*/
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ModelExportOptions_Lightwave, wxWindow)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_LW_PRESERVE_DIR, ModelExportOptions_Lightwave::OnCheck)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_LW_EXPORTLIGHTS, ModelExportOptions_Lightwave::OnCheck)
	EVT_CHECKBOX(ID_EXPORTOPTIONS_LW_EXPORTDOODADS, ModelExportOptions_Lightwave::OnCheck)
	EVT_COMBOBOX(ID_EXPORTOPTIONS_LW_DOODADSAS, ModelExportOptions_Lightwave::OnComboBox)
	
	// EVT_BUTTON(ID_SETTINGS_APPLY, ModelExportOptions_Lightwave::OnButton)
END_EVENT_TABLE()


BEGIN_EVENT_TABLE(ModelExportOptions_Control, wxWindow)
	//EVT_CLOSE(ModelExportOptions_Control::OnClose)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ModelExportOptions_X3D, wxWindow)
    EVT_CHECKBOX(ID_EXPORTOPTIONS_X3D_EXPORT_ANIMATION, ModelExportOptions_X3D::OnCheck)
    EVT_CHECKBOX(ID_EXPORTOPTIONS_X3D_CENTER_MODEL, ModelExportOptions_X3D::OnCheck)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(ModelExportOptions_M3, wxWindow)
	EVT_BUTTON(ID_EXPORTOPTIONS_M3_APPLY, ModelExportOptions_M3::OnButton)
	EVT_BUTTON(ID_EXPORTOPTIONS_M3_RESET, ModelExportOptions_M3::OnButton)
	EVT_BUTTON(ID_EXPORTOPTIONS_M3_RENAME, ModelExportOptions_M3::OnButton)
	EVT_LISTBOX(ID_EXPORTOPTIONS_M3_ANIMS, ModelExportOptions_M3::OnChoice)
END_EVENT_TABLE()


// --== Shared Options & Setup ==--

ModelExportOptions_General::ModelExportOptions_General(wxWindow* parent, wxWindowID id)
{
	if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_General")) == false) {
		wxLogMessage(_T("GUI Error: ModelExportOptions_General"));
		return;
	}
	wxFlexGridSizer *top = new wxFlexGridSizer(1);

	text = new wxStaticText(this, wxID_ANY, _T("Perferred Exporter:"), wxPoint(5,9), wxDefaultSize, 0);
	top->Add(ddextype = new wxComboBox(this, ID_EXPORTOPTIONS_PERFERED_EXPORTER, _T("Perferred Exporter"), wxPoint(115,5), wxDefaultSize, 0, 0, wxCB_READONLY), 1, wxEXPAND, 10);
	chkbox[MEO_CHECK_PRESERVE_DIR] = new wxCheckBox(this, ID_EXPORTOPTIONS_PRESERVE_DIR, _T("Preserve Directory Structure"), wxPoint(5,30), wxDefaultSize, 0);
	chkbox[MEO_CHECK_USE_WMV_POSROT] = new wxCheckBox(this, ID_EXPORTOPTIONS_USE_WMV_POSROT, _T("Use Position and Rotation from WMV"), wxPoint(5,50), wxDefaultSize, 0);
}


void ModelExportOptions_General::OnButton(wxCommandEvent &event)
{
	//int id = event.GetId();
}

void ModelExportOptions_General::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_PRESERVE_DIR){
		modelExport_PreserveDir = event.IsChecked();
	}else if (id==ID_EXPORTOPTIONS_USE_WMV_POSROT){
		modelExport_UseWMVPosRot = event.IsChecked();
	}
}

void ModelExportOptions_General::OnComboBox(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_PERFERED_EXPORTER){
		Perfered_Exporter = (ddextype->GetCurrentSelection()-1);

		g_modelViewer->InitMenu();
		g_fileControl->UpdateInterface();
	}
}

void ModelExportOptions_General::Update()
{
	ddextype->Clear();
	ddextype->Append(wxString(wxT("None")));
	for (int x=0;x<ExporterTypeCount;x++){
		ddextype->Append(Exporter_Types[x].Name);
	}
	ddextype->SetSelection(Perfered_Exporter+1);

	//Perfered_Exporter
	chkbox[MEO_CHECK_PRESERVE_DIR]->SetValue(modelExport_PreserveDir);
	chkbox[MEO_CHECK_USE_WMV_POSROT]->SetValue(modelExport_UseWMVPosRot);
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
    page3 = new ModelExportOptions_X3D(notebook, ID_EXPORTOPTIONS_PAGE_X3D);
	page4 = new ModelExportOptions_M3(notebook, ID_EXPORTOPTIONS_PAGE_M3);

	notebook->AddPage(page1, _T("General"), false, -1);
	notebook->AddPage(page2, _T("Lightwave"), false);
    notebook->AddPage(page3, _T("X3D and XHTML"), false);
	notebook->AddPage(page4, _T("M3"), false);
}

ModelExportOptions_Control::~ModelExportOptions_Control()
{
	//page1->Destroy();
	//page2->Destroy();
    //page3->Destroy();
	//page4->Destroy();
	//notebook->Destroy();
}

void ModelExportOptions_Control::Open()
{
	Show(true);

	page1->Update();
	page2->Update();
    page3->Update();
	page4->Update();
}

void ModelExportOptions_Control::OnClose(wxCloseEvent &event)
{

}



// --== Individual Options ==--


// -= Lightwave Options =-

ModelExportOptions_Lightwave::ModelExportOptions_Lightwave(wxWindow* parent, wxWindowID id)
{
	if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_Lightwave")) == false) {
		wxLogMessage(_T("GUI Error: ModelExportOptions_Lightwave"));
		return;
	}
	wxFlexGridSizer *top = new wxFlexGridSizer(1);

	chkbox[MEO_CHECK_PRESERVE_LWDIR] = new wxCheckBox(this, ID_EXPORTOPTIONS_LW_PRESERVE_DIR, _("Build Content Directories"), wxPoint(5,5), wxDefaultSize, 0);

	chkbox[MEO_CHECK_LW_EXPORTLIGHTS] = new wxCheckBox(this, ID_EXPORTOPTIONS_LW_EXPORTLIGHTS, _("Export Lights"), wxPoint(5,35), wxDefaultSize, 0);
	chkbox[MEO_CHECK_LW_EXPORTDOODADS] = new wxCheckBox(this, ID_EXPORTOPTIONS_LW_EXPORTDOODADS, _("Export Doodads"), wxPoint(160,35), wxDefaultSize, 0);
	top->Add(ddextype = new wxComboBox(this, ID_EXPORTOPTIONS_LW_DOODADSAS, _T("Doodads As"), wxPoint(160,50), wxSize(180, 25), 0, 0, wxCB_READONLY), 1, wxEXPAND, 10);
}

void ModelExportOptions_Lightwave::Update()
{
	chkbox[MEO_CHECK_PRESERVE_LWDIR]->SetValue(modelExport_LW_PreserveDir);
	chkbox[MEO_CHECK_LW_EXPORTLIGHTS]->SetValue(modelExport_LW_ExportLights);
	chkbox[MEO_CHECK_LW_EXPORTDOODADS]->SetValue(modelExport_LW_ExportDoodads);

	ddextype->Clear();

	ddextype->Append(wxString(wxT("as Nulls")));
	// Uncomment as we're able to do it!
	ddextype->Append(wxString(wxT("as Objects")));
	//ddextype->Append(wxString("as a Seperate Layer"));
	//ddextype->Append(wxString("as a Single Object"));
	//ddextype->Append(wxString("as a Single Object, Per Group"));
	ddextype->SetSelection(modelExport_LW_DoodadsAs);
	ddextype->Enable(modelExport_LW_ExportDoodads);

}

void ModelExportOptions_Lightwave::OnButton(wxCommandEvent &event)
{
	//int id = event.GetId();
	
}

void ModelExportOptions_Lightwave::OnCheck(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_LW_PRESERVE_DIR){
		modelExport_LW_PreserveDir = event.IsChecked();
	}else if (id==ID_EXPORTOPTIONS_LW_EXPORTLIGHTS){
		modelExport_LW_ExportLights = event.IsChecked();
	}else if (id==ID_EXPORTOPTIONS_LW_EXPORTDOODADS){
		modelExport_LW_ExportDoodads = event.IsChecked();
		ddextype->Enable(event.IsChecked());
	}
}

void ModelExportOptions_Lightwave::OnComboBox(wxCommandEvent &event)
{
	int id = event.GetId();

	if (id==ID_EXPORTOPTIONS_LW_DOODADSAS){
		modelExport_LW_DoodadsAs = ddextype->GetCurrentSelection();
	}
}



// -= X3D Options =-

ModelExportOptions_X3D::ModelExportOptions_X3D(wxWindow* parent, wxWindowID id)
{
    if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_X3D")) == false) {
        wxLogMessage(_T("GUI Error: ModelExportOptions_X3D"));
        return;
    }
    top = new wxFlexGridSizer(1);

    chkbox[MEO_CHECK_EXPORT_ANIMATION] = new wxCheckBox(this, ID_EXPORTOPTIONS_X3D_EXPORT_ANIMATION, _("Export keyframe animation"), wxPoint(5,5), wxDefaultSize, 0);
    chkbox[MEO_CHECK_CENTER_MODEL] = new wxCheckBox(this, ID_EXPORTOPTIONS_X3D_CENTER_MODEL, _("Add Transform to center model"), wxPoint(160,5), wxDefaultSize, 0);
    
    // disabled for now
    chkbox[MEO_CHECK_EXPORT_ANIMATION]->Enable(false);
}

void ModelExportOptions_X3D::Update()
{
    chkbox[MEO_CHECK_EXPORT_ANIMATION]->SetValue(modelExport_X3D_ExportAnimation);
    chkbox[MEO_CHECK_CENTER_MODEL]->SetValue(modelExport_X3D_CenterModel);
}

void ModelExportOptions_X3D::OnCheck(wxCommandEvent &event)
{
    int id = event.GetId();

    if (id==ID_EXPORTOPTIONS_X3D_EXPORT_ANIMATION){
        modelExport_X3D_ExportAnimation = event.IsChecked();
    }else if (id==ID_EXPORTOPTIONS_X3D_CENTER_MODEL){
        modelExport_X3D_CenterModel = event.IsChecked();
    }
}

// -= M3 Options =-

ModelExportOptions_M3::ModelExportOptions_M3(wxWindow* parent, wxWindowID id)
{
    if (Create(parent, id, wxPoint(0,0), wxSize(400,400), 0, _T("ModelExportOptions_M3")) == false) {
        wxLogMessage(_T("GUI Error: ModelExportOptions_M3"));
        return;
    }

	stBoundScale = new wxStaticText(this, wxID_ANY, _T("Bound Scale"), wxPoint(5, 8));
	tcBoundScale = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(100, 5));
	stSphereScale = new wxStaticText(this, wxID_ANY, _T("Sphere Scale"), wxPoint(5, 33));
	tcSphereScale = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(100, 30));
	stTexturePath = new wxStaticText(this, wxID_ANY, _T("Texture Path"), wxPoint(5, 58));
	tcTexturePath = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(100, 55), wxSize(250, 25));
	clbAnimations = new wxCheckListBox(this, ID_EXPORTOPTIONS_M3_ANIMS, wxPoint(5, 85), wxSize(250,165), 0, NULL, 0, wxDefaultValidator, _T("Animations"));
	stRename = new wxStaticText(this, wxID_ANY, _T("Rename"), wxPoint(5, 255));
	tcRename = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(100, 255), wxSize(250, 25));
	bApply = new wxButton(this, ID_EXPORTOPTIONS_M3_APPLY, _T("Apply"), wxPoint(5, 285));
	bReset = new wxButton(this, ID_EXPORTOPTIONS_M3_RESET, _T("Reset"), wxPoint(95, 285));
	bRename = new wxButton(this, ID_EXPORTOPTIONS_M3_RENAME, _T("Rename"), wxPoint(185, 285));
}

void ModelExportOptions_M3::Update()
{
	tcBoundScale->SetValue(wxString::Format(_T("%0.2f"), modelExport_M3_BoundScale));
	tcSphereScale->SetValue(wxString::Format(_T("%0.2f"), modelExport_M3_SphereScale));
	tcTexturePath->SetValue(modelExport_M3_TexturePath);
	clbAnimations->Clear();
	asAnims.Clear();
	if (g_selModel && g_selModel->animated && g_selModel->anims) {
		Model *m = g_selModel;
		
		for(uint32 i=0; i<m->header.nAnimations; i++) {
			wxString strName;
			try {
				AnimDB::Record rec = animdb.getByAnimID(m->anims[i].animID);
				strName = rec.getString(AnimDB::Name);
			} catch (AnimDB::NotFound) {
				strName = _T("???");
			}
			asAnims.push_back(strName);

			strName += wxString::Format(_T(" [%i]"), i);
			clbAnimations->Append(strName);

			// set default actions
			if (modelExport_M3_Anims.size() == 0) { 
				if (m->anims[i].animID == 0 || m->anims[i].animID == 1 || m->anims[i].animID == 5) {
					clbAnimations->Check(clbAnimations->GetCount()-1);
				}
			}
		}
		for(uint32 i=0; i<modelExport_M3_Anims.size(); i++) {
			uint32 j = modelExport_M3_Anims[i];
			if (j >= clbAnimations->GetCount()) // error check
				continue;
			clbAnimations->Check(j);
			asAnims[j] = modelExport_M3_AnimNames[i];
			clbAnimations->SetString(j, clbAnimations->GetString(j)+_T("  :")+asAnims[j]);
		}
	}
}

void ModelExportOptions_M3::OnChoice(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_EXPORTOPTIONS_M3_ANIMS) {
		int i = clbAnimations->GetSelection();
		tcRename->SetValue(asAnims[i]);
	}
}

void ModelExportOptions_M3::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_EXPORTOPTIONS_M3_APPLY) {
		modelExport_M3_BoundScale = wxAtof(tcBoundScale->GetValue());
		modelExport_M3_SphereScale = wxAtof(tcSphereScale->GetValue());
		modelExport_M3_TexturePath = tcTexturePath->GetValue();
		modelExport_M3_Anims.clear();
		modelExport_M3_AnimNames.clear();
		for(uint32 i=0; i<clbAnimations->GetCount(); i++) {
			if (!clbAnimations->IsChecked(i))
				continue;
			modelExport_M3_Anims.push_back(i);
			modelExport_M3_AnimNames.push_back(asAnims[i]);
		}
	} else if (id == ID_EXPORTOPTIONS_M3_RESET) {
		//modelExport_M3_BoundScale = 0.5f;
		//modelExport_M3_SphereScale = 0.5f;
		//modelExport_M3_TexturePath = _T("");
		modelExport_M3_Anims.clear();
		modelExport_M3_AnimNames.clear();
		Update();
	} else if (id == ID_EXPORTOPTIONS_M3_RENAME) {
		int i = clbAnimations->GetSelection();
		if (i > -1) {
			asAnims[i] = tcRename->GetValue();
			wxString strName = clbAnimations->GetString(i).BeforeFirst(']');
			strName.Append(_T("]"));
			clbAnimations->SetString(i, strName+_T("  :")+asAnims[i]);
		}
	}
}