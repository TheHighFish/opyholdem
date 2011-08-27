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
#include <Python.h>
#include "OPyEdit.h"
#include "OPyFrame.h"
#include "OhHandler.h"
#include "OPyWinApp.h"
#include "IniFile.h"


OPyWinApp app;


BOOL OPyWinApp::InitInstance(){

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	
	//
	// Lets check if the .ini file exists
	// If not the function will create it
	//
	CheckIniFile();

	//
	// Tries  to capture OpenHoldem handler
	//
	for(int i=0;OhHwnd == NULL || i>20;i++,::Sleep(250)){
		FishOhHwnd();
	}
	if(OhHwnd == NULL){
		WarnMsg(L"Failed to discover OpenHoldem HWND.");
	}
	
    //
	//For some reason if m_pMainWnd is used 
	//whenever the dll unloads, openholdem
    //will be placed behind all windows
    //

	OPyFrame *pMainWnd = new OPyFrame;
	pMainWnd->ShowWindow(SW_SHOW);

	//Global OPyFrmHwnd Now with Hwnd from OPyFrm
	OPyFrmHwnd=pMainWnd->GetSafeHwnd();

	return true;
}

BOOL OPyWinApp::ExitInstance(){
	return CWinApp::ExitInstance();
}
