#include <wx/wxprec.h>
#include <iostream>
#include <algorithm>
#include <wx/glcanvas.h>
#include <wx/splash.h>
#include <wx/image.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#ifdef __WXMAC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <list>
struct point {
	int x;
	int y;
};
typedef struct point point;

struct _color {
	float r;
	float g;
	float b;
};
typedef struct _color _color;

#define DEG2RAD 3.14159/180.0
#define RED 1.0,0.0,0.0
#define BLUE 0.0,0.0,1.0
#define GREEN 0.0,1.0,0.0
#define YELLOW 1.0,1.0,0.0
#define WHITE 1.0,1.0,1.0
#define BLACK 0.0,0.0,0.0
#define PURPLE 0.5,0.0,1.0

using namespace std;
list<point> colist;
list<point> drag;

wxStatusBar* m_statusBar1;
int PCOMMAND = 99;
bool DRAG_START;
void* old_pixel_data;
char* fill_pixels;
int PIXEL_ROW;
int PIXEL_COL;
point PRP = { -1,-1 };
_color BG_COLOR = {1.0,1.0,1.0};
_color FG_COLOR = {0.0,0.0,0.0};
_color TMP_COLOR = {0.5,0.5,0.5};

enum {
	ID_NEW,
	ID_LINE,
	ID_CURVE,
	ID_OVAL,
	ID_TRIANGLE,
	ID_RRECT,
	ID_HEX,
	ID_FILL,
	ID_CROP,
	ID_ERASE,
	ID_SAVE,
	ID_HELP,
	ID_EXIT,
	ID_FGRED,
	ID_FGBLUE,
	ID_FGGREEN,
	ID_FGYELLOW,
	ID_FGWHITE,
	ID_FGBLACK,
	ID_FGPURPLE
};

class BasicGLPane : public wxGLCanvas
{
    wxGLContext*	m_context;
private:
	void takeAction();
	void FloodFill(int x,int y,int r,int g,int b);
	void getPixel(float *r,float *g,float *b,int x,int y);
	void putPixel(int x,int y,float r,float g,float b);
	void CirclePoints(int x,int y,point center,int sx,int bx,int sy,int by);
	void drawCorner(point center,int radius,point c1,point c2);
	void drawRRect(point sp,point ep);
	void drawEllipse(float r1,float r2,point center);
	void saveFile();
 
public:
	BasicGLPane(wxFrame* parent, int* args);
	virtual ~BasicGLPane();
 
	void resized(wxSizeEvent& evt);
 
	int getWidth();
	int getHeight();
 
	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	
 
	// events
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
 
	DECLARE_EVENT_TABLE()
};

class Paint:public wxApp
{
	public:
	virtual bool OnInit();
	BasicGLPane * glPane;
};

class PFrame:public wxFrame
{
	public:
		PFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void OnToolRightClick(wxCommandEvent& event);
	void onClickRedFg(wxCommandEvent& event);
	void onClickBlueFg(wxCommandEvent& event);
	void onClickGreenFg(wxCommandEvent& event);
	void onClickYellowFg(wxCommandEvent& event);
	void onClickBlackFg(wxCommandEvent& event);
	void onClickWhiteFg(wxCommandEvent& event);
	void onClickPurpleFg(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(PFrame,wxFrame)
EVT_TOOL_RCLICKED(wxID_ANY, PFrame::OnToolRightClick)
EVT_MENU(ID_FGRED,PFrame::onClickRedFg)
EVT_MENU(ID_FGBLUE,PFrame::onClickBlueFg)
EVT_MENU(ID_FGGREEN,PFrame::onClickGreenFg)
EVT_MENU(ID_FGYELLOW,PFrame::onClickYellowFg)
EVT_MENU(ID_FGWHITE,PFrame::onClickWhiteFg)
EVT_MENU(ID_FGBLACK,PFrame::onClickBlackFg)
EVT_MENU(ID_FGPURPLE,PFrame::onClickPurpleFg)
END_EVENT_TABLE()

bool Paint::OnInit()
{
	//wxImage::AddHandler(new wxPNGHandler);
	
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	PFrame *frame = new PFrame( "Canvas vs 1.0", wxPoint(50, 50), wxSize(500, 500) );

	wxBitmap bitmap;
	bool ok = bitmap.LoadFile(wxT("/usr/local/share/canvas/canvas.png"), wxBITMAP_TYPE_PNG);
	if (ok)
	{
		new wxSplashScreen(bitmap,
			wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
			6000, frame, wxID_ANY, wxDefaultPosition, wxSize(500, 500),
			wxSIMPLE_BORDER|wxSTAY_ON_TOP);
	}
	wxYield();
	
	int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
	glPane = new BasicGLPane( (wxFrame*) frame, args);
	sizer->Add(glPane, 1, wxEXPAND);
	frame->SetSizer(sizer);
    frame->SetAutoLayout(true);
    sleep(5);
	frame->Show( true );
	return true;
}

BEGIN_EVENT_TABLE(BasicGLPane, wxGLCanvas)
EVT_MOTION(BasicGLPane::mouseMoved)
EVT_LEFT_DOWN(BasicGLPane::mouseDown)
EVT_LEFT_UP(BasicGLPane::mouseReleased)
EVT_RIGHT_DOWN(BasicGLPane::rightClick)
EVT_LEAVE_WINDOW(BasicGLPane::mouseLeftWindow)
EVT_SIZE(BasicGLPane::resized)
EVT_KEY_DOWN(BasicGLPane::keyPressed)
EVT_KEY_UP(BasicGLPane::keyReleased)
EVT_MOUSEWHEEL(BasicGLPane::mouseWheelMoved)
EVT_PAINT(BasicGLPane::render)
END_EVENT_TABLE()
 
 
// some useful events to use
void BasicGLPane::mouseMoved(wxMouseEvent& event) 
{
	m_statusBar1->SetStatusText("mouse moved");
	point cur;
	cur.x = wxGetMousePosition().x - this->GetScreenPosition().x;
	cur.y = wxGetMousePosition().y - this->GetScreenPosition().y;
	if(PCOMMAND == 1){
		if(colist.size()>=1){
			point sp = colist.front();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view); 
			glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
			glBegin(GL_LINES);
			glVertex2f(sp.x,sp.y);
			glVertex2f(cur.x,cur.y);
			glEnd();
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 2)
	{
		if(colist.size()>=1){
			if(PRP.x == -1 )
			PRP = colist.front();
			glBegin(GL_LINES);
			glVertex2f(cur.x,cur.y);
			glVertex2f(PRP.x,PRP.y);
			glEnd();
			PRP = cur;
			glFlush();
			SwapBuffers();
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view); 
			//free(old_pixel_data);
			old_pixel_data = malloc(3 * view[2] * view[3]); // must use malloc 
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
			glPixelStorei(GL_PACK_ALIGNMENT, 1); 
			//glReadBuffer( GL_BACK_LEFT ); 
			glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, old_pixel_data);
		}
	}
	if(PCOMMAND == 3)
	{
		if(colist.size()>=1){
			point sp = colist.front();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view);
			glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
			int g_x = max(sp.x,cur.x);
			int l_x = min(sp.x,cur.x);
			int g_y = max(sp.y,cur.y);
			int l_y = min(sp.y,cur.y);
			point center;
			center.x = (sp.x + cur.x)/2;
			center.y = (sp.y + cur.y)/2;
			int r1 = g_x - l_x;
			int r2 = g_y - l_y;
			drawEllipse(r1,r2,center);
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 4)
	{
		if(colist.size()==1)
		{
			point p1 = colist.front();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view);
			glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
			glBegin(GL_LINES);
			glVertex2f(p1.x,p1.y);
			glVertex2f(cur.x,cur.y);
			glEnd();
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 5)
	{
		if(colist.size()>=1)
		{
			point sp = colist.front();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view);
			glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
			point ep = cur;
			if((sp.x > ep.x )&&(sp.y < ep.y))
			{
				int temp = sp.x;
				sp.x = ep.x;
				ep.x = temp;
			}
			if(( sp.x < ep.x ) && (sp.y > ep.y ))
			{
				int temp = sp.y;
				sp.y = ep.y;
				ep.y = temp;
			}
			if(( sp.x > ep.x ) && (sp.y > ep.y ))
			{
				point temp = sp;
				sp  = ep;
				ep = temp;
			}
			drawRRect(sp,ep);
		}
	}
	if(PCOMMAND == 6)
	{
		if(colist.size()>=1){
			point sp = colist.front();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view);
			glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
			//rotate and get the 4 points by 60 degrees
			point center;
			center.x = (sp.x + cur.x)/2;
			center.y = (sp.y + cur.y)/2;
			//translate center to origin hence cord will be
			sp.x = sp.x - center.x;
			sp.y = sp.y - center.y;
			point p1,p2,p3,p4;
			p1.x = sp.x*0.5 - sp.y*0.866;
			p1.y = sp.x*0.866 + sp.y*0.5;
			p2.x = p1.x*0.5 - p1.y*0.866;
			p2.y = p1.x*0.866 + p1.y*0.5;
			p3.x = (cur.x - center.x)*0.5 - (cur.y - center.y)*0.866;
			p3.y = (cur.x - center.x)*0.866 + (cur.y - center.y)*0.5;
			p4.x = p3.x*0.5 - p3.y*0.866;
			p4.y = p3.x*0.866 + p3.y*0.5;
			sp = colist.front();
			glBegin(GL_LINE_LOOP);
			glVertex2f(sp.x,sp.y);
			glVertex2f(p1.x + center.x,p1.y + center.y);
			glVertex2f(p2.x + center.x,p2.y + center.y);
			glVertex2f(cur.x,cur.y);
			glVertex2f(p3.x + center.x,p3.y + center.y);
			glVertex2f(p4.x + center.x,p4.y + center.y);
			glEnd();
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 8){
		if(colist.size()>=1){
			point sp = colist.front();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view);
			glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
			glPushAttrib(GL_ENABLE_BIT);
			//# glPushAttrib is done to return everything to normal after drawing
			glLineStipple(4, 0xAAAA);
			glEnable(GL_LINE_STIPPLE);
			glBegin(GL_LINE_LOOP);
			glVertex2f(sp.x,sp.y);
			glVertex2f(sp.x,cur.y);
			glVertex2f(cur.x,cur.y);
			glVertex2f(cur.x,sp.y);
			glEnd();
			glDisable(GL_LINE_STIPPLE);
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 9)
	{
		if(colist.size()>=1){
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view);
			glColor3f(BG_COLOR.r,BG_COLOR.g,BG_COLOR.b);
			glBegin(GL_QUADS);
			glVertex2f(cur.x-10,cur.y-10);
			glVertex2f(cur.x+10,cur.y-10);
			glVertex2f(cur.x+10,cur.y+10);
			glVertex2f(cur.x-10,cur.y+10);
			glEnd();
		//glDisable(GL_LINE_STIPPLE);
			glFlush();
			SwapBuffers();
			glColor3f(FG_COLOR.r,FG_COLOR.g,FG_COLOR.b);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, old_pixel_data);
		}
	}
}
void BasicGLPane::mouseDown(wxMouseEvent& event) 
{
	GLint view[4];
	glGetIntegerv(GL_VIEWPORT, view); 
	old_pixel_data = malloc(3 * view[2] * view[3]); // must use malloc 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
	glPixelStorei(GL_PACK_ALIGNMENT, 1); 
	//glReadBuffer( GL_BACK_LEFT ); 
	glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, old_pixel_data);
	if(PCOMMAND == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFlush();
		SwapBuffers();
		glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, old_pixel_data);	
	}
	point npoint;
	npoint.x = wxGetMousePosition().x - this->GetScreenPosition().x;
	npoint.y = wxGetMousePosition().y - this->GetScreenPosition().y;
	colist.push_back(npoint);
	if(PCOMMAND == 7){   //Fill color
		//fill_pixels = (char *)old_pixel_data;
		PIXEL_COL = view[2];
		PIXEL_ROW = view[3];
		fill_pixels = (char  *)malloc(sizeof(unsigned char)*3*view[2]*view[3]);
		glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE,fill_pixels);
		FloodFill(npoint.x,npoint.y,npoint.x-1,npoint.y,0);                //comment for identifying the fill start
		glPixelStorei(GL_PACK_ALIGNMENT, 1); 
		glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,fill_pixels);
	}
	m_statusBar1->SetStatusText("mouse down");
}
void BasicGLPane::mouseWheelMoved(wxMouseEvent& event) 
{
	m_statusBar1->SetStatusText("mouse wheel moved");
}

void BasicGLPane::mouseReleased(wxMouseEvent& event) 
{
	point npoint;
	npoint.x = wxGetMousePosition().x - this->GetScreenPosition().x;
	npoint.y = wxGetMousePosition().y - this->GetScreenPosition().y;
	point prev = colist.front();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLint view[4];
	glGetIntegerv(GL_VIEWPORT, view); 
	glDrawPixels(view[2],view[3],GL_RGB,GL_UNSIGNED_BYTE,old_pixel_data);
	free(old_pixel_data);
	if(PCOMMAND == 2)
	{
		PRP.x = -1;
		PRP.y = -1;
	}
	if(!((prev.x==npoint.x)&&(prev.y==npoint.y)))
	{
		colist.push_back(npoint);
		if(PCOMMAND!=4)
		takeAction();
		else if(colist.size()>=3)
		takeAction();
	}
	m_statusBar1->SetStatusText("released");
}
void BasicGLPane::rightClick(wxMouseEvent& event) 
{
	m_statusBar1->SetStatusText("right click");
}

void BasicGLPane::mouseLeftWindow(wxMouseEvent& event) 
{

}
void BasicGLPane::keyPressed(wxKeyEvent& event) 
{
	
}
void BasicGLPane::keyReleased(wxKeyEvent& event) 
{
}

void BasicGLPane::takeAction()
{
	glColor3f(FG_COLOR.r,FG_COLOR.g,FG_COLOR.b);
	if(PCOMMAND == 1)
	{
		if(colist.size()>=2)
		{
			m_statusBar1->SetStatusText("Drawing the line");
			point st = colist.front();
			colist.pop_front();
			point ed = colist.front();
			colist.pop_front();
			glLoadIdentity();
			//cout<<getWidth()<<" "<<getHeight()<<endl;
			glBegin(GL_LINES);
			glVertex2f(st.x,st.y);
			glVertex2f(ed.x,ed.y);
			glEnd();
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 3)
	{
		if(colist.size()>=2)
		{
			point sp = colist.front();
			colist.pop_front();
			point ep = colist.front();
			colist.pop_front();
			int g_x = max(sp.x,ep.x);
			int l_x = min(sp.x,ep.x);
			int g_y = max(sp.y,ep.y);
			int l_y = min(sp.y,ep.y);
			point center;
			center.x = (sp.x + ep.x)/2;
			center.y = (sp.y + ep.y)/2;
			int r1 = g_x - l_x;
			int r2 = g_y - l_y;
			drawEllipse(r1,r2,center);
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 4)
	{
		if(colist.size()>=3)
		{
			point p1,p2,p3;
			p1 = colist.front();
			colist.pop_front();
			p2 = colist.front();
			colist.pop_front();
			p3 = colist.front();
			colist.pop_front();
			glBegin(GL_LINE_LOOP);
			glVertex2f(p1.x,p1.y);
			glVertex2f(p2.x,p2.y);
			glVertex2f(p3.x,p3.y);
			glEnd();
			
			glFlush();
			SwapBuffers();
		}
	}
	if(PCOMMAND == 5)
	{
		if(colist.size()>=2)
		{
			point sp = colist.front();
			colist.pop_front();
			point ep = colist.front();
			colist.front();
			if((sp.x > ep.x )&&(sp.y < ep.y))
			{
				int temp = sp.x;
				sp.x = ep.x;
				ep.x = temp;
			}
			if(( sp.x < ep.x ) && (sp.y > ep.y ))
			{
				int temp = sp.y;
				sp.y = ep.y;
				ep.y = temp;
			}
			if(( sp.x > ep.x ) && (sp.y > ep.y ))
			{
				point temp = sp;
				sp  = ep;
				ep = temp;
			}
			drawRRect(sp,ep);
		}
	}
	if(PCOMMAND == 6)
	{
		point sp = colist.front();
		colist.pop_front();
		point cur = colist.front();
		colist.pop_front();
		point center;
		center.x = (sp.x + cur.x)/2;
		center.y = (sp.y + cur.y)/2;
		//translate center to origin hence cord will be
		point p1,p2,p3,p4;
		p1.x = (sp.x - center.x)*0.5 - (sp.y - center.y)*0.866;
		p1.y = (sp.x - center.x)*0.866 + (sp.y - center.y)*0.5;
		p2.x = p1.x*0.5 - p1.y*0.866;
		p2.y = p1.x*0.866 + p1.y*0.5;
		p3.x = (cur.x - center.x)*0.5 - (cur.y - center.y)*0.866;
		p3.y = (cur.x - center.x)*0.866 + (cur.y - center.y)*0.5;
		p4.x = p3.x*0.5 - p3.y*0.866;
		p4.y = p3.x*0.866 + p3.y*0.5;
		glBegin(GL_LINE_LOOP);
		glVertex2f(sp.x,sp.y);
		glVertex2f(p1.x + center.x,p1.y + center.y);
		glVertex2f(p2.x + center.x,p2.y + center.y);
		glVertex2f(cur.x,cur.y);
		glVertex2f(p3.x + center.x,p3.y + center.y);
		glVertex2f(p4.x + center.x,p4.y + center.y);
		glEnd();
		glFlush();
		SwapBuffers();
	}
	if(PCOMMAND == 7)
	{

	}
	if(PCOMMAND == 8)
	{
		glColor3f(BG_COLOR.r,BG_COLOR.g,BG_COLOR.b);
		if(colist.size()>=2){
			point p1 = colist.front();
			colist.pop_front();
			point p2 = colist.front();
			colist.pop_front();
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view); 
			//fill out 4 regions
			int xmin = min(p1.x,p2.x);
			int xmax = max(p1.x,p2.x);
			int ymin = min(p1.y,p2.y);
			int ymax = max(p1.y,p2.y);
			//make the 4 quads
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(view[2],0);
			glVertex2f(view[2],ymin);
			glVertex2f(0,ymin);
			glEnd();
			glBegin(GL_QUADS);
			glVertex2f(0,ymin);
			glVertex2f(xmin,ymin);
			glVertex2f(xmin,ymax);
			glVertex2f(0,ymax);
			glEnd();
			glBegin(GL_QUADS);
			glVertex2f(xmax,ymin);
			glVertex2f(view[2],ymin);
			glVertex2f(view[2],ymax);
			glVertex2f(xmax,ymax);
			glEnd();
			glBegin(GL_QUADS);
			glVertex2f(0,ymax);
			glVertex2f(view[2],ymax);
			glVertex2f(view[2],view[3]);
			glVertex2f(0,view[3]);
			glEnd();
			glFlush();
			SwapBuffers();
			glColor3f(FG_COLOR.r,FG_COLOR.g,FG_COLOR.b);
		}
	}
	if(PCOMMAND == 9) //erase command
	{
		
	}
	if(PCOMMAND == 10)
	{
		saveFile();
	}
	PCOMMAND = 99;
}

/***********************************************************************
 *                 FUNCTION FOR SAVING THE IMAGE FILE                  *    
 **********************************************************************/
 
void BasicGLPane::saveFile()
{
	wxFileDialog *fd = new wxFileDialog(this,("Save Image file"), "", "",
"Image files (*.png)|*.bmp", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if (fd->ShowModal() == wxID_OK)
	{
		//wxString fileName = fd->GetPath();
		GLint view[4];
		glGetIntegerv(GL_VIEWPORT, view); 
		void* pixels = malloc(3 * view[2] * view[3]); // must use malloc 
		glPixelStorei(GL_PACK_ALIGNMENT, 1); 
		glReadBuffer( GL_BACK_LEFT ); 
		glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, pixels); 
		wxImage image((int) view[2], (int) view[3]);
		image.SetData((unsigned char*) pixels);
		image = image.Mirror(false); 
		image.SaveFile(fd->GetPath(),wxBITMAP_TYPE_PNG);
	}
}
  
/**********************************************************************/

/***********************************************************************
 *              COMMANDS FOR DRAWING ROUNDED RECTANGLE                 *
 * ********************************************************************/

void BasicGLPane::CirclePoints(int x,int y,point center,int sx,int bx,int sy,int by)
{
	point p1,p2;
	p1.x = min(sx,bx);
	p1.y = min(sy,by);
	p2.x = max(sx,bx);
	p2.y = max(sy,by);
	if(((x+center.x)>=p1.x)&&((x+center.x)<=p2.x)&&((y+center.y)<=p2.y)&&((y+center.y)>=p1.y))
	glVertex2f(x+center.x,y+center.y);
	if(((y+center.x)>=p1.x)&&((y+center.x)<=p2.x)&&((x+center.y)<=p2.y)&&((x+center.y)>=p1.y))
	glVertex2f(y+center.x,x+center.y);
	if(((y+center.x)>=p1.x)&&((y+center.x)<=p2.x)&&((-x+center.y)<=p2.y)&&((-x+center.y)>=p1.y))
	glVertex2f(y+center.x,-x+center.y);
	if(((x+center.x)>=p1.x)&&((x+center.x)<=p2.x)&&((-y+center.y)<=p2.y)&&((-y+center.y)>=p1.y))
	glVertex2f(x+center.x,-y+center.y);
	if(((-x+center.x)>=p1.x)&&((-x+center.x)<=p2.x)&&((-y+center.y)<=p2.y)&&((-y+center.y)>=p1.y))
	glVertex2f(-x+center.x,-y+center.y);
	if(((-y+center.x)>=p1.x)&&((-y+center.x)<=p2.x)&&((-x+center.y)<=p2.y)&&((-x+center.y)>=p1.y))
	glVertex2f(-y+center.x,-x+center.y);
	if(((-y+center.x)>=p1.x)&&((-y+center.x)<=p2.x)&&((x+center.y)<=p2.y)&&((x+center.y)>=p1.y))
	glVertex2f(-y+center.x,x+center.y);
	if(((-x+center.x)>=p1.x)&&((-x+center.x)<=p2.x)&&((y+center.y)<=p2.y)&&((y+center.y)>=p1.y))
	glVertex2f(-x+center.x,y+center.y);
}

void BasicGLPane::drawCorner(point center,int radius,point c1,point c2)
{
	int x,y,d;
	x=0;y=radius;
	d=1-radius;
	int deltaE = 3;
	int deltaSE = (-2*radius)+5;
	CirclePoints(x,y,center,c1.x,c2.x,c2.y,c1.y);
	while(y>x){
		if(d<0){
			d=d+deltaE;
			deltaE=deltaE+2;
			deltaSE=deltaSE+2;
			x=x+1;
		}else{
			d=d+deltaSE;
			deltaE=deltaE+2;
			deltaSE=deltaSE+4;
			x=x+1;	
			y=y-1;
		}
		CirclePoints(x,y,center,c1.x,c2.x,c2.y,c1.y);
	}
}

void BasicGLPane::drawRRect(point sp,point ep)
{
	//enumerate the points in clockwise order
	point p1,p2,p3,p4,p5,p6,p7,p8;
	int r = 20;
	p1.x = sp.x ;
	p1.y = sp.y + r;
	p2.x = sp.x + r;
	p2.y = sp.y;
	p3.x = ep.x - r;
	p3.y = sp.y;
	p4.x = ep.x;
	p4.y = sp.y + r;
	p5.x = ep.x;
	p5.y = ep.y - r;
	p6.x = ep.x - r;
	p6.y = ep.y;
	p7.x = sp.x + r;
	p7.y = ep.y;
	p8.x = sp.x;
	p8.y = ep.y - r;
	//draw the corner arcs
	glBegin(GL_POINTS);
	point center;
	center.x = sp.x + r;
	center.y = sp.y + r;
	drawCorner(center,r,p1,p2);
	center.x = ep.x - r;
	center.y = sp.y + r;
	drawCorner(center,r,p3,p4);
	center.x = ep.x - r;
	center.y = ep.y - r;
	drawCorner(center,r,p5,p6);
	center.x = sp.x + r;
	center.y = ep.y - r;
	drawCorner(center,r,p7,p8);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(p2.x,p2.y);
	glVertex2f(p3.x,p3.y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(p4.x,p4.y);
	glVertex2f(p5.x,p5.y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(p6.x,p6.y);
	glVertex2f(p7.x,p7.y);
	glEnd();
	glBegin(GL_LINES);
	glVertex2f(p8.x,p8.y);
	glVertex2f(p1.x,p1.y);
	glEnd();
	glFlush();
	SwapBuffers(); 
}

/**********************************************************************/

/***********************************************************************
 *                FUNCTION TO DRAW ELLIPSE IN OPENGL                   *
 * ********************************************************************/ 

void BasicGLPane::drawEllipse(float radiusX, float radiusY,point center)
{
   int i;
 
   glBegin(GL_LINE_LOOP);
 
   for(i=0;i<360;i++)
   {
      float rad = i*DEG2RAD;
      glVertex2f(cos(rad)*radiusX + center.x,
                  sin(rad)*radiusY + center.y);
   }
 
   glEnd();
}

/***********************************************************************/

void BasicGLPane::getPixel(float *r,float *g,float *b,int x,int y)
{
	glReadBuffer( GL_FRONT );
	glReadPixels(x,PIXEL_ROW-y,1,1,GL_RED,GL_FLOAT,r);
	glReadPixels(x,PIXEL_ROW-y,1,1,GL_GREEN,GL_FLOAT,g);
	glReadPixels(x,PIXEL_ROW-y,1,1,GL_BLUE,GL_FLOAT,b);
}

void BasicGLPane::putPixel(int x,int y,float r,float g,float b)
{
	glBegin(GL_POINTS);
	glVertex2f(x,y);
	glEnd();
	glFlush();
	SwapBuffers();
}

void BasicGLPane::FloodFill(int x,int y,int r,int g,int b)				//Function for filling
{
	if((x<0)||(y<0)||(x>PIXEL_COL)||(y>PIXEL_ROW))
	return;
	float rr,gg,bb;
	getPixel(&rr,&gg,&bb,x,y);
	float rrr,ggg,bbb;
	int tx = x;
	int ty = y;
	tx = x;
	ty = y+1;
	getPixel(&rr,&gg,&bb,x,y);
	//cout << "(" << tx << "," << ty << ")" << rr << "--" << gg << "--" << bb <<endl;
	while(rr==1 && gg==1 && bb==1)
	{
		//putPixel(tx,ty,1,0,0);
		tx = x - 2 ;
		getPixel(&rrr,&ggg,&bbb,tx,ty);
		while(rrr==1 && ggg==1 && bbb==1)
		{
			putPixel(tx+2,ty,1,0,0);
			tx = tx - 1;
			getPixel(&rrr,&ggg,&bbb,tx,ty);
		}
		ty = ty + 1;
		getPixel(&rr,&gg,&bb,x,ty);
		//putPixel(tx,ty,1,0,0);
	}
	tx = x;
	ty = y+1;
	getPixel(&rr,&gg,&bb,x,y);
	//cout << "(" << tx << "," << ty << ")" << rr << "--" << gg << "--" << bb <<endl;
	while(rr==1 && gg==1 && bb==1)
	{
		putPixel(x,ty,1,0,0);
		tx = x + 1;
		getPixel(&rrr,&ggg,&bbb,tx,ty);
		while(rrr==1 && ggg==1 && bbb==1)
		{
			putPixel(tx,ty,1,0,0);
			tx = tx + 1;
			getPixel(&rrr,&ggg,&bbb,tx,ty);
		}
		ty = ty + 1;
		getPixel(&rr,&gg,&bb,x,ty);
	}
	//top left
	tx = x;
	ty = y;
	getPixel(&rr,&gg,&bb,x,y);
	//cout << "(" << tx << "," << ty << ")" << rr << "--" << gg << "--" << bb <<endl;
	while(rr==1 && gg==1 && bb==1)
	{
		putPixel(tx,ty,1,0,0);
		tx = x - 2;
		getPixel(&rrr,&ggg,&bbb,tx,ty);
		while(rrr==1 && ggg==1 && bbb==1)
		{
			putPixel(tx + 2,ty,1,0,0);
			tx = tx - 1;
			getPixel(&rrr,&ggg,&bbb,tx,ty);
		}
		ty = ty - 1;
		getPixel(&rr,&gg,&bb,x,ty);
	}
	//top right
	tx = x + 1;
	ty = y;
	getPixel(&rr,&gg,&bb,x,y);
	//cout << "(" << tx << "," << ty << ")" << rr << "--" << gg << "--" << bb <<endl;
	while(rr==1 && gg==1 && bb==1)
	{
		putPixel(x,ty,1,0,0);
		tx = x + 1;
		getPixel(&rrr,&ggg,&bbb,tx,ty);
		while(rrr==1 && ggg==1 && bbb==1)
		{
			putPixel(tx,ty,1,0,0);
			tx = tx + 1;
			getPixel(&rrr,&ggg,&bbb,tx,ty);
		}
		ty = ty - 1;
		getPixel(&rr,&gg,&bb,x,ty);
	}
}

//creating the main frame of the window
PFrame::PFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	//ShowFullScreen(true, wxFULLSCREEN_ALL);
	Maximize(true);
	//the menubar
	wxMenuBar *menuBar = new wxMenuBar;
	//the menu
	wxMenu* m_menu = new wxMenu;
	m_menu->Append(ID_NEW, "&New","New Paint File");
	m_menu->Append(ID_SAVE,"&Save","Save as PNG");
	m_menu->Append(ID_EXIT, "&Exit","Exit Program");
	//the fgcolor menu
	wxMenu* fg_menu = new wxMenu;
	fg_menu->Append(ID_FGRED,"&Red","Put Red Color");
	fg_menu->Append(ID_FGBLUE,"&Blue","Put Blue Color");
	fg_menu->Append(ID_FGGREEN,"&Green","Put Green Color");
	fg_menu->Append(ID_FGYELLOW,"&Yellow","Put Yellow Color");
	fg_menu->Append(ID_FGWHITE,"&White","Put White Color");
	fg_menu->Append(ID_FGBLACK,"&Black","Put Black Color");
	fg_menu->Append(ID_FGPURPLE,"&Purple","Put Purple Color");
	
	menuBar->Append( m_menu, "&File" );
	menuBar->Append( fg_menu, "&Foreground" );
	SetMenuBar(menuBar);
	
	m_statusBar1=CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	SetStatusText( "Welcome to Paint" );
	wxImage::AddHandler(new wxPNGHandler);
	wxBitmap nscr(wxT("/usr/local/share/canvas/chk.png"),wxBITMAP_TYPE_PNG);
	wxBitmap line(wxT("/usr/local/share/canvas/line.png"),wxBITMAP_TYPE_PNG);
	wxBitmap curve(wxT("/usr/local/share/canvas/curve.png"),wxBITMAP_TYPE_PNG);
	wxBitmap oval(wxT("/usr/local/share/canvas/oval.png"),wxBITMAP_TYPE_PNG);
	wxBitmap triangle(wxT("/usr/local/share/canvas/triangle.png"),wxBITMAP_TYPE_PNG);
	wxBitmap rrect(wxT("/usr/local/share/canvas/rrect.png"),wxBITMAP_TYPE_PNG);
	wxBitmap hexagon(wxT("/usr/local/share/canvas/hexagon.png"),wxBITMAP_TYPE_PNG);
	wxBitmap fill(wxT("/usr/local/share/canvas/fill.png"),wxBITMAP_TYPE_PNG);
	wxBitmap crop(wxT("/usr/local/share/canvas/crop.png"),wxBITMAP_TYPE_PNG);
	wxBitmap erase(wxT("/usr/local/share/canvas/erase.png"),wxBITMAP_TYPE_PNG);
	wxBitmap save(wxT("/usr/local/share/canvas/save.png"),wxBITMAP_TYPE_PNG);
	//toolbar
	wxToolBar* m_toolBar;
	m_toolBar = this->CreateToolBar( wxTB_VERTICAL, wxID_ANY ); 
	//m_toolBar->SetMargins(0,100);
	m_toolBar->AddTool( ID_NEW, wxT("New"), nscr, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_LINE, wxT("Line"), line, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_CURVE, wxT("curve"), curve, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_OVAL, wxT("oval"), oval, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_TRIANGLE, wxT("triangle"), triangle, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_RRECT, wxT("rrect"), rrect, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_HEX, wxT("hexagon"), hexagon, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_FILL, wxT("fill"), fill, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_CROP, wxT("crop"), crop, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_ERASE, wxT("erase"), erase, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar->AddTool( ID_SAVE, wxT("save"), save, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );

	m_toolBar->Realize();
	m_toolBar->Show(true);
}

/***********************************************************************
 *                           MENU FUNCTIONS                            *
 * ********************************************************************/

void PFrame::onClickRedFg(wxCommandEvent& event)
{
	//cout << "here" <<endl;
	FG_COLOR.r = 1.0;
	FG_COLOR.g = 0.0;
	FG_COLOR.b = 0.0;
}

void PFrame::onClickBlueFg(wxCommandEvent& event)
{
	FG_COLOR.r = 0.0;
	FG_COLOR.g = 0.0;
	FG_COLOR.b = 1.0;
} 

void PFrame::onClickGreenFg(wxCommandEvent& event)
{
	FG_COLOR.r = 0.0;
	FG_COLOR.g = 1.0;
	FG_COLOR.b = 0.0;
}

void PFrame::onClickYellowFg(wxCommandEvent& event)
{
	FG_COLOR.r = 1.0;
	FG_COLOR.g = 1.0;
	FG_COLOR.b = 0.0;
}

void PFrame::onClickWhiteFg(wxCommandEvent& event)
{
	FG_COLOR.r = 1.0;
	FG_COLOR.g = 1.0;
	FG_COLOR.b = 1.0;
}

void PFrame::onClickBlackFg(wxCommandEvent& event)
{
	FG_COLOR.r = 0.0;
	FG_COLOR.g = 0.0;
	FG_COLOR.b = 0.0;
}

void PFrame::onClickPurpleFg(wxCommandEvent& event)
{
	FG_COLOR.r = 0.5;
	FG_COLOR.g = 0.0;
	FG_COLOR.b = 1.0;
}

/**********************************************************************/     

void PFrame::OnToolRightClick(wxCommandEvent& event){
	int id = event.GetInt();
	PCOMMAND = id;
	//cout << id << ":::::" <<endl;
	colist.clear();
	if(id == ID_NEW )
	{
	
	}
	if(id == ID_LINE)
	{
		m_statusBar1->SetStatusText("Click on the start and end points");
	}
}

wxIMPLEMENT_APP(Paint);
	
BasicGLPane::BasicGLPane(wxFrame* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_context = new wxGLContext(this);  
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}
 
BasicGLPane::~BasicGLPane()
{
	delete m_context;
}
 
void BasicGLPane::resized(wxSizeEvent& evt)
{
//	wxGLCanvas::OnSize(evt);
 
    Refresh();
}
 
/** Inits the OpenGL viewport for drawing in 3D. */
void BasicGLPane::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
 
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
 
    glEnable(GL_COLOR_MATERIAL);
 
    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    float ratio_w_h = (float)(bottomrigth_x-topleft_x)/(float)(bottomrigth_y-topleft_y);
    gluPerspective(45 /*view angle*/, ratio_w_h, 0.1 /*clip close*/, 200 /*clip far*/);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
}
 
/** Inits the OpenGL viewport for drawing in 2D. */
void BasicGLPane::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Black Background
    glEnable(GL_TEXTURE_2D);   // textures
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 
    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
 
    gluOrtho2D(topleft_x, bottomrigth_x, bottomrigth_y, topleft_y);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
 
int BasicGLPane::getWidth()
{
    return GetSize().x;
}
 
int BasicGLPane::getHeight()
{
    return GetSize().y;
}
 
 
void BasicGLPane::render( wxPaintEvent& evt )
{
    if(!IsShown()) return;
 
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    // ------------- draw some 2D ----------------
    prepare2DViewport(0,0,getWidth(), getHeight());
    glLoadIdentity();
    // color the background
    glColor3f(BG_COLOR.r,BG_COLOR.g,BG_COLOR.b);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);
    glVertex3f(getWidth(),0,0);
    glVertex3f(getWidth(),getHeight(),0);
    glVertex3f(0,getHeight(),0);
    glEnd();
    //set the FG_COLOR
    glColor3f(FG_COLOR.r,FG_COLOR.g,FG_COLOR.b);
	
    glFlush();
    SwapBuffers();
}

