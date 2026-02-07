#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/textdlg.h>
#include <wx/image.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/quantize.h>
#include <wx/utils.h>
#include <vector>
#include "gifmake.h";

class MainFrame : public wxFrame
{
public:

	MainFrame(const wxString& title);

private:
	wxListCtrl* m_pListOfFrames;
	wxButton* m_pMoveUp;
	wxButton* m_pMoveDown;
	wxButton* m_pCompile;
	wxButton* m_pClear;
	wxButton* m_pView;
	wxButton* m_pDelete;
	void OnAbout(wxCommandEvent& e);
	void OnExit(wxCommandEvent& e);
	void OnOpen(wxCommandEvent& e);
	void OnRowDoubleClicked(wxListEvent& e);
	void OnRowSelected(wxListEvent& e);
	void OnSetDura(wxCommandEvent& e);
	void MoveItemUp(wxCommandEvent& e);
	void MoveItemDown(wxCommandEvent& e);
	void Compile(wxCommandEvent& e);
	void Clear(wxCommandEvent& e);
	void Delete(wxCommandEvent& e);
	void View(wxCommandEvent& e);
	void SwapRows(uint64_t p, uint64_t q);
	void RestackIndices();
};
