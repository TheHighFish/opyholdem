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
#include "OhHandler.h"
#include "OPython.h"
#include "OPyEdit.h"


BEGIN_MESSAGE_MAP(CNEdit, CEdit)
	ON_WM_SETFOCUS()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CNEdit, CEdit)


CNEdit::CNEdit()
{
}

CNEdit::~CNEdit()
{
}


void CNEdit::OnSetFocus(CWnd* pOldWnd){
	CEdit::OnSetFocus(pOldWnd);
}

UINT CNEdit::OnGetDlgCode() {
	// Assure we will receive Enter key
	return CEdit::OnGetDlgCode() | DLGC_WANTALLKEYS;
}

void CNEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if ( nChar == VK_RETURN ) {
		// Intercept return key and call the handler
		OnKeyEnterPressed();
	} else{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}

}

int GetSymbolChairArg(char *cmdA){
	char *p = strchr(cmdA, ',');
	char chair[2]="";

	if(p==NULL)
		return 0;
	*p='\0';
	p++;
	while(isspace(*p)) p++;
	if(*p>='0' && *p<='9'){
		chair[0]=*p;
		return atoi((char *)chair);
	}
	return -1;
}

void CNEdit::OnKeyEnterPressed() {
	
	//
	//	For some reason if cmdA is initialized to NULL
	//	The dll will crash
	//

	char *cmdA, s_cmd[70], buffer[250];
	int chair;
	CString cmdW;

	GetWindowText(cmdW);
	
	wcstombs(cmdA,(LPCTSTR) cmdW, 250);

	if(cmdA[0] == '$'){
		chair = GetSymbolChairArg(cmdA);
		if(chair == -1) {
			LogStdout("Invalid argument, usage: ${Symbol}[, chair Nº]");
		} else if(strlen(cmdA) > 25) {
			LogStdout("Invalid symbol");
		} else {
			sprintf(s_cmd,"print OpenHoldem.getSymbol(%d, \"%s\")", chair, cmdA+1);
			LogStdout(s_cmd);
			PyRun_SimpleString(s_cmd);
		}
	}else if(strcmp(cmdA, "!h") == 0 || strcmp(cmdA, "!help") == 0){
		LogStdout("!help");
		LogStdout("############   OPyHoldem   ############");
		LogStdout("#                                     #");
		LogStdout("# Ask for OpenHoldem symbols          #");
		LogStdout("# ${Symbol}[, chair Nº]               #");
		LogStdout("#                                     #");
		LogStdout("# Ex: $handsplayed                    #");
		LogStdout("#-------------------------------------#");
		LogStdout("# Running module filename and name    #");
		LogStdout("# !b (!bot)                           #");
		LogStdout("#-------------------------------------#");
		LogStdout("# OPyHoldem and Python versions       #");
		LogStdout("# !v (!version)                       #");
		LogStdout("#-------------------------------------#");
		LogStdout("# Calls your module self.stats()      #");
		LogStdout("# !s (!stats)                         #");
		LogStdout("#-------------------------------------#");
		LogStdout("# You can call any member of your     #");
		LogStdout("# module just by calling it's name    #");
		LogStdout("# or any valid python syntax.         #");
		LogStdout("#                                     #");
		LogStdout("# You need the following method in    #"); 
		LogStdout("# your python class to use this:      #");
		LogStdout("#                                     #");
		LogStdout("# def __eval(self, cmd):              #");
		LogStdout("#     exec cmd                        #");
		LogStdout("#######################################");

	}else if(strcmp(cmdA, "!s") == 0 || strcmp(cmdA, "!stats") == 0){
		LogStdout("!stats");
		PyObject *pValue = PyObject_CallMethod(pInstance, "stats", "()");
		if(pValue==NULL){
			PyErr_Print();
		}
		Py_XDECREF(pValue);
	}else if(strcmp(cmdA, "!v") == 0 || strcmp(cmdA, "!version") == 0){
		LogStdout("!version");
		sprintf(buffer, "OPyHoldem v. %s", VERSION);
		LogStdout(buffer);
		sprintf(buffer, "Python v. %s", Py_GetVersion());
		LogStdout(buffer);
	}else if(strcmp(cmdA, "!b") == 0 || strcmp(cmdA, "!bot") == 0){
		LogStdout("!bot");
		if(!IsPyRunning){
			LogStdout("No script running");
		}else{
			sprintf(buffer, "python\\%s", pLoadedScript);
			LogStdout(buffer);
			LogStdout(PyModule_GetBotName());
		}
	}else{
		//You need the following method in your python class to use this feature:
		//---------------------
		//def __eval(self, cmd):
		//    exec cmd");
		//---------------------

		LogStdout(cmdA);
		PyObject *pValue = PyObject_CallMethod(pInstance, "_eval", "(s)", cmdA);
		if(pValue==NULL){
			PyErr_Print();
		}
		Py_XDECREF(pValue);
	}
	delete cmdW;
	::free(cmdA);
	SetSel(0, -1);
	Clear();
}

//
// Keeps only a max of 1000 lines in the CEdit console
//
void CEditClearBuffer(CEdit *editArg){
	if((editArg->GetLineCount()%1000) == 0){
		editArg->SetSel(editArg->LineIndex(0), editArg->LineIndex(100), true);
		editArg->ReplaceSel(L"");
	}
}

void CEditAppend(CEdit *editArg, LPCTSTR pszTextArg ) {
	CEditClearBuffer(editArg);
	int nLength = editArg->GetWindowTextLength(); 
	editArg->SetSel(nLength, nLength);
	editArg->ReplaceSel(pszTextArg);
}

void CEditAppendLine(CEdit *editArg, LPCTSTR pszTextArg ) {
	CEditClearBuffer(editArg);
	int nLength = editArg->GetWindowTextLength(); 
	editArg->SetSel(nLength, nLength);
	editArg->ReplaceSel(pszTextArg);
	nLength = editArg->GetWindowTextLength();
	editArg->SetSel(nLength, nLength);
	editArg->ReplaceSel(L"\r\n");
}