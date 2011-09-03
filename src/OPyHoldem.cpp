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

#include "OhHandler.h"
#include "OPython.h"
#include "OPyHoldem.h"
#include "IniFile.h"

using namespace std;

extern pfgws_t m_pget_winholdem_symbol = NULL;
extern PointerTypeSendChatMessage pSendMessages = NULL;


double process_query(const char* pquery )
{
	PyObject *pValue;
	double ret;

	if(pInstance==NULL){
		LogStderr("OpenHoldem sent \"query\" but there is no script running.");
		return 0;
	}

	if (pquery==NULL)
		return 0;

	pValue = PyObject_CallMethod(pInstance, "process_message", "(ss)", "query", pquery);
	if(!IsValidPyValue(pValue)){
		return 0;
	}

	if(PyInt_Check(pValue))
		ret=(double)PyInt_AsLong(pValue);
	else
		ret=0;
	
	Py_DECREF(pValue);

	return ret;
}



double process_state(holdem_state* pstate)
{
	double ret;
	
	if(pInstance==NULL){
		LogStderr("OpenHoldem sent \"gamestate\" but there is no script running.");
		return 0;
	}

	PyObject *pState = PyDict_New();
	PyObject *pPlayer[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; 
	PyObject *pValue = NULL, *pRetValue = NULL;
	PyObject *pPlayers = PyList_New(10);

	pValue = Py_BuildValue("s", pstate->m_title);
	if(pValue == NULL){
		LogStderr("Error converting \"m_title\", returning blank");
		pValue = Py_BuildValue("s", "");			
	}
	PyDict_SetItemString(pState, "m_title", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("[d,d,d,d,d,d,d,d,d,d]", 
		pstate->m_pot[0],
		pstate->m_pot[1],
		pstate->m_pot[2],
		pstate->m_pot[3],
		pstate->m_pot[4],
		pstate->m_pot[5],
		pstate->m_pot[6],
		pstate->m_pot[7],
		pstate->m_pot[8],
		pstate->m_pot[9]
	);
	PyDict_SetItemString(pState, "m_pot", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("[B,B,B,B,B]", 
		pstate->m_cards[0],
		pstate->m_cards[1],
		pstate->m_cards[2],
		pstate->m_cards[3],
		pstate->m_cards[4]
	);
	PyDict_SetItemString(pState, "m_cards", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("B", pstate->m_is_playing);
	PyDict_SetItemString(pState, "m_is_playing", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("B", pstate->m_is_posting);
	PyDict_SetItemString(pState, "m_is_posting", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("B", pstate->m_fillerbits);
	PyDict_SetItemString(pState, "m_fillerbits", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("B", pstate->m_fillerbyte);
	PyDict_SetItemString(pState, "m_fillerbyte", pValue);
	Py_XDECREF(pValue);

	pValue = Py_BuildValue("B", pstate->m_dealer_chair);
	PyDict_SetItemString(pState, "m_dealer_chair", pValue);
	Py_XDECREF(pValue);
	
	for(int p=0; p<10; p++){
		pPlayer[p] = PyDict_New();
		
		pValue = Py_BuildValue("s", pstate->m_player[p].m_name);
		if(pValue == NULL){
			LogStderr("Error converting \"m_player[p].m_name\", returning blank");
			pValue = Py_BuildValue("s", "");
		}
		PyDict_SetItemString(pPlayer[p], "m_name", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("d", pstate->m_player[p].m_balance);
		PyDict_SetItemString(pPlayer[p], "m_balance", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("d", pstate->m_player[p].m_currentbet);
		PyDict_SetItemString(pPlayer[p], "m_currentbet", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("[B,B]", 
			pstate->m_player[p].m_cards[0], 
			pstate->m_player[p].m_cards[1]
		);
		PyDict_SetItemString(pPlayer[p], "m_cards", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("B", pstate->m_player[p].m_name_known);
		PyDict_SetItemString(pPlayer[p], "m_name_known", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("B", pstate->m_player[p].m_balance_known);
		PyDict_SetItemString(pPlayer[p], "m_balance_known", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("B", pstate->m_player[p].m_fillerbits);
		PyDict_SetItemString(pPlayer[p], "m_fillerbits", pValue);
		Py_XDECREF(pValue);

		pValue = Py_BuildValue("B", pstate->m_player[p].m_fillerbyte);
		PyDict_SetItemString(pPlayer[p], "m_fillerbyte", pValue);
		Py_XDECREF(pValue);

		PyList_SetItem(pPlayers, p, pPlayer[p]);
	}

	PyDict_SetItemString(pState, "m_player", pPlayers );
	Py_XDECREF(pPlayers);

	pRetValue = PyObject_CallMethod(pInstance, "process_message", "(sS)", "state", pState);
	Py_XDECREF(pState);

	if(pRetValue!=NULL && PyInt_Check(pRetValue))
		ret = (double)PyInt_AsLong(pValue);
	else
		ret = 0;

	if(!IsValidPyValue(pRetValue)){
		return 0;
	}

	Py_DECREF(pRetValue);

	return ret;
}

double DLL_LOAD() {
	
	//CheckPyInitFile();
	
	//Lets set the python interperter dir
	std::string PythonHomeDir = CIniFile::GetValue("PythonHomeDir", "Python", OPY_INI_FILENAME);
	if(!PythonHomeDir.empty()){
		Py_SetPythonHome((char *) PythonHomeDir.c_str());
	}
	//Allocate space for the running module name
	pLoadedScript = (char *)malloc(100 * sizeof(char));
	//Gets the last used script and tries to run it
	std::string LastScript = CIniFile::GetValue("LastScript", "OPyHoldem", OPY_INI_FILENAME);
	if(!LastScript.empty()){
		strcpy(pLoadedScript, (char *) LastScript.c_str());
		LogStdout("Click Reload to load last script");
	}else{
		LogStdout("Please open your script");
	}
	return 0;
}

double DLL_UNLOAD() {
	SendMessage(OPyFrmHwnd, WM_CLOSE, 0, 0);
	free (pLoadedScript);
	return TerminatePython();
}


WHUSER_API double process_message(const char* pmessage,	const void* param) {
	if (pmessage == NULL || param == NULL)
		return 0;

	if (strcmp(pmessage, "event") == 0)	{
		if(strcmp((char *)param, "load") == 0){
			LOADED = true;
			return DLL_LOAD();
		} else if (strcmp((char *)param, "unload") == 0){
			LOADED = false;
			return DLL_UNLOAD();
		}
	}

	if(WAIT_TO_UNLOAD){
		::Sleep(100);
		return 0;
	}
	

	if (strcmp(pmessage, "state") == 0) {
		return process_state((holdem_state*)param);

	} else if (strcmp(pmessage, "query") == 0) {
		return process_query((const char*)param);

	} else if (strcmp(pmessage, "pfgws") == 0){
		m_pget_winholdem_symbol = (pfgws_t)param;
		return 0;
	} else if (strcmp(pmessage, "pointer_for_send_chat_message") == 0){
		pSendMessages = (PointerTypeSendChatMessage)param;
		return 0;
	}

	return 0;
}