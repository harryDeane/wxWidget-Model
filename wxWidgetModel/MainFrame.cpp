#include "main.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    // Create main sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Create GL canvas
    m_glCanvas = new MyGLCanvas(this);
    mainSizer->Add(m_glCanvas, 1, wxEXPAND);

    // Create control panel
    m_controlPanel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    // Add controls to panel
    m_rotationSlider = new wxSlider(m_controlPanel, wxID_ANY, 0, 0, 360,
        wxDefaultPosition, wxDefaultSize,
        wxSL_LABELS | wxSL_VERTICAL);
    panelSizer->Add(m_rotationSlider, 1, wxEXPAND | wxALL, 5);

    m_wireframeCheckBox = new wxCheckBox(m_controlPanel, wxID_ANY, "Wireframe Mode");
    panelSizer->Add(m_wireframeCheckBox, 0, wxALL, 5);

    m_controlPanel->SetSizer(panelSizer);
    mainSizer->Add(m_controlPanel, 0, wxEXPAND);

    // Initially hide the control panel
    m_controlPanel->Hide();

    SetSizer(mainSizer);

    // Bind events
    m_rotationSlider->Bind(wxEVT_SLIDER, &MainFrame::OnSliderChange, this);
    m_wireframeCheckBox->Bind(wxEVT_CHECKBOX, &MainFrame::OnCheckBox, this);
}

void MainFrame::OnSliderChange(wxCommandEvent& event) {
    m_glCanvas->rotationAngle = m_rotationSlider->GetValue();
    m_glCanvas->Refresh();
}

void MainFrame::OnCheckBox(wxCommandEvent& event) {
    m_glCanvas->showWireframe = m_wireframeCheckBox->GetValue();
    m_glCanvas->Refresh();
}

void MainFrame::OnClose(wxCloseEvent& event) {
    if (m_glCanvas) {
        m_glCanvas->CleanupGL();
    }
    event.Skip();
}

void MainFrame::UpdateFromControls() {
    m_rotationSlider->SetValue(m_glCanvas->rotationAngle);
    m_wireframeCheckBox->SetValue(m_glCanvas->showWireframe);
}