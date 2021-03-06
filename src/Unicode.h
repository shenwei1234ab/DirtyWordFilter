//
//  vnUnicode.h
//  vnlib
//
//  Created by Wayne on 14-5-11.
//  Copyright (c) 2014 viichi.com. All rights reserved.
//

#ifndef vnlib_vnUnicode_h
#define vnlib_vnUnicode_h
#include "vnDefine.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

class Unicode {
public:
	static ssize_t convert(const c8 *src, size_t src_len, c32 *dest);
	static ssize_t convert(const c8 *src, size_t src_len, c32 *dest, size_t dest_len);
	static ssize_t convert(const c8 *src, size_t src_len, str32 &dest);
	static ssize_t convert(const str8 &src, str32 &dest) {
		return convert(src.c_str(), src.length(), dest);
	}

	static ssize_t convert(const c8 *src, size_t src_len, c16 *dest);
	static ssize_t convert(const c8 *src, size_t src_len, c16 *dest, size_t dest_len);
	static ssize_t convert(const c8 *src, size_t src_len, str16 &dest);
	static ssize_t convert(const str8 &src, str16 &dest) {
		return convert(src.c_str(), src.length(), dest);
	}

	static ssize_t convert(const c16 *src, size_t src_len, c8 *dest);
	static ssize_t convert(const c16 *src, size_t src_len, c8 *dest, size_t dest_len);
	static ssize_t convert(const c16 *src, size_t src_len, str8 &dest);
	static ssize_t convert(const str16 &src, str8 &dest) {
		return convert(src.c_str(), src.length(), dest);
	}

	static ssize_t convert(const c16 *src, size_t src_len, c32 *dest);
	static ssize_t convert(const c16 *src, size_t src_len, c32 *dest, size_t dest_len);
	static ssize_t convert(const c16 *src, size_t src_len, str32 &dest);
	static ssize_t convert(const str16 &src, str32 &dest) {
		return convert(src.c_str(), src.length(), dest);
	}

	static ssize_t convert(const c32 *src, size_t src_len, c8 *dest);
	static ssize_t convert(const c32 *src, size_t src_len, c8 *dest, size_t dest_len);
	static ssize_t convert(const c32 *src, size_t src_len, str8 &dest);
	static ssize_t convert(const str32 &src, str8 &dest) {
		return convert(src.c_str(), src.length(), dest);
	}

	static ssize_t convert(const c32 *src, size_t src_len, c16 *dest);
	static ssize_t convert(const c32 *src, size_t src_len, c16 *dest, size_t dest_len);
	static ssize_t convert(const c32 *src, size_t src_len, str16 &dest);
	static ssize_t convert(const str32 &src, str16 &dest) {
		return convert(src.c_str(), src.length(), dest);
	}


#ifdef _WIN32
	static int GBKToUTF8(unsigned char * lpGBKStr, unsigned char * lpUTF8Str, int nUTF8StrLen)
	{
		wchar_t * lpUnicodeStr = NULL;
		int nRetLen = 0;

		if (!lpGBKStr)  //????GBK????????NULL??????????
			return 0;
		nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)lpGBKStr, -1, NULL, NULL);  //??????????Unicode??????????????????????????
		lpUnicodeStr = new WCHAR[nRetLen + 1];  //??Unicode??????????
		nRetLen = ::MultiByteToWideChar(CP_ACP, 0, (char *)lpGBKStr, -1, lpUnicodeStr, nRetLen);  //??????Unicode????
		if (!nRetLen)  //??????????????????
			return 0;

		nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, NULL, 0, NULL, NULL);  //??????????UTF8??????????????????????????

		if (!lpUTF8Str)  //????????????????????????????????????????
		{
			if (lpUnicodeStr)
				delete[]lpUnicodeStr;
			return nRetLen;
		}

		if (nUTF8StrLen < nRetLen)  //????????????????????????????
		{
			if (lpUnicodeStr)
				delete[]lpUnicodeStr;
			return 0;
		}

		nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, -1, (char *)lpUTF8Str, nUTF8StrLen, NULL, NULL);  //??????UTF8????

		if (lpUnicodeStr)
			delete[]lpUnicodeStr;

		return nRetLen;
	}


	static int UTF8ToGBK(unsigned char * lpUTF8Str, unsigned char * lpGBKStr, int nGBKStrLen)
	{
		wchar_t * lpUnicodeStr = NULL;
		int nRetLen = 0;

		if (!lpUTF8Str)  //????UTF8????????NULL??????????
			return 0;

		nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, NULL, NULL);  //??????????Unicode??????????????????????????
		lpUnicodeStr = new WCHAR[nRetLen + 1];  //??Unicode??????????
		nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, (char *)lpUTF8Str, -1, lpUnicodeStr, nRetLen);  //??????Unicode????
		if (!nRetLen)  //??????????????????
			return 0;

		nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);  //??????????GBK??????????????????????????

		if (!lpGBKStr)  //????????????????????????????????????????
		{
			if (lpUnicodeStr)
				delete[]lpUnicodeStr;
			return nRetLen;
		}

		if (nGBKStrLen < nRetLen)  //????????????????????????????
		{
			if (lpUnicodeStr)
				delete[]lpUnicodeStr;
			return 0;
		}

		nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, (char *)lpGBKStr, nRetLen, NULL, NULL);  //??????GBK????

		if (lpUnicodeStr)
			delete[]lpUnicodeStr;

		return nRetLen;
	}



	static std::string utf8fromgbk(const std::string &check)
	{
		std::string out;
		int nRetLen = Unicode::GBKToUTF8((unsigned char *)check.c_str(), NULL, NULL);
		printf("????????????????????????????????%d \n", nRetLen);
		char *lpUTF8Str = new char[nRetLen + 1];
		nRetLen = Unicode::GBKToUTF8((unsigned char *)check.c_str(), (unsigned char *)lpUTF8Str, nRetLen);
		if (nRetLen)
		{
			printf("GBKToUTF8??????????");
			out = lpUTF8Str;
			/*for (int i = 0; i < nRetLen; ++i)
			{
			printf("%x\n", lpUTF8Str[i]);
			}*/
		}
		else
		{
			printf("GBKToUTF8??????????");
		}
		delete[]lpUTF8Str;
		return out;
	}


	static std::string gbkfromutf8(const std::string &utf)
	{
		std::string out;
		int nRetLen = UTF8ToGBK((unsigned char *)utf.c_str(), NULL, NULL);  //????????
		printf("????????????????????????????????%d ", nRetLen);
		char *lpGBKStr = new char[nRetLen + 1];
		nRetLen = UTF8ToGBK((unsigned char *)utf.c_str(), (unsigned char *)lpGBKStr, nRetLen);
		if (nRetLen)
		{
			printf("UTF8toGBK??????????\n");
			out = lpGBKStr;
		}
		else
		{
			printf("UTF8ToGBK??????????");
		}
		delete[]lpGBKStr;
		return out;
	}
#endif
};



#endif
