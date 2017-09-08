// copyuuid.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "copyuuid.h"

using std::wstring;
#include <cwctype>
#include <windows.h>
#include "../../lsMisc/SetClipboardText.h"
#include "../../lsMisc/OpenCommon.h"
#include "../../lsMisc/CommandLineParser.h"
#include "../../lsMisc/stdwin32/stdwin32.h"
using namespace stdwin32;
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

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CCommandLineParser parser;

	COption opUpper(L"/u");
	parser.AddOption(&opUpper);

	COption opLower(L"/l");
	parser.AddOption(&opLower);

	COption opHelp(L"/h", L"/H");
	parser.AddOption(&opHelp);

	COption opPulse(L"/pf", 1);
	parser.AddOption(&opPulse);

	COption opPulseCount(L"/pc", 1);
	parser.AddOption(&opPulseCount);

	parser.Parse();

	if(opHelp.hadOption())
	{
		wstring message = L"copyuuid.exe [/u] [/l] [/h]";
		message += L"\r\n\r\n";
		message += L"/l\r\n Output is Lowercase\r\n";
		message += L"/u\r\n Output is Uppercase (default)\r\n";
		message += L"/h\r\n Show help";
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
			ErrorExit(L"/pf must have argument.");
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
				ErrorExit(L"/pc must have value");
			}

			wstring t = opPulseCount.getFirstValue();
			pulsecount = _wtoi(t.c_str());
			if(pulsecount <= 0)
				pulsecount=1;
		}
	}

	if(parser.hadUnknownOption())
	{
		wstring message = I18N(L"Unknown option(s):\r\n");
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
			ErrorExit(I18N(L"Failed to convert to string."));
		}

		wstring clipset((LPCWSTR)pUUID);
		if(isLower)
			std::transform(clipset.begin(), clipset.end(), clipset.begin(), towlower);
		else 
			std::transform(clipset.begin(), clipset.end(), clipset.begin(), towupper);

		if(!SetClipboardText(NULL, clipset.c_str()))
		{
			ErrorExit(I18N(L"Failed to set on clipboard."));
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


	wstring message = I18N(L"New UUID has been copied to the clipboard.");


	wstring param;
	param += L"/balloonicon:";
	param += L"1";

	param += L" /title:";
	param += stdwin32::stdEncodeUrl(title);

	param += L" /icon:";
	param += stdGetFileName(stdGetModuleFileName()).c_str();

	param += L" /iconindex:0";

	param += _T(" \"") + stdEncodeUrl(message) + _T("\"");

	OpenCommon(NULL,
		file,
		param.c_str(),
		dir.c_str());

	return 0;
}
