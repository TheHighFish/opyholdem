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
#pragma once

#include "OPyExtern.h"
#include "OPyEdit.h"


class OPyFrame : public CFrameWnd {
	protected:
		CEdit OPyConsole;
		CNEdit OPyCmd;
		CToolBar m_wndToolBar;
		CImageList m_ToolbarImagesDisabled;
		CImageList m_ToolbarImagesHot;
		CImageList m_ToolbarImages;
	
	public:
		OPyFrame();
		void OnFileOpen();
		void OnReload();
		void OnAboutDialog();
		void OnPyStop();
	
	private:
		void AttachToolbarImages (UINT inNormalImageID, UINT inDisabledImageID, UINT inHotImageID);

	protected:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnClose();
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg LRESULT OnPyLogStderr(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnPyLogStdout(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnPyModuleLoad(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnPyModuleUnload(WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()
};

static void	ReplaceBackgroundColor (CBitmap& ioBM);
static void	MakeToolbarImageList (UINT inBitmapID, CImageList &outImageList);