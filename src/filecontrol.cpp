#include "modelviewer.h"
#include "globalvars.h"

typedef std::pair<wxTreeItemId, std::string> TreeStackItem;
typedef std::vector<TreeStackItem> TreeStack;

IMPLEMENT_CLASS(FileControl, wxWindow)

BEGIN_EVENT_TABLE(FileControl, wxWindow)
	// model tree
	EVT_TREE_SEL_CHANGED(ID_FILELIST, FileControl::OnTreeSelect)
	EVT_TREE_ITEM_EXPANDED(ID_FILELIST, FileControl::OnTreeCollapsedOrExpanded)
	EVT_TREE_ITEM_COLLAPSED(ID_FILELIST, FileControl::OnTreeCollapsedOrExpanded)
	EVT_BUTTON(ID_FILELIST_SEARCH, FileControl::OnButton)
	EVT_TEXT_ENTER(ID_FILELIST_CONTENT, FileControl::OnButton)
	EVT_CHOICE(ID_FILELIST_FILTER, FileControl::OnChoice)
	EVT_TREE_ITEM_MENU(ID_FILELIST, FileControl::OnTreeMenu)
END_EVENT_TABLE()

class FileTreeData:public wxTreeItemData
{
public:
	std::string fn;
	FileTreeData(std::string fn):fn(fn) {}
};


FileControl::FileControl(wxWindow* parent, wxWindowID id)
{
	modelviewer = NULL;
	filterMode = 0;

	if (Create(parent, id, wxDefaultPosition, wxSize(170,700), 0, _T("ModelControlFrame")) == false) {
		wxLogMessage(_T("GUI Error: Failed to create a window for our FileControl!"));
		return;
	}

	try {
		txtContent = new wxTextCtrl(this, ID_FILELIST_CONTENT, _T(""), wxPoint(10, 10), wxSize(110, 20), wxTE_PROCESS_ENTER, wxDefaultValidator);
		btnSearch = new wxButton(this, ID_FILELIST_SEARCH, _("Clear"), wxPoint(120, 10), wxSize(46,20));
		fileTree = new wxTreeCtrl(this, ID_FILELIST, wxPoint(0, 35), wxSize(250,600), wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxTR_FULL_ROW_HIGHLIGHT|wxTR_NO_LINES);
		wxString chos[] = {_T("Model"), _T("Music"), _T("Graphic")};
		choFilter = new wxChoice(this, ID_FILELIST_FILTER, wxPoint(10, 645), wxSize(110, 10), WXSIZEOF(chos), chos);
		choFilter->SetSelection(filterMode);
	} catch(...) {};
}

FileControl::~FileControl()
{
	if (fileTree) {
		fileTree->Destroy();
		fileTree = NULL;
	}
	txtContent->Destroy();
	btnSearch->Destroy();
	choFilter->Destroy();
}

wxString content;

/*
bool filterModels(std::string s)
{
	//s.LowerCase();
	const size_t len = s.length();
	if (len < 4) 
		return false;

	//return ((s[len-2]=='m' && s[len-1]=='2') || (s[len-3]=='w' && s[len-2]=='m'));
	return ( 
			((s[len-2]|0x20)=='m' && s[len-1]=='2')
			|| ((s[len-3]|0x20)=='w' && (s[len-2]|0x20)=='m' && (s[len-1]|0x20)=='o' )
			//||((s[len-3]|0x20)=='a' && (s[len-2]|0x20)=='d' && (s[len-1]|0x20)=='t' ) 
			);
}

bool filterNpcs(std::string s)
{
	// textures\BakedNpcTextures\*.*
	if (s.length() < 18) 
		return false;
	
	return (s.substr(9, 8) == "BakedNpc");
}


bool filterSounds(std::string s)
{
	const size_t len = s.length();
	if (len < 4) 
		return false;

	return (
			((s[len-3]|0x20)=='w' && (s[len-2]|0x20)=='a' && (s[len-1]|0x20)=='v') ||
			((s[len-3]|0x20)=='m' && (s[len-2]|0x20)=='p' && s[len-1]=='3')
			);
}
*/

bool filterModelsSearch(std::string s)
{
	const size_t len = s.length();
	if (len < 4) 
		return false;

	wxString temp(s.c_str(), wxConvUTF8);
	temp = temp.MakeLower();
	if (!temp.Mid(temp.Length()-2).IsSameAs(wxT("m2"))
		&& !temp.Mid(temp.Length()-3).IsSameAs(wxT("wmo"))
		//&& !temp.Mid(temp.Length()-3).IsSameAs(wxT("adt"))
		)
		return false;
	if (content != _T("") && temp.Find(content) == wxNOT_FOUND)
		return false;

	return true;
}

bool filterSoundsSearch(std::string s)
{
	const size_t len = s.length();
	if (len < 4) 
		return false;

	wxString temp(s.c_str(), wxConvUTF8);
	temp = temp.MakeLower();
	if (!temp.Mid(temp.Length()-3).IsSameAs(wxT("wav"))
		&& !temp.Mid(temp.Length()-3).IsSameAs(wxT("wp3"))
		)
		return false;
	if (content != _T("") && temp.Find(content) == wxNOT_FOUND)
		return false;

	return true;
}

bool filterGraphicsSearch(std::string s)
{
	const size_t len = s.length();
	if (len < 4) 
		return false;

	wxString temp(s.c_str(), wxConvUTF8);
	temp = temp.MakeLower();
	if (!temp.Mid(temp.Length()-3).IsSameAs(wxT("blp"))
		&& !temp.Mid(temp.Length()-3).IsSameAs(wxT("png"))
		)
		return false;
	if (content != _T("") && temp.Find(content) == wxNOT_FOUND)
		return false;

	return true;
}

void FileControl::Init(ModelViewer* mv)
{
	if (modelviewer == NULL)
		modelviewer = mv;

	// Gets the list of files that meet the filter criteria
	// and puts them into an array to be processed into out file tree
	content = txtContent->GetValue();
	content = content.MakeLower();
	content = content.Trim();

	if (filterMode == 0)
		getFileLists(filelist, filterModelsSearch);
	else if (filterMode == 1)
		getFileLists(filelist, filterSoundsSearch);
	else if (filterMode == 2)
		getFileLists(filelist, filterGraphicsSearch);

	// Put all the viewable files into our File Tree.
	TreeStack stack;
	TreeStackItem root;
	wxTreeItemId item;
	fileTree->DeleteAllItems();
	fileTree->AddRoot(_T("Root"));
	root.first = fileTree->GetRootItem();
	root.second = "";
	stack.push_back(root);

	size_t index=0;

	for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it) {
		const std::string &str = (*it).fn;
		size_t p = 0;
		size_t i;

		// Alfred 2009.7.28 skip cameras
		/*wxString tmp(str.c_str(), wxConvUTF8);
		if (tmp.Mid(0, 7).IsSameAs(_T("cameras"), false))
			continue;
		*/

		// find the matching place in the stack
		for (i=1; i<stack.size(); i++) {
			std::string &comp = stack[i].second;
			bool match = true;
			for (unsigned int j=0; j<comp.length(); j++) {
				if (comp[j] != str[p+j]) {
					match = false;
					break;
				}
			}
			if (match) 
				match &= str[p+comp.length()] == '\\';

			if (!match)
				break;
            p += comp.length() + 1;
		}
		// i-1 is the index of the last matching piece in the stack

		// delete the extra parts off the end of the stack
		size_t numtopop = stack.size()-i;
		for (size_t k=0; k<numtopop; k++) {
			stack.pop_back();
		}
		
		// starting at p, append extra folders
		size_t start = p;
		bool colour=false;
		for (; p<str.length(); p++) {
			if (str[p]=='\\') {
				// we've hit a folder, push it onto the stack
				TreeStackItem newItem;
				newItem.second = str.substr(start, p-start);
				start = p+1;
				newItem.first = fileTree->AppendItem(stack[stack.size()-1].first, wxString(newItem.second.c_str(), wxConvUTF8));
				
				//if (colour == true) {
					if ((*it).col == 0)
						fileTree->SetItemTextColour(newItem.first, *wxBLACK);
					else if ((*it).col == 1)
						fileTree->SetItemTextColour(newItem.first, *wxBLUE);
					else if ((*it).col == 2)
						fileTree->SetItemTextColour(newItem.first, *wxRED);
					else if ((*it).col == 3)
						fileTree->SetItemTextColour(newItem.first, wxColour(0,170,0));		// Green
					else if ((*it).col == 4)
						fileTree->SetItemTextColour(newItem.first, wxColour(160,0,160));	// Outland Purple
					else if ((*it).col == 5)
						fileTree->SetItemTextColour(newItem.first, wxColour(35,130,179));	// Frozen Blue
					else if ((*it).col == 6)
						fileTree->SetItemTextColour(newItem.first, wxColour(233,109,17));	// Destruction Orange
					else
						fileTree->SetItemTextColour(newItem.first, *wxLIGHT_GREY);

				//} else {
				//	colour = true;
				//}
				//fileTree->SetItemBackgroundColour(newItem.first, wxColour(237,243,254));
					
				stack.push_back(newItem);
			}
		}
		// now start was at the character after the last \\, so we append a filename
		std::string fileName = str.substr(start);

		item = fileTree->AppendItem(stack[stack.size()-1].first, wxString(fileName.c_str(), *wxConvCurrent), -1, -1, new FileTreeData(str));
		if ((*it).col == 0)
			fileTree->SetItemTextColour(item, *wxBLACK);
		else if ((*it).col == 1)
			fileTree->SetItemTextColour(item, *wxBLUE);
		else if ((*it).col == 2)
			fileTree->SetItemTextColour(item, *wxRED);
		else if ((*it).col == 3)
			fileTree->SetItemTextColour(item, wxColour(0,170,0));		// Green
		else if ((*it).col == 4)
			fileTree->SetItemTextColour(item, wxColour(160,0,160));		// Outland Purple
		else if ((*it).col == 5)
			fileTree->SetItemTextColour(item, wxColour(35,130,179));	// Frozen Blue
		else if ((*it).col == 6)
			fileTree->SetItemTextColour(item, wxColour(233,109,17));	// Destruction Orange
		else
			fileTree->SetItemTextColour(item, *wxLIGHT_GREY);

		index++;

	}

	if (content != _T(""))
		fileTree->ExpandAll();

	// bg recolor
	wxTreeItemId h;
	size_t i = 0;
	for(h=fileTree->GetFirstVisibleItem();h;h=fileTree->GetNextVisible(h)) {
		if (i++%2==1)
			fileTree->SetItemBackgroundColour(h, wxColour(237,243,254));
		else
			fileTree->SetItemBackgroundColour(h, *wxWHITE);
	}

	filelist.clear();
}

void FileControl::OnChoice(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_FILELIST_FILTER) {
		int curSelection = choFilter->GetCurrentSelection();
		if (curSelection >= 0 && curSelection != filterMode) {
			filterMode = curSelection;
			Init();
		}
	}
}

// copy from ModelOpened::Export
void FileControl::Export(wxString val)
{
	if (val == _T(""))
		return;
	MPQFile f(val.mb_str());
	if (f.isEof()) {
		wxLogMessage(_T("Error: Could not extract %s\n"), val.c_str());
		f.close();
		return;
	}
	wxFileName fn(val);

	FILE *hFile = NULL;
	wxString filename = wxFileSelector(wxT("Please select your file to export"), 
		wxGetCwd(), fn.GetName(), fn.GetExt(), fn.GetExt()+_T(" files (.")+fn.GetExt()+")|*."+fn.GetExt());
	if ( !filename.empty() )
	{
		hFile = fopen(filename.mb_str(), "wb");
	}
	if (hFile) {
		fwrite(f.getBuffer(), 1, f.getSize(), hFile);
		fclose(hFile);
	}
	f.close();
}

void FileControl::OnPopupClick(wxCommandEvent &evt)
{
	switch(evt.GetId()) {
		case ID_MODELOPENED_EXPORT:
			FileTreeData *data = (FileTreeData*)(static_cast<wxMenu *>(evt.GetEventObject())->GetClientData());
			wxString rootfn(data->fn.c_str(), wxConvUTF8);
			Export(rootfn);
		break;
	}
}

void FileControl::OnTreeMenu(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();

	if (!item.IsOk() || !modelviewer->canvas) // make sure that a valid Tree Item was actually selected.
		return;

	void *data = reinterpret_cast<void *>(fileTree->GetItemData(item));

	// make sure the data (file name) is valid
	if (!data)
		return; // isn't valid, exit.
	
	// Make a menu to show item Info or export it
	wxMenu infoMenu;
	infoMenu.SetClientData( data );
	infoMenu.Append(ID_MODELOPENED_EXPORT, wxT("&Export"), wxT("Export this object"));
	infoMenu.Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FileControl::OnPopupClick, NULL, this);
	PopupMenu(&infoMenu);
}

void FileControl::OnTreeSelect(wxTreeEvent &event)
{
	wxTreeItemId item = event.GetItem();

	if (!item.IsOk() || !modelviewer->canvas) // make sure that a valid Tree Item was actually selected.
		return;

	FileTreeData *data = (FileTreeData*)fileTree->GetItemData(item);

	// make sure the data (file name) is valid
	if (!data)
		return; // isn't valid, exit.

	if (filterMode == 0) {
		// Exit, if its the same model thats currently loaded
		if (modelviewer->canvas->model && !modelviewer->canvas->model->name.empty() && modelviewer->canvas->model->name == data->fn)
			return; // clicked on the same model thats currently loaded, no need to load it again - exit

		// Delete any previous models that were loaded.
		if (modelviewer->isWMO) {
			//canvas->clearAttachments();
			wxDELETE(modelviewer->canvas->wmo);
			modelviewer->canvas->wmo = NULL;
		} else {
			modelviewer->canvas->clearAttachments();

			// If it was a character model, no need to delete canvas->model, 
			//it was just pointing to a model created as an attachment - just set back to NULL instead.
			//canvas->model = NULL;
			
			if (!modelviewer->isChar) { 
				//wxDELETE(modelviewer->canvas->model); // may memory leak
				modelviewer->canvas->model = NULL;
			} else{
				modelviewer->charControl->charAtt = NULL;

				wxString rootfn(data->fn.c_str(), wxConvUTF8);
				if (rootfn.Last() != '2' && modelviewer->canvas->model) {
					modelviewer->canvas->model = NULL;
				}
			}
		}

#ifdef _DEBUG
		GLenum err=glGetError();
		if (err)
			wxLogMessage(_T("OGL Error: [0x%x] An error occured."), (unsigned int)err);
		wxLogMessage(_T("Clearing textures from previous model..."));
#endif

		// Texture clearing and debugging
		texturemanager.clear();

#ifdef _DEBUG
		err = glGetError();
		if (err)
			wxLogMessage(_T("OpenGL Error: [0x%x] An error occured."), (unsigned int)err);
#endif


		wxString rootfn(data->fn.c_str(), wxConvUTF8);

		// Check to make sure the selected item is a model (an *.m2 file).
		modelviewer->isModel = (rootfn.Last() == '2');
		modelviewer->isChar = false;

		if (modelviewer->isModel) {
			modelviewer->isWMO = false;

			// not functional yet.
			//if (wxGetKeyState(WXK_SHIFT)) 
			//	canvas->AddModel(rootfn);
			//else
				modelviewer->LoadModel(rootfn);	// Load the model.
		
		} else { // else, it isn't a m2 file, so load the file as a WMO.
			modelviewer->isWMO = true;

			// is WMO?
			//canvas->model->modelType = MT_WMO;

			// if we have selected a non-root wmo, find the root filename
			char dash = rootfn[data->fn.length() - 8];
			char num = rootfn[data->fn.length() - 7];
			bool isroot = !((dash=='_') && (num>='0') && (num<='9'));
			if (!isroot) {
				rootfn.erase(rootfn.length()-8);
				rootfn.append(_T(".wmo"));
			}

			modelviewer->canvas->LoadWMO(std::string(rootfn.mb_str()));

			int id = -1;
			if (!isroot) {
				char idnum[4];
				strncpy(idnum, data->fn.c_str() + strlen(data->fn.c_str())-7,3);
				//wxString(data->fn.Substr((data->fn.Length() - 7), 3)).ToLong(&id);
				idnum[3]=0;
				sscanf(idnum,"%d",&id);
			}
			modelviewer->canvas->wmo->loadGroup(id);
			modelviewer->canvas->ResetViewWMO(id);
			modelviewer->animControl->UpdateWMO(modelviewer->canvas->wmo, id);

			// wxAUI
			modelviewer->interfaceManager.GetPane(modelviewer->charControl).Show(false);
		}

		// Disable whatever formats can't be export yet!

		// You MUST put true in one if the other is false! Otherwise, if they open the other model type and go back,
		// your function will still be disabled!!
		if (modelviewer->isWMO == true){
			// If the object is a WMO file...
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_INIT, false);	// Disable Init Mode when viewing WMOs...
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_COLLADA, false);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_MS3D, false);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_3DS, false);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_X3D, false);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_XHTML, false);

			modelviewer->charMenu->Enable(ID_SAVE_CHAR, false);
			modelviewer->charMenu->Enable(ID_SHOW_UNDERWEAR, false);
			modelviewer->charMenu->Enable(ID_SHOW_EARS, false);
			modelviewer->charMenu->Enable(ID_SHOW_HAIR, false);
			modelviewer->charMenu->Enable(ID_SHOW_FACIALHAIR, false);
			modelviewer->charMenu->Enable(ID_SHOW_FEET, false);
			modelviewer->charMenu->Enable(ID_SHEATHE, false);
			modelviewer->charMenu->Enable(ID_SAVE_EQUIPMENT, false);
			modelviewer->charMenu->Enable(ID_LOAD_EQUIPMENT, false);
			modelviewer->charMenu->Enable(ID_CLEAR_EQUIPMENT, false);
			modelviewer->charMenu->Enable(ID_LOAD_SET, false);
			modelviewer->charMenu->Enable(ID_LOAD_START, false);
			modelviewer->charMenu->Enable(ID_LOAD_NPC_START, false);
			modelviewer->charMenu->Enable(ID_MOUNT_CHARACTER, false);
			modelviewer->charMenu->Enable(ID_CHAR_RANDOMISE, false);
		}else{
			// If it's an M2 file...
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_INIT, true);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_COLLADA, false);	// Currently totally disabled. No support at all...
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_MS3D, true);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_3DS, true);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_X3D, true);
			modelviewer->exportMenu->Enable(ID_MODELEXPORT_XHTML, true);

			modelviewer->charMenu->Enable(ID_SAVE_CHAR, true);
			modelviewer->charMenu->Enable(ID_SHOW_UNDERWEAR, true);
			modelviewer->charMenu->Enable(ID_SHOW_EARS, true);
			modelviewer->charMenu->Enable(ID_SHOW_HAIR, true);
			modelviewer->charMenu->Enable(ID_SHOW_FACIALHAIR, true);
			modelviewer->charMenu->Enable(ID_SHOW_FEET, true);
			modelviewer->charMenu->Enable(ID_SHEATHE, true);
			modelviewer->charMenu->Enable(ID_SAVE_EQUIPMENT, true);
			modelviewer->charMenu->Enable(ID_LOAD_EQUIPMENT, true);
			modelviewer->charMenu->Enable(ID_CLEAR_EQUIPMENT, true);
			modelviewer->charMenu->Enable(ID_LOAD_SET, true);
			modelviewer->charMenu->Enable(ID_LOAD_START, true);
			modelviewer->charMenu->Enable(ID_LOAD_NPC_START, true);
			modelviewer->charMenu->Enable(ID_MOUNT_CHARACTER, true);
			modelviewer->charMenu->Enable(ID_CHAR_RANDOMISE, true);
		}

		// Update the layout
		modelviewer->interfaceManager.Update();
	} else {
		modelviewer->exportMenu->Enable(ID_MODELEXPORT_INIT, false);
		modelviewer->exportMenu->Enable(ID_MODELEXPORT_COLLADA, false);
		modelviewer->exportMenu->Enable(ID_MODELEXPORT_MS3D, false);
		modelviewer->exportMenu->Enable(ID_MODELEXPORT_3DS, false);
		modelviewer->exportMenu->Enable(ID_MODELEXPORT_X3D, false);
		modelviewer->exportMenu->Enable(ID_MODELEXPORT_XHTML, false);

		modelviewer->charMenu->Enable(ID_SAVE_CHAR, false);
		modelviewer->charMenu->Enable(ID_SHOW_UNDERWEAR, false);
		modelviewer->charMenu->Enable(ID_SHOW_EARS, false);
		modelviewer->charMenu->Enable(ID_SHOW_HAIR, false);
		modelviewer->charMenu->Enable(ID_SHOW_FACIALHAIR, false);
		modelviewer->charMenu->Enable(ID_SHOW_FEET, false);
		modelviewer->charMenu->Enable(ID_SHEATHE, false);
		modelviewer->charMenu->Enable(ID_SAVE_EQUIPMENT, false);
		modelviewer->charMenu->Enable(ID_LOAD_EQUIPMENT, false);
		modelviewer->charMenu->Enable(ID_CLEAR_EQUIPMENT, false);
		modelviewer->charMenu->Enable(ID_LOAD_SET, false);
		modelviewer->charMenu->Enable(ID_LOAD_START, false);
		modelviewer->charMenu->Enable(ID_LOAD_NPC_START, false);
		modelviewer->charMenu->Enable(ID_MOUNT_CHARACTER, false);
		modelviewer->charMenu->Enable(ID_CHAR_RANDOMISE, false);

		// Update the layout
		modelviewer->interfaceManager.Update();
	}
}

// bg recolor
void FileControl::OnTreeCollapsedOrExpanded(wxTreeEvent &event)
{
	wxTreeItemId h;
	size_t i = 0;
	for(h=fileTree->GetFirstVisibleItem();h;h=fileTree->GetNextVisible(h)) {
		if (!fileTree->IsVisible(h))
			break;
		if (i++%2==1)
			fileTree->SetItemBackgroundColour(h, wxColour(237,243,254));
		else
			fileTree->SetItemBackgroundColour(h, *wxWHITE);
	}
}

void FileControl::OnButton(wxCommandEvent &event)
{
	int id = event.GetId();
	if (id == ID_FILELIST_CONTENT)
		Init();
	else if (id == ID_FILELIST_SEARCH) {
		txtContent->SetValue(_T(""));
		Init();
	}
}


