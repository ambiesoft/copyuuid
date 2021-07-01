//Copyright (C) 2017 Ambiesoft All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//1. Redistributions of source code must retain the above copyright
//notice, this list of conditions and the following disclaimer.
//2. Redistributions in binary form must reproduce the above copyright
//notice, this list of conditions and the following disclaimer in the
//documentation and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//SUCH DAMAGE.

#include "stdafx.h"

#include "copyuuid.h"

using std::wstring;
//#include <cwctype>
#include <windows.h>
#include "../../lsMisc/SetClipboardText.h"
#include "../../lsMisc/OpenCommon.h"
#include "../../lsMisc/CommandLineParser.h"
#include "../../lsMisc/I18N.h"
#include "../../lsMisc/UrlEncode.h"
#include "../../lsMisc/stdosd/stdosd.h"

using namespace Ambiesoft::stdosd;
using namespace Ambiesoft;

#pragma comment(lib,"Rpcrt4.lib")

void ErrorExit(LPCWSTR p)
{
	MessageBox(NULL,
		p,
		APPNAME,
		MB_ICONERROR);
	exit(1);
}

int mymain()
{
	CCommandLineParser parser(L"Copy uuid onto clipboard", L"copyuuid");

	COption opUpper(L"/u", ArgCount::ArgCount_Zero, ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"Output is Uppercase(default)."));
	parser.AddOption(&opUpper);

	COption opLower(L"/l", ArgCount::ArgCount_Zero, ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"Output is Lowercase."));
	parser.AddOption(&opLower);

	bool bHelp = false;
	parser.AddOptionRange({ L"--help",L"-h",L"-help",L"/?",L"/h" },
		0,
		&bHelp,
		ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"Show Help"));

	COption opPulse(L"/pf", ExactCount::Exact_1, ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"Repeatedly(with freqency of FREQ(in millisec)) copy a new UUID onto the clipboard."));
	parser.AddOption(&opPulse);

	COption opPulseCount(L"/pc", ExactCount::Exact_1, ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"Count of repetition of /pf (default = 100)"));
	parser.AddOption(&opPulseCount);

	bool opVersion=false;
	parser.AddOptionRange({ L"--version",L"-v",L"-version",L"/v" },
		0,
		&bHelp,
		ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"Show Version"));

	wstring opLang;
	parser.AddOption(L"/lang", 1, &opLang, ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"3-letter language id for displaying text"));

	parser.Parse();

	i18nInitLangmap(NULL, opLang.empty() ? NULL : opLang.c_str());

	if(bHelp)
	{
		wstring message;
		if(true)
		{
			message = parser.getHelpMessage();
		}
		else
		{
			message = L"copyuuid.exe [/h] [/u] [/l] [/pf FREQ] [/pc COUNT]";
			message += L"\r\n\r\n";

			message += L"/l\r\n  ";
			message += I18N(L"Output is Lowercase.");
			message += L"\r\n";

			message += L"/u\r\n  ";
			message += I18N(L"Output is Uppercase(default).");
			message += L"\r\n";

			message += L"/pf FREQ\r\n  ";
			message += I18N(L"Repeatedly(with freqency of FREQ(in millisec)) copy a new UUID onto the clipboard.");
			message += L"\r\n";

			message += L"/pc COUNT\r\n  ";
			message += I18N(L"Count of repetition of /pf (default = 100)");
			message += L"\r\n";

			message += L"/lang LANG\r\n  ";
			message += I18N(L"3-letter language id for displaying text");
			message += L"\r\n";

			message += L"/v\r\n  ";
			message += I18N(L"Show version");
			message += L"\r\n";

			message += L"/h\r\n  ";
			message += I18N(L"Show help");
			message += L"\r\n";
		}
		MessageBox(NULL,
			message.c_str(),
			APPNAME L" HELP",
			MB_ICONINFORMATION);
		return 0;
	}
	if(opVersion)
	{
		wstring message = stdFormat(L"%s version %s", APPNAME, VERSION);
		MessageBox(NULL,
			message.c_str(),
			APPNAME,
			MB_ICONINFORMATION);
		return 0;
	}
	bool ispulse = opPulse.hadOption();
	int freq=1000;
	int pulsecount=100;
	if(ispulse)
	{
		if(!opPulse.hadValue())
		{
			ErrorExit(I18N(L"/pf must have argument FREQ."));
		}
		wstring freqt = opPulse.getFirstValue();
		freq = _wtoi(freqt.c_str());
		if(freq<0)
		{
			freq=-freq;
		}
		if(freq==0)
			freq=1;


		if(opPulseCount.hadOption())
		{
			if(!opPulseCount.hadValue())
			{
				ErrorExit(I18N(L"/pc must have argument COUNT"));
			}

			wstring t = opPulseCount.getFirstValue();
			pulsecount = _wtoi(t.c_str());
			if(pulsecount <= 0)
				pulsecount=1;
		}
	}

	if(parser.hadUnknownOption())
	{
		wstring message = I18N(L"Unknown option(s):");
		message += L"\r\n";
		message += parser.getUnknowOptionStrings();
		ErrorExit(message.c_str());
	}

	bool isLower = opLower.hadOption();
	do
	{
		UUID uuid;
		if(RPC_S_OK != UuidCreate(&uuid))
		{
			ErrorExit(I18N(L"Failed to create UUID."));
		}

		RPC_WSTR pUUID = NULL;
		if(RPC_S_OK != UuidToStringW(&uuid, &pUUID))
		{
			ErrorExit(I18N(L"Failed to convert UUID to string."));
		}

		wstring clipset((LPCWSTR)pUUID);
		if(isLower)
			std::transform(clipset.begin(), clipset.end(), clipset.begin(), towlower);
		else 
			std::transform(clipset.begin(), clipset.end(), clipset.begin(), towupper);

		if(!SetClipboardText(NULL, clipset.c_str()))
		{
			ErrorExit(I18N(L"Failed to copy onto the clipboard."));
		}
		RpcStringFreeW(&pUUID);

		if(!ispulse)
		{
			break;
		}
		else
		{
			Sleep(freq);
		}
	} while(pulsecount--);



	wstring dir;
	TCHAR file[MAX_PATH];
	GetModuleFileName(NULL, file, MAX_PATH);
	*_tcsrchr(file, _T('\\')) = 0;
	dir=file;
	lstrcat(file, _T("\\showballoon.exe"));


	wstring title;
	title = APPNAME;

	//time_t t;
	//t = time(NULL);
	//struct tm tmTemp = {0};
	//if(0 != localtime_s(&tmTemp, &t))
	//{
	//	ErrorExit(L"localtime_s()");
	//}

	//wchar_t szTimeBuff[256];
	//if (!wcsftime(szTimeBuff, sizeof(szTimeBuff)/sizeof(szTimeBuff[0]), L"%H:%M:%S", &tmTemp))
	//{
	//	szTimeBuff[0] = L'\0';
	//}
	//else
	//{
	//	title += L" ";
	//	title += szTimeBuff;
	//}


	wstring message = I18N(L"New UUID has been copied onto the clipboard.");


	wstring param;
	param += L"/balloonicon:";
	param += L"1";

	param += L" /title:";
	param += UrlEncodeStd(title.c_str());

	param += L" /icon:";
	param += stdGetFileName(stdGetModuleFileName()).c_str();

	param += L" /iconindex:0";

	param += _T(" \"") + UrlEncodeStd(message.c_str()) + _T("\"");

	OpenCommon(NULL,
		file,
		param.c_str(),
		dir.c_str());

	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return mymain();
}
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR    lpCmdLine,
                     int       nCmdShow)
 {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    return mymain();
 }
