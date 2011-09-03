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

class CNEdit : public CEdit {
	DECLARE_DYNAMIC(CNEdit)

	public:
		CNEdit();
		virtual ~CNEdit();

	protected:
		virtual int OnKeyEnterPressed();
	protected:
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg UINT OnGetDlgCode();
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
		
		DECLARE_MESSAGE_MAP()
};

void CEditAppend(CEdit *editArg, LPCTSTR pszTextArg );
void CEditAppendLine(CEdit *editArg, LPCTSTR pszTextArg );