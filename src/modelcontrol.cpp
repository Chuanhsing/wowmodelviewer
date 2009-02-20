
#include "modelcontrol.h"

IMPLEMENT_CLASS(ModelControl, wxWindow)

BEGIN_EVENT_TABLE(ModelControl, wxWindow)
	EVT_CHECKLISTBOX(ID_MODEL_GEOSETS, ModelControl::OnList)

	EVT_COMBOBOX(ID_MODEL_NAME, ModelControl::OnCombo)
	EVT_COMBOBOX(ID_MODEL_LOD, ModelControl::OnCombo)

	EVT_CHECKBOX(ID_MODEL_BONES, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_BOUNDS, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_RENDER, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_WIREFRAME, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_PARTICLES, ModelControl::OnCheck)
	EVT_CHECKBOX(ID_MODEL_TEXTURE, ModelControl::OnCheck)

	EVT_COMMAND_SCROLL(ID_MODEL_ALPHA, ModelControl::OnSlider)
	EVT_COMMAND_SCROLL(ID_MODEL_SCALE, ModelControl::OnSlider)
END_EVENT_TABLE()


// ModelName
// LevelOfDetail
// Opacity
// Bones
// Bounding Box
// Render
// Geosets
// Future Additions:
//		- Pos
//		- Rotation
//		- Scale
//		- Attach model

ModelControl::ModelControl(wxWindow* parent, wxWindowID id)
{
	model = NULL;
	att = NULL;

	wxLogMessage(_T("Creating Model Control..."));

	if (Create(parent, id, wxDefaultPosition, wxSize(160,460), 0, _T("ModelControlFrame")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create a window for our ModelControl!"));
		return;
	}

	try {
		modelname = new wxComboBox(this, ID_MODEL_NAME, _T(""), wxPoint(5,5), wxSize(150,16), 0, NULL, wxCB_READONLY);
		
		lblLod = new wxStaticText(this, wxID_ANY, _("View"), wxPoint(5,25), wxDefaultSize);
		lod = new wxComboBox(this, ID_MODEL_LOD, _T(""), wxPoint(5,40), wxSize(120,16), 0, NULL, wxCB_READONLY, wxDefaultValidator, _T("LOD")); //|wxCB_SORT); //wxPoint(66,10)
		lod->Enable(false);

		lblAlpha = new wxStaticText(this, wxID_ANY, _("Alpha"), wxPoint(5,65), wxDefaultSize);
		alpha = new wxSlider(this, ID_MODEL_ALPHA, 100, 0, 100, wxPoint(45, 65), wxSize(110, 30), wxSL_HORIZONTAL);
		
		lblScale = new wxStaticText(this, wxID_ANY, _("Scale"), wxPoint(5,90), wxDefaultSize);
		scale = new wxSlider(this, ID_MODEL_SCALE, 100, 10, 300, wxPoint(45, 90), wxSize(110, 30), wxSL_HORIZONTAL);

		bones = new wxCheckBox(this, ID_MODEL_BONES, _("Bones"), wxPoint(5, 115), wxDefaultSize);
		box = new wxCheckBox(this, ID_MODEL_BOUNDS, _("Bounds"), wxPoint(5, 135), wxDefaultSize);
		render = new wxCheckBox(this, ID_MODEL_RENDER, _("Render"), wxPoint(5, 155), wxDefaultSize);
		wireframe = new wxCheckBox(this, ID_MODEL_WIREFRAME, _("Wireframe"), wxPoint(5, 175), wxDefaultSize);
		texture = new wxCheckBox(this, ID_MODEL_TEXTURE, _("Texture"), wxPoint(5, 195), wxDefaultSize);
		particles = new wxCheckBox(this, ID_MODEL_PARTICLES, _("Particles"), wxPoint(5, 215), wxDefaultSize);

		lblGeosets = new wxStaticText(this, wxID_ANY, _("Show Geosets"), wxPoint(5,235), wxDefaultSize);
		geosets = new wxCheckListBox(this, ID_MODEL_GEOSETS, wxPoint(5, 250), wxSize(150,120), 0, NULL, 0, wxDefaultValidator, _T("GeosetsList"));
		
		wxStaticText *lblXYZ = new wxStaticText(this, wxID_ANY, _T("X\nY\nZ"), wxPoint(2,380), wxSize(20,60));
		txtX = new wxTextCtrl(this, ID_MODEL_X, _T("0.0"), wxPoint(25,380), wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator);
		txtY = new wxTextCtrl(this, ID_MODEL_Y, _T("0.0"), wxPoint(25,400), wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator);
		txtZ = new wxTextCtrl(this, ID_MODEL_Z, _T("0.0"), wxPoint(25,420), wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator);

	} catch(...) {};
}

ModelControl::~ModelControl()
{
	modelname->Destroy();
	lod->Destroy();
	alpha->Destroy();
	scale->Destroy();
	bones->Destroy();
	box->Destroy();
	render->Destroy();
	wireframe->Destroy();
	texture->Destroy();
	particles->Destroy();
	geosets->Destroy();
	txtX->Destroy();
	txtY->Destroy();
	txtZ->Destroy();
}

// Iterates through all the models counting and creating a list
void ModelControl::RefreshModel(Attachment *root)
{
	try {
		attachments.clear();

		Model *m = static_cast<Model*>(root->model);
		if (m) {
			wxASSERT(m);
			attachments.push_back(root);
			if (!init)
				UpdateModel(root);
		}
		
		for (std::vector<Attachment *>::iterator it=root->children.begin(); it!=root->children.end(); ++it) {
			//m = NULL;
			m = static_cast<Model*>((*it)->model);
			if (m) {
				attachments.push_back((*it));
				if (!init)
					UpdateModel((*it));
			}

			for (std::vector<Attachment *>::iterator it2=(*it)->children.begin(); it2!=(*it)->children.end(); ++it2) {
				m = static_cast<Model*>((*it2)->model);
				if (m) {
					//models.push_back(m);
					attachments.push_back((*it2));
					if (!init)
						UpdateModel((*it2));
				}

				for (std::vector<Attachment *>::iterator it3=(*it2)->children.begin(); it3!=(*it2)->children.end(); ++it3) {
					m = static_cast<Model*>((*it3)->model);
					if (m) {
						//models.push_back(m);
						attachments.push_back((*it3));
						if (!init)
							UpdateModel((*it3));
					}
				}
			}
		}
		
		// update combo box with the list of models?
		wxString tmp;
		modelname->Clear();
		for (std::vector<Attachment*>::iterator it=attachments.begin(); it!=attachments.end(); ++it) {
			m = static_cast<Model*>((*it)->model);
			if (m) {
				tmp = m->name.c_str();
				modelname->Append(tmp.AfterLast('\\'));
			}
		}

		if (modelname->GetCount() > 0)
			modelname->SetSelection(0);

	} catch( ... ) {
		wxLogMessage(_T("Error: Problem occured in ModelControl::RefreshModel(Attachment *)"));
	}

}

void ModelControl::UpdateModel(Attachment *a)
{
	if (!a)
		return;

	init = false;

	Model *m = NULL;
	if (a->model)
		m = static_cast<Model*>(a->model);

	if (m) {
		init = true;
		model = m;
		att = a;

		modelname->SetLabel(m->name.c_str());

		Update();
	}
}

void ModelControl::Update()
{
	if (!model)
		return;

	/*
	// Loop through all the views.
	lod->Clear();
	if (model->header.nViews == 1) {
		lod->Append(_T("1 (Only View)"));
	} else if (model->header.nViews == 2) {
		lod->Append(_T("1 (Worst)"));
		lod->Append(_T("2 (Best)"));
	} else {
		lod->Append(_T("1 (Worst)"));
		for (unsigned int i=1; i<(model->header.nViews-1); i++) {
			lod->Append(wxString::Format("%i", i+1));
		}
		lod->Append(wxString::Format("%i (Best)", model->header.nViews));
	}
	lod->SetSelection(lod->GetCount() - 1);
	*/

	// Loop through all the geosets.
	wxArrayString geosetItems;
	//geosets->Clear();
	for (unsigned int i=0; i<model->geosets.size(); i++) {
		geosetItems.Add(wxString::Format("%i", i), 1);
	}
	//geosets->InsertItems(items, 0);
	geosets->Set(geosetItems, 0);

	for (unsigned int i=0; i<model->geosets.size(); i++) {
		geosets->Check(i, model->showGeosets[i]);
	}

	bones->SetValue(model->showBones);
	box->SetValue(model->showBounds);
	render->SetValue(model->showModel);
	wireframe->SetValue(model->showWireframe);
	particles->SetValue(model->showParticles);
	texture->SetValue(model->showTexture);

	alpha->SetValue(int(model->alpha * 100));
	scale->SetValue(100);

	txtX->SetValue(wxString::Format("%f", model->pos.x));
	txtY->SetValue(wxString::Format("%f", model->pos.y));
	txtZ->SetValue(wxString::Format("%f", model->pos.z));
}

void ModelControl::OnCheck(wxCommandEvent &event)
{
	if (!init || !model)
		return;

	int id = event.GetId();

	if (id == ID_MODEL_BONES) {
		model->showBones = event.IsChecked();
	} else if (id == ID_MODEL_BOUNDS) {
		model->showBounds = event.IsChecked();
	} else if (id == ID_MODEL_RENDER) {
		model->showModel = event.IsChecked();
	} else if (id == ID_MODEL_WIREFRAME) {
		model->showWireframe = event.IsChecked();
	} else if (id == ID_MODEL_PARTICLES) {
		model->showParticles = event.IsChecked();
	} else if (id == ID_MODEL_TEXTURE) {
		model->showTexture = event.IsChecked();
	}
}

void ModelControl::OnCombo(wxCommandEvent &event)
{
	if (!init)
		return;

	int id = event.GetId();
	
	if (id == ID_MODEL_LOD) {
		int value = event.GetInt();
		
		MPQFile f(model->name.c_str());
		if (f.isEof() || (f.getSize() < sizeof(ModelHeader))) {
			wxLogMessage("ERROR - unable to open MPQFile: [%s]", model->name.c_str());
			f.close();
			return;
		}	

		model->showModel = false;
		model->setLOD(f, value);
		model->showModel = true;

		/*
		for (size_t i=0; i<model->geosets.size(); i++) {
			int id = model->geosets[i].id;
			model->showGeosets[i] = (id==0);
		}

		cc->RefreshModel();
		*/

		f.close();
	} else if (id == ID_MODEL_NAME) {
		// Error check
		if (modelname->GetSelection() < (int)attachments.size()) {
			UpdateModel(attachments[modelname->GetSelection()]);
			att = attachments[modelname->GetSelection()];
			model = static_cast<Model*>(attachments[modelname->GetSelection()]->model);
			
			animControl->UpdateModel(model);
		}
	}
}

void ModelControl::OnList(wxCommandEvent &event)
{
	if (!init || !model)
		return;

	for (unsigned int i=0; i<model->geosets.size(); i++) {
		model->showGeosets[i] = geosets->IsChecked(i);
	}
}

void ModelControl::OnSlider(wxScrollEvent &event)
{
	if (!init || !model)
		return;

	int id = event.GetId();
	if (id == ID_MODEL_ALPHA) {
		model->alpha = event.GetInt() / 100.0f;
	} else if (id == ID_MODEL_SCALE) {
		att->scale = event.GetInt() / 100.0f;
	}
}


//manager->GetPane(this).Show(false);
