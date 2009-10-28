#ifndef FILECONTROL_H
#define FILECONTROL_H

class FileControl: public wxWindow
{
	DECLARE_CLASS(FileControl)
	DECLARE_EVENT_TABLE()

	std::set<FileTreeItem> filelist;
public:
	// Constructor + Deconstructor
	FileControl(wxWindow* parent, wxWindowID id);
	~FileControl();

	void Init(ModelViewer* mv=NULL);
	void OnTreeSelect(wxTreeEvent &event);
	void OnTreeCollapsedOrExpanded(wxTreeEvent &event);
	void OnButton(wxCommandEvent &event);

	wxTreeCtrl *fileTree;
	wxButton *btnSearch;
	wxTextCtrl *txtContent;

	ModelViewer* modelviewer; // point to parent
};

#endif