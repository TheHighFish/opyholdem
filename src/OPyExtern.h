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

#include <Python.h>

#define VERSION "0.1.1b"

#define OPY_INI_FILENAME  "OPyHoldem.ini"

#define MAX_CONSOLE_CHR   524288 //512kb

typedef double (*pfgws_t)( int c, const char* psym, bool& iserr );
typedef void (*PointerTypeSendChatMessage)(char* message);

extern bool WAIT_TO_UNLOAD;
//OpenHoldem handler
extern HWND OhHwnd;
//OPyHoldem frame handler
extern HWND OPyFrmHwnd;
//Running python module instance
extern PyObject *pInstance;
//Running python module filename
extern char *pLoadedScript;
//Flag for python interpreter
extern bool IsPyRunning;

extern PointerTypeSendChatMessage pSendMessages;
extern pfgws_t m_pget_winholdem_symbol;

bool CheckPyInitFile();
bool CheckIniFile();
bool FileExist(char *File);
char *ModuleDir();