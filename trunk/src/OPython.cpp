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
#include "IniFile.h"

using namespace std;

const char *PyClassToCall="Main";

PyObject *pName = NULL, *pModule = NULL, *pDict = NULL, *pClass = NULL;
PyGILState_STATE gstate;

extern bool IsPyRunning = false;

extern PyObject *pInstance = NULL;
extern char *pLoadedScript=NULL;

char* basename (char* path){
	char *pch=strrchr(path, '\\');
	
	if(pch==NULL) {
		return "OPyHoldem.dll";
	} else {
		return ++pch;
	}
}


void SetPyArgv() {
	char result[MAX_PATH];

	GetModuleFileNameA(0, result, MAX_PATH);

	char *argv[] = {basename(result)};
	PySys_SetArgvEx(1, argv, 0);
}

PyObject* OPy_stdout(PyObject* self, PyObject* pArgs) {
	char* LogStr = NULL;

	if (!PyArg_ParseTuple(pArgs, "s", &LogStr)) 
		return NULL;
	
	LogStdout(LogStr);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject* OPy_stderr(PyObject* self, PyObject* pArgs) {
	char* LogStr = NULL;

	if (!PyArg_ParseTuple(pArgs, "s", &LogStr)) 
		return NULL;
	
	LogStderr(LogStr);

	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef OPyHoldemDllMethods[] = {
	{"_stdout", OPy_stdout, METH_VARARGS, "Captures stdout"},
	{"_stderr", OPy_stderr, METH_VARARGS, "Captures stderr"},
	{NULL, NULL, 0, NULL}
};


static PyObject* getSymbol(PyObject *self, PyObject *args) {
	char *symbol;
	int chair;
	bool dupa;

    if(!PyArg_ParseTuple(args, "is", &chair, &symbol))
        return NULL;
		
	return Py_BuildValue("d", m_pget_winholdem_symbol(chair, symbol, dupa));
}

static PyMethodDef OhMethods[] = {
    {"getSymbol", getSymbol, METH_VARARGS, "Get OpenHoldem symbol"},
    {NULL, NULL, 0, NULL}
};

bool LoadPyInstance(const char *PyMainFile){
	char Buffer[100]={0};
	
	sprintf(Buffer, "python\\%s", PyMainFile);
	if(!FileExist((char *)Buffer)){
		sprintf(Buffer,"The script file dosen't exist\n%s", PyMainFile);
		CString msg(Buffer);
		ErrMsg(msg);
		return false;
	}

	//Saves the module name to enable script reload
	if(strcmp(pLoadedScript, PyMainFile) != 0){
		strcpy(pLoadedScript, (char *)PyMainFile);
	}

	memset(Buffer, 0, sizeof(Buffer));
	sprintf(Buffer, "python.%s", PyMainFile);
	
	//Elimitanes the file termination to be loaded has a module
	*strrchr((char *)Buffer, '.')='\0';

	pName = PyString_FromString(Buffer);

	if(pName == NULL){
		PyErr_Print();
		ErrMsg(
			L"Failed to load Python script file,\n"
			L"Please check you settings in python.ini\n"
			L"The script will now stop"
		);
		TerminatePython();
		return 0;
	}
	pModule = PyImport_Import(pName);
	if(pModule == NULL){
		PyErr_Print();
		ErrMsg(
			L"The loaded python module contain errors.\n"
			L"Please check the log for detailed information on the error.\n"
			L"The script will now stop"
		);
		TerminatePython();
		return 0;
	}
	pDict = PyModule_GetDict(pModule);

	pClass = PyDict_GetItemString(pDict, PyClassToCall);

	if (PyCallable_Check(pClass)) {
		pInstance = PyObject_CallObject(pClass, NULL); 
	}else{
		PyErr_Print();
		ErrMsg(
			L"OPyHoldem class not found in script file.\n"
			L"The script will now stop"
		);
		TerminatePython();
		return 0;
	}
	PostMessage(OPyFrmHwnd, ON_PY_MODULE_LOAD, 0, 0);
	return 0;
}

void DestroyPyInstance(){

	Py_XDECREF(pInstance);
	Py_XDECREF(pModule);
	Py_XDECREF(pName);
	
	pModule=NULL;
	pName=NULL;
	pInstance=NULL;
	pDict=NULL;
	pClass=NULL;
}

bool IsValidPyValue(PyObject *pValue){
	if(pValue==NULL){
		PyErr_Print();
		ErrMsg(
			L"There is an error in your script\n"
			L"The script will now stop"
		);
		TerminatePython();
		return false;
	}
	return true;
}

int ReInitializePython(const char *PyMainFile){
	if(InitializePython())
		return LoadPyInstance(PyMainFile);
	return 0;
}

int InitializePython(){
	//If py already running it will stop it
	TerminatePython();
		
	try {
		//Initializes the python API
		Py_Initialize();
		IsPyRunning=true;

		SetPyArgv();
		gstate = PyGILState_Ensure();
		

		//Exports the OpenHoldem module
		//Makes possible to call OpenHoldem.getSymbol(chair, symbol)
		Py_InitModule("OpenHoldem", OhMethods);
		Py_InitModule("OPyHoldemDll", OPyHoldemDllMethods);
		
		//Forces the python stdout and stderr to be handled by the dll
		 PyRun_SimpleString(
			"import OPyHoldemDll\n"
			"import OpenHoldem\n"
			"import sys\n"
			"class StdoutCatcher:\n"
			"\tdef write(self, str):\n"
			"\t\tif (len(str.strip())):\n"
			"\t\t\tOPyHoldemDll._stdout('>> %s'%str)\n"
			"class StderrCatcher:\n"
			"\tdef write(self, str):\n"
			"\t\tif (len(str.strip())):\n"
			"\t\t\tOPyHoldemDll._stderr(str)\n"
			"sys.stdout = StdoutCatcher()\n"
			"sys.stderr = StderrCatcher()\n"
		);
		
	} catch(...) {
		PyErr_Print();
		ErrMsg(
			L"Error initializing the Python interpreter\n"
			L"The script will now stop"
		);
		TerminatePython();
		return 0;
	}

	return 1;
}

char *PyModule_GetBotName() {
    PyObject *nameobj;
	
	if(pDict == NULL)
		return NULL;

    if ((nameobj = PyDict_GetItemString(pDict, "__BotName__")) == NULL ||
        !PyString_Check(nameobj))
    {
        LogStderr("Nameless bot");
		LogStderr("Please set __BotName__ in your script file");
		LogStderr("Ex: __BotName__ = \"This is my bot name\"");
		
		return "__Nameless__";
    }
    return PyString_AsString(nameobj);
}


int TerminatePython(){
	if(!IsPyRunning){
		return 0;
	}

	DestroyPyInstance();
	
	PyGILState_Release(gstate);
    Py_Finalize();

	IsPyRunning=false;
	PostMessage(OPyFrmHwnd, ON_PY_MODULE_UNLOAD, 0, 0);
	return 0;
}