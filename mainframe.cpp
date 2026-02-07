#include "mainframe.h"

MainFrame::MainFrame(const wxString& title) : 
	wxFrame(
		nullptr, 
		wxID_ANY, 
		title,
		wxDefaultPosition, 
		wxSize(800, 510),
		wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)
	)
{
	wxMenuBar* pMenuBar = new wxMenuBar();
	wxMenu* pFileMenu = new wxMenu();
	wxMenu* pFileAbout = new wxMenu();

	pFileMenu->Append(wxID_OPEN, "Frames...\tCtrl+O", "Select frame(s)...");
	pFileMenu->Append(MainFrame::_ID_SET_DURAS, "Set Durations\tCtrl+D", "Set durations for frames...");
	pFileMenu->Append(wxID_EXIT, "E&xit\tCtrl-C", "Exit");
	pFileAbout->Append(wxID_ABOUT, "About", "About");
	pMenuBar->Append(pFileMenu, "&File");
	pMenuBar->Append(pFileAbout, "About");
	SetMenuBar(pMenuBar);

	Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::OnSetDura, this, MainFrame::_ID_SET_DURAS);

	wxPanel* pListPanel = new wxPanel(this);
	m_pListOfFrames = new wxListCtrl(
		pListPanel, 
		wxID_ANY, 
		wxDefaultPosition, 
		wxSize(800, 400), 
		wxLC_REPORT | wxLC_SINGLE_SEL
	);
	m_pListOfFrames->InsertColumn(0, "#", wxLIST_FORMAT_LEFT, 50);
	m_pListOfFrames->InsertColumn(1, "Duration", wxLIST_FORMAT_LEFT, 100);
	m_pListOfFrames->InsertColumn(2, "Frame", wxLIST_FORMAT_LEFT, 650);
	m_pListOfFrames->Bind(wxEVT_LIST_ITEM_ACTIVATED, &MainFrame::OnRowDoubleClicked, this);
	m_pListOfFrames->Bind(wxEVT_LIST_ITEM_SELECTED, &MainFrame::OnRowSelected, this);

	m_pView = new wxButton(pListPanel, wxID_ANY, "View", wxPoint(240, 410), wxSize(80, 32));
	m_pDelete = new wxButton(pListPanel, wxID_ANY, "Delete", wxPoint(330, 410), wxSize(80, 32));
	m_pClear = new wxButton(pListPanel, wxID_ANY, "Clear", wxPoint(420, 410), wxSize(80, 32));
	m_pMoveUp = new wxButton(pListPanel, wxID_ANY, "Move Up", wxPoint(510, 410), wxSize(80, 32));
	m_pMoveDown = new wxButton(pListPanel, wxID_ANY, "Move Down", wxPoint(600, 410), wxSize(80, 32));
	m_pCompile = new wxButton(pListPanel, wxID_ANY, "Compile", wxPoint(690, 410), wxSize(80, 32));
	m_pView->Disable();
	m_pDelete->Disable();
	m_pMoveUp->Disable();
	m_pMoveDown->Disable();
	m_pCompile->Disable();
	m_pClear->Disable();
	m_pView->Bind(wxEVT_BUTTON, &MainFrame::View, this);
	m_pDelete->Bind(wxEVT_BUTTON, &MainFrame::Delete, this);
	m_pClear->Bind(wxEVT_BUTTON, &MainFrame::Clear, this);
	m_pMoveUp->Bind(wxEVT_BUTTON, &MainFrame::MoveItemUp, this);
	m_pMoveDown->Bind(wxEVT_BUTTON, &MainFrame::MoveItemDown, this);
	m_pCompile->Bind(wxEVT_BUTTON, &MainFrame::Compile, this);
}

void MainFrame::OnSetDura(wxCommandEvent& e)
{
	wxTextEntryDialog prompt(
		this,
		"Set in seconds (1dp):",
		"Set Durations",
		"1.0"
	);
	if (prompt.ShowModal() == wxID_OK)
	{
		double v;
		if (!prompt.GetValue().ToDouble(&v))
			return;
		if (v <= 0)
			return;
		for (uint64_t y = 0; y < m_pListOfFrames->GetItemCount(); y++)
			m_pListOfFrames->SetItem(y, 1, wxString::Format("%.1f", v));
	}
}

void MainFrame::OnRowSelected(wxListEvent& e)
{
	uint64_t y = m_pListOfFrames->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (y >= 1)
		m_pMoveUp->Enable();
	else
		m_pMoveUp->Disable();
	if (y < m_pListOfFrames->GetItemCount() - 1 )
		m_pMoveDown->Enable();
	else
		m_pMoveDown->Disable();
	m_pDelete->Enable();
	m_pView->Enable();
}

void MainFrame::View(wxCommandEvent& e)
{
	uint64_t y = m_pListOfFrames->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (y != -1)
		wxLaunchDefaultBrowser(m_pListOfFrames->GetItemText(y, 2));
}

void MainFrame::Delete(wxCommandEvent& e)
{
	uint64_t y = m_pListOfFrames->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (y == -1)
		return;
	m_pListOfFrames->DeleteItem(y);
	m_pMoveDown->Disable();
	m_pMoveUp->Disable();
	m_pView->Disable();
	m_pDelete->Disable();
	this->RestackIndices();
	if (m_pListOfFrames->GetItemCount() == 0)
	{
		m_pClear->Disable();
		m_pCompile->Disable();
	}
	else
		m_pCompile->Enable();
}

void MainFrame::MoveItemUp(wxCommandEvent& e)
{
	uint64_t y = m_pListOfFrames->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (y >= 1)
		this->SwapRows(y, y-1);
}

void MainFrame::MoveItemDown(wxCommandEvent& e)
{
	uint64_t y = m_pListOfFrames->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (y < m_pListOfFrames->GetItemCount() - 1)
		this->SwapRows(y, y+1);
}

void MainFrame::Compile(wxCommandEvent& e)
{
	wxFileDialog sd(
		this, 
		"Save Gif", "", "",
		"Gif file (*.gif)|*.gif",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (sd.ShowModal() == wxID_OK)
	{
		/*
		invoke the image handlers
		*/
		wxInitAllImageHandlers();

		/*
		full path to frame
		*/
		wxString p = sd.GetPath();

		/*
		work variables
		*/
		double dura;
		std::vector<wxImage> frames;
		std::vector<double> duras;
		uint32_t w, h, fps;

		for (uint64_t y = 0; y < m_pListOfFrames->GetItemCount(); y++)
		{
			/*
			extract duration
			*/
			if (!m_pListOfFrames->GetItemText(y, 1).ToDouble(&dura))
			{
				wxMessageBox(
					"Format conversion fault extracting duration", "Process Error", 
					wxOK | wxICON_ERROR
				);
				return;
			}

			/*
			load frame from disk
			*/
			wxImage frame(m_pListOfFrames->GetItemText(y, 2));
			if (!frame.IsOk())
			{
				wxMessageBox(
					"Image binary could not be parsed", "Process Error", 
					wxOK | wxICON_ERROR
				);
				return;
			}

			/*
			record and assert the image dims
			*/
			if (y == 0)
			{
				w = frame.GetWidth();
				h = frame.GetHeight();
			}
			else if (w != frame.GetWidth() || h != frame.GetHeight())
			{
				wxMessageBox(
					"Dimensions are not consistent amongst image set", "Process Error", 
					wxOK | wxICON_ERROR
				);
				return;
			}

			/*
			clamp image data to the gif palette colour space
			*/
			wxImage ib;
			wxQuantize::Quantize(frame, ib, 256);

			/*
			stack frame counts relative to a 25ms resolution
			*/
			fps = (dura * 1000) / 25;
			for ( uint32_t k = 0; k < fps; k++ )
				frames.push_back(ib);
			duras.push_back(dura);
		}

		/*
		compile image set into a gif
		*/
		if (!frames.empty())
		{
			wxFileOutputStream outputStream(p);
			wxGIFHandler* gh = new wxGIFHandler();
			if ( !gh->SaveAnimation(frames, &outputStream, false, 25) )
				wxMessageBox("The gif could not be saved", "Save Error", wxOK | wxICON_ERROR);
			outputStream.Close();
		}
	}
}

void MainFrame::Clear(wxCommandEvent& e)
{
	m_pListOfFrames->DeleteAllItems();
	m_pMoveUp->Disable();
	m_pMoveDown->Disable();
	m_pCompile->Disable();
	m_pClear->Disable();
	m_pDelete->Disable();
	m_pView->Disable();
}

void MainFrame::OnRowDoubleClicked(wxListEvent& e) {
	uint64_t y = e.GetIndex();
	wxTextEntryDialog prompt(
		this, 
		"Set in seconds (1dp):", 
		"Set Duration", 
		m_pListOfFrames->GetItemText(y, 1)
	);
	if (prompt.ShowModal() == wxID_OK)
	{
		double v;
		if (!prompt.GetValue().ToDouble(&v))
			return;
		if (v <= 0)
			return;
		m_pListOfFrames->SetItem(y, 1, wxString::Format("%.1f", v));
	}
}

void MainFrame::OnAbout(wxCommandEvent& e) {
	wxMessageBox(
		"GIFMake\n\n...compile frames into an animation", 
		"About", 
		wxOK | wxICON_INFORMATION
	);
}

void MainFrame::OnExit(wxCommandEvent& e) {
	Close(true);
}

void MainFrame::OnOpen(wxCommandEvent& e) {
	wxFileDialog openFileDialog(
		this, 
		"Select Frame(s)", "", "",
		"Common Frames (*.bmp;*.png;*.jpg)|*.bmp;*.png;*.jpg|Bitmaps (*.bmp)|*.bmp|Jpegs (*.jpg)|*.jpg|Portable Network Graphics (*.png)|*.png",
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST
	);
	if (openFileDialog.ShowModal() == wxID_OK)
	{
		wxArrayString fps;
		openFileDialog.GetPaths(fps);
		for (const wxString& p : fps)
		{
			uint64_t c = m_pListOfFrames->GetItemCount();
			uint64_t y = m_pListOfFrames->InsertItem(c, wxString::Format("%llu", c+1));
			m_pListOfFrames->SetItem(y, 1, "1.0");
			m_pListOfFrames->SetItem(y, 2, p);
		}
		if (m_pListOfFrames->GetItemCount() != 0)
		{
			m_pCompile->Enable();
			m_pClear->Enable();
		}
	}
}

void MainFrame::SwapRows(uint64_t p, uint64_t q)
{
	wxString q1 = m_pListOfFrames->GetItemText(p, 1);
	wxString w1 = m_pListOfFrames->GetItemText(p, 2);
	wxString q2 = m_pListOfFrames->GetItemText(q, 1);
	wxString w2 = m_pListOfFrames->GetItemText(q, 2);
	m_pListOfFrames->SetItem(p, 1, q2);
	m_pListOfFrames->SetItem(p, 2, w2);
	m_pListOfFrames->SetItem(q, 1, q1);
	m_pListOfFrames->SetItem(q, 2, w1);
	m_pListOfFrames->SetItemState(q, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MainFrame::RestackIndices()
{
	for (uint64_t y = 0; y < m_pListOfFrames->GetItemCount(); y++)
		m_pListOfFrames->SetItem(y, 0, wxString::Format("%llu", y + 1));
}