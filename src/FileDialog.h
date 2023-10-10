#pragma once

#include<iostream>
#include<cstring>
#include<string>
#include<string.h>
#include <shlobj.h>
#include <locale>
//#include <codecvt>

#define STRICT_TYPED_ITEMIDS

namespace FileDlg 
{
	char selectedFilePath[1024] = "";

	std::string wideCharToMultiByte(wchar_t* pWCStrKey)
	{
		//第一次调用确认转换后单字节字符串的长度，用于开辟空间
		int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
		char* pCStrKey = new char[pSize + 1];
		//第二次调用将双字节字符串转换成单字节字符串
		WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
		pCStrKey[pSize] = '\0';

		//如果想要转换成string，直接赋值即可
		string pKey = pCStrKey;
		return pKey;
	}


	std::string GetFileDialog()
	{
		OPENFILENAME ofn;       // common dialog box structure
		LPWSTR szFile = NULL;       // buffer for file name
		wchar_t szfile[1024] = L"";
		HWND hwnd = NULL;              // owner window
		HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		//ofn.lpstrFile = szFile;
		ofn.lpstrFile = szfile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.

		ofn.nMaxFile = sizeof(szfile);
		ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		//ofn.lpstrInitialDir = NULL;
		ofn.lpstrInitialDir = TEXT("D:\\");//默认的文件路径
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 

		if (GetOpenFileName(&ofn) == TRUE)
		{
			//hf = CreateFile(ofn.lpstrFile,
			//	GENERIC_READ,
			//	0,
			//	(LPSECURITY_ATTRIBUTES)NULL,
			//	OPEN_EXISTING,
			//	FILE_ATTRIBUTE_NORMAL,
			//	(HANDLE)NULL);


			//printf("%S", szfile);
			//std::wstring wstr(szfile);
			//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
			//std::string narrowString = converter.to_bytes(szfile);
			//std::cout << narrowString << std::endl;
			std::string path = wideCharToMultiByte(szfile);
			return path;
		}

		return std::string("");
	}
}

