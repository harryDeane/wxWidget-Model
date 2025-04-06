#ifndef MAIN_H
#define MAIN_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/slider.h>
#include <wx/checkbox.h>

#ifdef __WXMAC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MainFrame;

class MyGLCanvas : public wxGLCanvas {
public:
    MyGLCanvas(MainFrame* parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0, const wxString& name = "GLCanvas");
    ~MyGLCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    void Render();
    bool IsButtonClicked(int x, int y);
    void InitGL();
    void RenderText(const wxString& text, int x, int y);
    void CleanupGL();

    // Scene parameters
    float rotationAngle;
    float scale;
    bool showWireframe;
    bool showControlPanel;

private:
    static const int* GetGLAttributes();  // Changed to return const int*

    wxGLContext* m_context;
    MainFrame* m_parentFrame;

    // Button properties
    wxRect m_buttonRect;
    bool m_buttonHovered;

    DECLARE_EVENT_TABLE()
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);

    void OnSliderChange(wxCommandEvent& event);
    void OnCheckBox(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    MyGLCanvas* GetGLCanvas() { return m_glCanvas; }
    wxPanel* GetControlPanel() { return m_controlPanel; }

    void UpdateFromControls();

private:
    MyGLCanvas* m_glCanvas;
    wxPanel* m_controlPanel;
    wxSlider* m_rotationSlider;
    wxCheckBox* m_wireframeCheckBox;

    DECLARE_EVENT_TABLE()
};

#endif // MAIN_H