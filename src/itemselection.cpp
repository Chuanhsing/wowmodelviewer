
#include "itemselection.h"
#include "charcontrol.h"

// HACK: this is the ID for the single choice dialog listbox in the wx src
// - if it changes this code may break
#define wxID_LISTBOX 3000
#define wxID_LISTCTRL 3001

BEGIN_EVENT_TABLE(ChoiceDialog, wxSingleChoiceDialog)
	EVT_LISTBOX(wxID_LISTBOX, FilteredChoiceDialog::OnClick)
	EVT_LIST_ITEM_SELECTED(wxID_LISTCTRL, ChoiceDialog::OnSelect)
END_EVENT_TABLE()

ChoiceDialog::ChoiceDialog(CharControl *dest, int type,
	                       wxWindow *parent,
                           const wxString& message,
                           const wxString& caption,
                           const wxArrayString& choices)
    : wxSingleChoiceDialog(parent, message, caption, choices, (char**)NULL, wxCHOICEDLG_STYLE & ~wxCANCEL & ~wxCENTER, wxDefaultPosition)
{
	cc = dest;
	this->type = type;

	m_listctrl = NULL;

	/*
	// New Item Selection stuff
	// Objective is to change over from a wxListBox to a wxListCtrl
	// which supports different text colours
	wxCArrayString chs(choices);
	m_listctrl = new wxListView(this, wxID_LISTCTRL, wxDefaultPosition, wxSize(200,200), wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_NO_HEADER); 
	
	m_listctrl->InsertColumn(0, _T("Item"), wxLIST_FORMAT_LEFT, 180);
	//m_listctrl->SetColumnWidth(0, wxLIST_AUTOSIZE);

	for (unsigned int i = 0; i < chs.GetCount(); i++) {
		m_listctrl->InsertItem(i, choices[i]);
	}

	wxBoxSizer *frameSizer = (wxBoxSizer*)this->GetSizer();
	if (frameSizer) {
		frameSizer->Detach(m_listbox);
		frameSizer->Insert(1, m_listctrl, wxEXPAND|wxADJUST_MINSIZE|wxLEFT|wxRIGHT);

		m_listbox->Show(false);
	}
	*/
}

void ChoiceDialog::OnClick(wxCommandEvent &event)
{
    m_selection = m_listbox->GetSelection();

	if (m_selection == -1) // If nothing was selected, exit function.
		return;

    m_stringSelection = m_listbox->GetStringSelection();
	if (cc)	
		cc->OnUpdateItem(type, GetSelection());
}

void ChoiceDialog::OnSelect(wxListEvent &event)
{
    m_selection = m_listctrl->GetFocusedItem();

	if (m_selection == -1) // If nothing was selected, exit function.
		return;

    m_stringSelection = m_listbox->GetStringSelection();
	if (cc)	
		cc->OnUpdateItem(type, GetSelection());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(FilteredChoiceDialog, ChoiceDialog)
    EVT_TEXT_ENTER(FilteredChoiceDialog::ID_FILTER_TEXT, FilteredChoiceDialog::OnFilter)
    EVT_TEXT(FilteredChoiceDialog::ID_FILTER_TEXT, FilteredChoiceDialog::OnFilter)
    EVT_BUTTON(FilteredChoiceDialog::ID_FILTER_BUTTON, FilteredChoiceDialog::OnFilter)
    EVT_BUTTON(FilteredChoiceDialog::ID_FILTER_CLEAR, FilteredChoiceDialog::OnFilter)
END_EVENT_TABLE()


FilteredChoiceDialog::FilteredChoiceDialog(CharControl *dest, int type, wxWindow *parent,
                            const wxString& message,
                            const wxString& caption,
                            const wxArrayString& choices,
							const std::vector<int> *quality,
							bool keepfirst)
    :ChoiceDialog(dest, type, parent, message, caption, choices)
{
	keepFirst = keepfirst;
    m_choices = &choices;
    m_indices.resize(m_choices->GetCount());
    for(size_t i=0; i<m_choices->GetCount(); ++i) 
		m_indices[i]=(int)i;
    
    wxSizer *topsizer = GetSizer();
    
    wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    m_pattern = new wxTextCtrl(this, ID_FILTER_TEXT, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER); // fnmatch: "*" re: ""
    
    sizer->Add(new wxStaticText(this, -1, _("Filter   ")), 0, wxALIGN_CENTER_VERTICAL);
	sizer->Add(m_pattern, 1, 0);
    sizer->Add(new wxButton(this, ID_FILTER_CLEAR, _("Clear"), wxDefaultPosition, wxSize(40,-1)), 0, 0);
    
    topsizer->Prepend(sizer, 0, wxEXPAND | wxALL, 10);
    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    
	/*
	m_listbox->Clear();

	for(unsigned int i=0; i<choices.Count(); i++) {
		wxListItem item; 
		//item.m_mask=wxLIST_MASK_TEXT; 
		//item.m_itemId=tmp; 
		//item.m_col=col; 
		item.m_text=choices[i]; 
		if (quality[i] < 3)
			item.SetTextColour(*wxRED); 
		else
			item.SetTextColour(*wxBLUE); 

		m_listbox->InsertItem(item);
	}
	*/

    m_listbox->SetFocus();
}

void FilteredChoiceDialog::OnFilter(wxCommandEvent& event){
	if (event.GetId() == ID_FILTER_CLEAR) 
		m_pattern->SetValue(_T(""));
    DoFilter();
}

void FilteredChoiceDialog::DoFilter()
{
	InitFilter();

	m_indices.clear();

    wxArrayString filtered;
    for(int i=0; i<(int)m_choices->GetCount(); ++i){
		if (FilterFunc(i)) {
            //m_indices[filtered.GetCount()]=(int)i;
			m_indices.push_back((int)i);
            filtered.Add(m_choices->Item(i));
        }
    }
    
    m_listbox->Set(filtered);
}

void FilteredChoiceDialog::InitFilter()
{
	std::string f = "^.*";
    wxString pattern(m_pattern->GetValue());
	for (size_t i=0; i<pattern.Length(); i++) {
		char c = pattern[i];
		if (c=='?') f.append(".");
		else if (c=='*') f.append(".*");
		else f.append(1,c);
	}
	f.append(".*$");

	filter.Compile(wxString(f.c_str()), wxRE_ICASE);
}

bool FilteredChoiceDialog::FilterFunc(int index)
{
	if (index==0 && keepFirst) 
		return true;

	return filter.Matches(m_choices->Item(index));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CategoryChoiceDialog, FilteredChoiceDialog)
	EVT_CHECKLISTBOX(CategoryChoiceDialog::ID_CAT_LIST, CategoryChoiceDialog::OnCheck)
	EVT_LISTBOX_DCLICK(CategoryChoiceDialog::ID_CAT_LIST, CategoryChoiceDialog::OnCheckDoubleClick)
END_EVENT_TABLE()

CategoryChoiceDialog::CategoryChoiceDialog(CharControl *dest, int type,
	                    wxWindow *parent,
                        const wxString& message,
                        const wxString& caption,
                        const wxArrayString& choices,
						const std::vector<int> &cats,
						const wxArrayString& catnames,
						const std::vector<int> *quality,
						bool keepfirst,
						bool helpmsg):
	FilteredChoiceDialog(dest, type, parent, message, caption, choices, quality, keepfirst), m_cats(cats)
{
	m_catlist = new wxCheckListBox(this, ID_CAT_LIST, wxDefaultPosition, wxDefaultSize, catnames, wxLB_HSCROLL);
	for (size_t i=0; i<catnames.GetCount(); i++) 
		m_catlist->Check((unsigned int)i);

	numcats = (int)catnames.GetCount();

    wxSizer *topsizer = GetSizer();

    topsizer->Prepend(m_catlist, 0, wxEXPAND | wxALL, 10);

	if (helpmsg) {
		topsizer->Prepend(new wxStaticText(this, -1, _("Double-click to select only one category")), 0, wxEXPAND | wxTOP | wxLEFT, 10);
	}

    topsizer->SetSizeHints(this);
    topsizer->Fit(this);

    m_listbox->SetFocus();
}

bool CategoryChoiceDialog::FilterFunc(int index)
{
	if (index==0 && keepFirst) 
		return true;

	return m_catlist->IsChecked(m_cats[index]) && FilteredChoiceDialog::FilterFunc(index);
}

void CategoryChoiceDialog::OnCheck(wxCommandEvent &e)
{
    DoFilter();
}

void CategoryChoiceDialog::OnCheckDoubleClick(wxCommandEvent &e)
{
	for (int i=0; i<numcats; i++) {
		Check(i, i==e.GetInt());
	}
	DoFilter();
}

void CategoryChoiceDialog::Check(int index, bool state)
{
	m_catlist->Check(index, state);
}
