/*                                                                          #
#  OPyHoldem - Python dll module for OpenHoldem                             #
#  =======================================================================  #
#  Copyright (C) 2011           Satake ' <satakecode@gmail.com>             #
#                                                                           #
#    This program is free software: you can redistribute it and/or modify   #
#    it under the terms of the GNU General Public License as published by   #
#    the Free Software Foundation, either version 3 of the License, or      #
#    (at your option) any later version.                                    #
#                                                                           #
#    This program is distributed in the hope that it will be useful,        #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#    GNU General Public License for more details.                           #
#                                                                           #
#    You should have received a copy of the GNU General Public License      #
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.  #
#                                                                           #
#    http://code.google.com/p/opyholdem/                                    #
#                                                                           #
###########################################################################*/


#include "stdafx.h"
#include <string>
#include <Python.h>
#include "OPyEdit.h"
#include "OPyFrame.h"
#include "OhHandler.h"
#include "OPython.h"
#include "IniFile.h"


using namespace std;
const int OPyCmdSize = 18;
int ToolbarHeight = 0;

 // these constants represent the dimensions and number of buttons in
 // the default MFC-generated toolbar. If you need something different,
 // feel free to change them. For extra credit, you can load the
 // toolbar's existing image list at runtime and copy the parameters from
 // there.
static const int	kImageWidth (16);
static const int	kImageHeight (16);
static const int	kNumImages (4);

static const UINT	kToolBarBitDepth (ILC_COLOR24);

 // this color will be treated as transparent in the loaded bitmaps --
 // in other words, any pixel of this color will be set at runtime to
 // the user's button color. The Visual Studio toolbar editor defaults
 // to 192, 192, 192 (light gray).
static const RGBTRIPLE	kBackgroundColor = {189, 189, 189};



//IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(OPyFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_MESSAGE(ON_PY_LOG_MESSAGE_STDERR, OPyFrame::OnPyLogStderr)
	ON_MESSAGE(ON_PY_LOG_MESSAGE_STDOUT, OPyFrame::OnPyLogStdout)
	ON_MESSAGE(ON_PY_MODULE_LOAD, OPyFrame::OnPyModuleLoad)
	ON_MESSAGE(ON_PY_MODULE_UNLOAD, OPyFrame::OnPyModuleUnload)
	ON_COMMAND(OPY_ID_FILE_OPEN, OPyFrame::OnFileOpen)
	ON_COMMAND(OPY_ID_FILE_RELOAD, OPyFrame::OnReload)
    ON_COMMAND(OPY_ID_PY_INSTANCE_DESTROY, OPyFrame::OnPyStop)
	ON_COMMAND(OPY_IDD_ABOUTBOX, OPyFrame::OnAboutDialog)

END_MESSAGE_MAP()


void OPyFrame::OnFileOpen(){

   	CFileDialog OpenPyDlg(
		true, 
		NULL, 
		NULL, 
		OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST, 
		L"Python script(*.py;*.pyo;*.pyc)|*.py;*.pyo;*.pyc||", 
		this
	);

	OpenPyDlg.m_pOFN->lpstrTitle=L"OPyHoldem open script";
	
	char *pDir = ModuleDir();
	CString dir(pDir);
	OpenPyDlg.m_pOFN->lpstrInitialDir=dir;
	free (pDir);
	
		
	if(OpenPyDlg.DoModal()==IDOK){
		CT2CA PathNameAnsi (OpenPyDlg.GetPathName());
		std::string PathName(PathNameAnsi);
		char *pch=strrchr((char *)PathName.c_str(), '\\');
		pch++;
		std::string ModuleFName(pch);
		CIniFile::SetValue("LastScript", ModuleFName, "OPyHoldem", OPY_INI_FILENAME);
		ReInitializePython(pch);	
	}

	delete OpenPyDlg;
}

void OPyFrame::OnReload(){
	if(pLoadedScript == NULL){
		ErrMsg(L"No script running to reload");
	}
	ReInitializePython(pLoadedScript);
}

void OPyFrame::OnPyStop(){
   TerminatePython();
}

void OPyFrame::OnAboutDialog(){
   CDialog AboutDlg(OPY_IDD_ABOUTBOX, this);
   AboutDlg.DoModal();
   delete AboutDlg;
}

OPyFrame::OPyFrame(){
	CWnd *OhCWnd = CWnd::FromHandle(OhHwnd);
	
	RECT OPyFrmR;
	HINSTANCE aa=AfxGetInstanceHandle();
	LPCTSTR theClass=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_OWNDC|CS_DBLCLKS,0,0,LoadIcon(aa,MAKEINTRESOURCE(IDR_MY_ICON)));

	std::string FrmR_x = CIniFile::GetValue("CMainFrame_x", "WindowManager", OPY_INI_FILENAME);
	if(!FrmR_x.empty()){
		std::string FrmR_y = CIniFile::GetValue("CMainFrame_y", "WindowManager", OPY_INI_FILENAME);
		std::string FrmR_cx = CIniFile::GetValue("CMainFrame_cx", "WindowManager", OPY_INI_FILENAME);
		std::string FrmR_cy = CIniFile::GetValue("CMainFrame_cy", "WindowManager", OPY_INI_FILENAME);
		OPyFrmR.left = atoi(FrmR_x.c_str());
		OPyFrmR.top = atoi(FrmR_y.c_str());	
		OPyFrmR.right = atoi(FrmR_cx.c_str());
		OPyFrmR.bottom = atoi(FrmR_cy.c_str());
	}else{
		OPyFrmR.left = 0;
		OPyFrmR.top = 0;	
		OPyFrmR.right = 500;
		OPyFrmR.bottom = 300;
	}

    Create(
		theClass,
		L"OPyHoldem :: Not running",
		WS_POPUP | WS_CAPTION | WS_SIZEBOX,
		OPyFrmR,
		OhCWnd,
		NULL,
		WS_EX_TOOLWINDOW
	); 

}

int OPyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct){

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_wndToolBar.CreateEx(
			this, 
			TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS  | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
		) || !m_wndToolBar.LoadToolBar(IDB_TOOLBAR) ) 
	{
      ErrMsg(L"Failed to create the toolbar.");
   }
	
   // attach the hicolor bitmaps to the toolbar
	AttachToolbarImages (IDB_HICOLOR_TOOLBAR,
						 IDB_HICOLOR_TOOLBAR_DISABLED,
						 IDB_HICOLOR_TOOLBAR_HOT);


	/*m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/
	
	//Will set the ToolbarHeight
	CRect ToolBarR;
	m_wndToolBar.GetItemRect(0, ToolBarR);
	ToolbarHeight=ToolBarR.Height()+2;

	CFont *font = new CFont;
	
	VERIFY(font->CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Consolas")
	));

	RECT OPyFrmCliR;
	GetClientRect(&OPyFrmCliR);

	OPyConsole.Create(
		WS_CHILD | ES_READONLY | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
		CRect(0, ToolbarHeight, OPyFrmCliR.right, OPyFrmCliR.bottom-OPyCmdSize-ToolbarHeight), 
		this, 
		1
	);
	OPyConsole.SetLimitText(MAX_CONSOLE_CHR);
	OPyConsole.SetFont(font);
	OPyConsole.LineScroll(1, 0);

	OPyCmd.Create(
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		CRect(0, OPyFrmCliR.bottom-OPyCmdSize, OPyFrmCliR.right, OPyFrmCliR.bottom), 
		this, 
		1
	);

	OPyCmd.SetFont(font);

	return 0;
}

//
// On main frame resize action
//
void OPyFrame::OnSize(UINT nType, int cx, int cy){
	CFrameWnd::OnSize(nType, cx, cy-OPyCmdSize);
	
	OPyConsole.MoveWindow(0, ToolbarHeight, cx, cy-OPyCmdSize-ToolbarHeight);
	OPyCmd.MoveWindow(0, cy-OPyCmdSize, cx, OPyCmdSize);
}


//
// Python stdout message handler
//
LRESULT OPyFrame::OnPyLogStdout(WPARAM wParam, LPARAM lParam){
	//You can do whatever with the Python stdout here
	string *msg = reinterpret_cast<string*>(lParam);
	const char *Cmsg = msg->c_str();
	CString CSmsg(Cmsg);

	if(Cmsg[strlen(Cmsg)-1] == '\n') {
		CEditAppend(&OPyConsole, CSmsg);
	}else{
		CEditAppendLine(&OPyConsole, CSmsg);
	}
	delete msg, CSmsg;
	return true;
}

//
// Python stderr message handler
//
LRESULT OPyFrame::OnPyLogStderr(WPARAM wParam, LPARAM lParam){
	//You can do whatever with the Python stderr here
	string *msg = reinterpret_cast<string*>(lParam);
	const char *Cmsg = msg->c_str();
	CString CSmsg(Cmsg);

	if(Cmsg[strlen(Cmsg)-1] == '\n') {
		CEditAppend(&OPyConsole, CSmsg);
	}else{
		CEditAppendLine(&OPyConsole, CSmsg);
	}
	delete msg, CSmsg;
	return true;
}

//
// On python module load message
//

LRESULT OPyFrame::OnPyModuleLoad(WPARAM wParam, LPARAM lParam){

	char buffer[250];
	char *BotName = PyModule_GetBotName();
	sprintf(buffer, "OPyHoldem :: Running :: %s", BotName);
	CString title((char *)buffer);
	SetWindowText(title);
	return true;
}

//
// On python module unload message
//

LRESULT OPyFrame::OnPyModuleUnload(WPARAM wParam, LPARAM lParam){
	SetWindowText(L"OPyHoldem :: Not running");
	return true;
}


//
// Main frame On Close action
// Save what you can and get out
//
void OPyFrame::OnClose(){
	std::stringstream tmp;
	
	CRect r;
	GetWindowRect(&r);

	CIniFile::SetValue("CMainFrame_x", ItS(r.left) ,"WindowManager", OPY_INI_FILENAME);
	tmp.str(std::string());
	CIniFile::SetValue("CMainFrame_y", ItS(r.top),"WindowManager", OPY_INI_FILENAME);
	tmp.str(std::string());
	CIniFile::SetValue("CMainFrame_cx", ItS(r.right),"WindowManager", OPY_INI_FILENAME);
	tmp.str(std::string());
	CIniFile::SetValue("CMainFrame_cy", ItS(r.bottom),"WindowManager", OPY_INI_FILENAME);
	
	CFrameWnd::OnClose();
}



 // load the high color toolbar images and attach them to m_wndToolBar
void OPyFrame::AttachToolbarImages (UINT inNormalImageID,
									  UINT inDisabledImageID,
									  UINT inHotImageID)
{
	 // make high-color image lists for each of the bitmaps
	::MakeToolbarImageList (inNormalImageID, m_ToolbarImages);
	::MakeToolbarImageList (inDisabledImageID, m_ToolbarImagesDisabled);
	::MakeToolbarImageList (inHotImageID, m_ToolbarImagesHot);

	 // get the toolbar control associated with the CToolbar object
	CToolBarCtrl&	barCtrl = m_wndToolBar.GetToolBarCtrl();

	 // attach the image lists to the toolbar control
	barCtrl.SetImageList (&m_ToolbarImages);
	barCtrl.SetDisabledImageList (&m_ToolbarImagesDisabled);
	barCtrl.SetHotImageList (&m_ToolbarImagesHot);
}

 // find every pixel of the default background color in the specified
 // bitmap and set each one to the user's button color.
static void	ReplaceBackgroundColor (CBitmap& ioBM)
{
	 // figure out how many pixels there are in the bitmap
	BITMAP		bmInfo;

	VERIFY (ioBM.GetBitmap (&bmInfo));
	
	 // add support for additional bit depths here if you choose
	VERIFY (bmInfo.bmBitsPixel == 24);
	VERIFY (bmInfo.bmWidthBytes == (bmInfo.bmWidth * 3));

	const UINT		numPixels (bmInfo.bmHeight * bmInfo.bmWidth);

	 // get a pointer to the pixels
    DIBSECTION  ds;

    VERIFY (ioBM.GetObject (sizeof (DIBSECTION), &ds) == sizeof (DIBSECTION));

	RGBTRIPLE*		pixels = reinterpret_cast<RGBTRIPLE*>(ds.dsBm.bmBits);
	VERIFY (pixels != NULL);

	 // get the user's preferred button color from the system
	const COLORREF		buttonColor (::GetSysColor (COLOR_BTNFACE));
	const RGBTRIPLE		userBackgroundColor = {
		GetBValue (buttonColor), GetGValue (buttonColor), GetRValue (buttonColor)};

	 // search through the pixels, substituting the user's button
	 // color for any pixel that has the magic background color
	for (UINT i = 0; i < numPixels; ++i)
	{
		if (pixels [i].rgbtBlue == kBackgroundColor.rgbtBlue &&
			pixels [i].rgbtGreen == kBackgroundColor.rgbtGreen &&
			pixels [i].rgbtRed == kBackgroundColor.rgbtRed)
		{
			pixels [i] = userBackgroundColor;
		}
	}
}


 // create an image list for the specified BMP resource
static void	MakeToolbarImageList (UINT			inBitmapID,
								  CImageList&	outImageList)
{
	CBitmap		bm;

	 // if we use CBitmap::LoadBitmap() to load the bitmap, the colors
	 // will be reduced to the bit depth of the main screen and we won't
	 // be able to access the pixels directly. To avoid those problems,
	 // we'll load the bitmap as a DIBSection instead and attach the
	 // DIBSection to the CBitmap.
	VERIFY (bm.Attach (::LoadImage (::AfxFindResourceHandle(
		MAKEINTRESOURCE (inBitmapID), RT_BITMAP),
		MAKEINTRESOURCE (inBitmapID), IMAGE_BITMAP, 0, 0,
		(LR_DEFAULTSIZE | LR_CREATEDIBSECTION))));

	 // replace the specified color in the bitmap with the user's
	 // button color
	::ReplaceBackgroundColor (bm);

	 // create a 24 bit image list with the same dimensions and number
	 // of buttons as the toolbar
	VERIFY (outImageList.Create (
		kImageWidth, kImageHeight, kToolBarBitDepth, kNumImages, 0));

	 // attach the bitmap to the image list
	VERIFY (outImageList.Add (&bm, RGB (0, 0, 0)) != -1);
}

