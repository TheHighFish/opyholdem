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

#include <stdafx.h>
#include "OhHandler.h"
#include "IniFile.h"

extern bool WAIT_TO_UNLOAD = false;
extern HWND OhHwnd = NULL;
extern HWND OPyFrmHwnd = NULL;

//
// Will search all process's to find OpenHoldem handler
//
BOOL CALLBACK OPyEnumProc(HWND hWnd, LPARAM lParam){
	DWORD pidwin;
    
    GetWindowThreadProcessId(hWnd, &pidwin);
	
    if(pidwin == GetCurrentProcessId() && GetParent(hWnd) == NULL)
    {
	    OhHwnd = hWnd;
        return FALSE;
    }
    return TRUE;
}

void FishOhHwnd(void){
    EnumWindows(OPyEnumProc, 0);
}

void ErrMsg(LPCWSTR msg){
	MessageBoxW(OPyFrmHwnd, msg, L"OPyHoldem Error", MB_OK | MB_ICONERROR);
}

void InfoMsg(LPCWSTR msg){
	MessageBoxW(OPyFrmHwnd, msg, L"OPyHoldem Information", MB_OK | MB_ICONINFORMATION);
}

void WarnMsg(LPCWSTR msg){
	MessageBoxW(OPyFrmHwnd, msg, L"OPyHoldem Warning", MB_OK | MB_ICONWARNING);
}

void SendDllUnloadMsg(){
	//Send comand was my first choice but, oh well	
	//Second choice, take focus and send F4
	if(!WAIT_TO_UNLOAD){
		SetActiveWindow(OhHwnd);
		SetFocus(OhHwnd);

		keybd_event(VK_F4, 0, 0, 0);
		::Sleep(500);
		keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);
		
		WAIT_TO_UNLOAD = true;
	}
}

void LogStderr(char *LogStr){
	PostMessage(OPyFrmHwnd, ON_PY_LOG_MESSAGE_STDERR, 0, (LPARAM)new string(LogStr));
}

void LogStdout(char *LogStr){
	PostMessage(OPyFrmHwnd, ON_PY_LOG_MESSAGE_STDOUT, 0, (LPARAM)new string(LogStr));
}
