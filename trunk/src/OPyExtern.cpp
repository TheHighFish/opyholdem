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
#include <iostream>
#include <fstream>
#include "IniFile.h"
#include "OPyExtern.h"
#include <sys/stat.h>

using namespace std;

char *ModuleDir(){
	char result[MAX_PATH];
	char *buffer = (char *)malloc(MAX_PATH * sizeof(char));

	GetModuleFileNameA(0, result, MAX_PATH);
	*strrchr(result, '\\')='\0';
	sprintf(buffer, "%s\\%s",result , "python");
	return buffer;
}

bool FileExist(char *File) {
	struct stat stFileInfo;

	if(stat(File,&stFileInfo) == 0) {
		return true;
	} 

	return false;
}

bool CheckPyInitFile(){

	if(!FileExist("python\\__init__.py")) {
		if(!FileExist("python")){
			::CreateDirectory(L"python", (LPSECURITY_ATTRIBUTES) NULL);
		}
		ofstream pyInit;
		pyInit.open ("python\\__init__.py");
		pyInit << "# Dummy file to make this directory a package";
		pyInit.close();
		return true;
	}
	return false;
}

bool CheckIniFile(){
	if(FileExist((char *)OPY_INI_FILENAME)){
		return false;
	}

	CIniFile::Create(OPY_INI_FILENAME);
	CIniFile::AddSection("Python", OPY_INI_FILENAME);
	CIniFile::AddSection("OPyHoldem", OPY_INI_FILENAME);
	CIniFile::AddSection("WindowManager", OPY_INI_FILENAME);
	CIniFile::SetSectionComments("#PythonHomeDir can be left in blank for default dir", "Python", OPY_INI_FILENAME);
	CIniFile::SetValue("PythonHomeDir", "", "Python", OPY_INI_FILENAME);
	CIniFile::SetValue("LastScript", "", "OPyHoldem", OPY_INI_FILENAME);
	return true;
}