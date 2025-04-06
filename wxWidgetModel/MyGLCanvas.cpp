#include "main.h"

BEGIN_EVENT_TABLE(MyGLCanvas, wxGLCanvas)
EVT_PAINT(MyGLCanvas::OnPaint)
EVT_SIZE(MyGLCanvas::OnSize)
EVT_MOUSE_EVENTS(MyGLCanvas::OnMouse)
EVT_ERASE_BACKGROUND(MyGLCanvas::OnEraseBackground)
END_EVENT_TABLE()

// Define OpenGL attributes at file scope
static const int glAttribs[] = {
    WX_GL_RGBA,
    WX_GL_DOUBLEBUFFER,
    WX_GL_DEPTH_SIZE, 24,
    WX_GL_STENCIL_SIZE, 8,
    0
};

MyGLCanvas::MyGLCanvas(MainFrame* parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
    : wxGLCanvas(parent, id, glAttribs, pos, size, style | wxFULL_REPAINT_ON_RESIZE, name),
    m_parentFrame(parent),
    m_context(nullptr),
    m_buttonHovered(false)
{
    // Initialize scene parameters
    rotationAngle = 0.0f;
    scale = 1.0f;
    showWireframe = false;
    showControlPanel = false;

    // Set button position and size (relative to canvas)
    m_buttonRect = wxRect(20, 20, 120, 40);
}


MyGLCanvas::~MyGLCanvas() {
    CleanupGL();
}

void MyGLCanvas::CleanupGL() {
    if (m_context) {
        delete m_context;
        m_context = nullptr;
    }
}


void MyGLCanvas::OnPaint(wxPaintEvent& WXUNUSED(event)) {
    wxPaintDC dc(this);

    if (!m_context) {
        m_context = new wxGLContext(this);
        SetCurrent(*m_context);
        InitGL();
    }

    SetCurrent(*m_context);
    Render();
    SwapBuffers();
}

void MyGLCanvas::OnSize(wxSizeEvent& event) {
    event.Skip();
    Refresh();
}

void MyGLCanvas::OnMouse(wxMouseEvent& event) {
    if (event.LeftDown()) {
        if (IsButtonClicked(event.GetX(), event.GetY())) {
            showControlPanel = !showControlPanel;
            m_parentFrame->GetControlPanel()->Show(showControlPanel);
            m_parentFrame->Layout();

            // Update controls to match current state
            m_parentFrame->UpdateFromControls();
        }
    }
    else if (event.Moving()) {
        bool wasHovered = m_buttonHovered;
        m_buttonHovered = IsButtonClicked(event.GetX(), event.GetY());

        if (wasHovered != m_buttonHovered) {
            Refresh();
        }
    }

    event.Skip();
}

void MyGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
    // Do nothing to avoid flashing on MSW
}

void MyGLCanvas::InitGL() {
    SetCurrent(*m_context);

    // Basic OpenGL setup
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    // Set up projection matrix
    int w, h;
    GetClientSize(&w, &h);
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyGLCanvas::RenderText(const wxString& text, int x, int y) {
    // Save OpenGL state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int w, h;
    GetClientSize(&w, &h);
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth test and lighting for 2D rendering
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Render text using wxWidgets
    wxClientDC dc(this);
    dc.SetTextForeground(*wxWHITE);
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.DrawText(text, x, y);

    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void MyGLCanvas::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    // Draw 3D scene
    glPushMatrix();
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
    glScalef(scale, scale, scale);

    if (showWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Draw a colored cube
    glBegin(GL_QUADS);
    // Front face (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // Back face (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Top face (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    // Bottom face (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);

    // Right face (magenta)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Left face (cyan)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();

    glPopMatrix();

    // Switch to orthographic projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int w, h;
    GetClientSize(&w, &h);
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable depth test and lighting for 2D rendering
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Draw the overlay button
    if (m_buttonHovered) {
        glColor3f(0.6f, 0.6f, 0.8f);  // Hover color
    }
    else {
        glColor3f(0.4f, 0.4f, 0.6f);  // Normal color
    }

    glBegin(GL_QUADS);
    glVertex2i(m_buttonRect.GetLeft(), m_buttonRect.GetTop());
    glVertex2i(m_buttonRect.GetRight(), m_buttonRect.GetTop());
    glVertex2i(m_buttonRect.GetRight(), m_buttonRect.GetBottom());
    glVertex2i(m_buttonRect.GetLeft(), m_buttonRect.GetBottom());
    glEnd();

    // Draw button text using wxWidgets text rendering
    RenderText(showControlPanel ? "Hide Controls" : "Show Controls",
        m_buttonRect.GetLeft() + 10, m_buttonRect.GetTop() + 10);

    // Restore state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

bool MyGLCanvas::IsButtonClicked(int x, int y) {
    return m_buttonRect.Contains(x, y);
}