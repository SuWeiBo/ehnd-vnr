#include "stdafx.h"
#include "filter.h"


filter::filter()
{
}


filter::~filter()
{
}

bool filter::pre_load()
{
	WCHAR lpEztPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	FILTERSTRUCT fs;
	wstring Path;

	DWORD dwStart, dwEnd;
	dwStart = GetTickCount();

	GetLoadPath(lpEztPath, MAX_PATH);
	Path = lpEztPath;
	Path += L"\\Ehnd\\PreFilter*.txt";

	int pre_line = 0;
	bool IsUnicode;
	PreFilter.clear();

	HANDLE hFind = FindFirstFile(Path.c_str(), &FindFileData);
	
	do
	{
		if (hFind == INVALID_HANDLE_VALUE) break;
		else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		if (!wcsncmp(FindFileData.cFileName, L"PreFilterU", 10))
			IsUnicode = true;
		else IsUnicode = false;

		Path = lpEztPath;
		Path += L"\\Ehnd\\";

		filter_load(PreFilter, Path.c_str(), FindFileData.cFileName, 1, IsUnicode, pre_line);

	} while (FindNextFile(hFind, &FindFileData));

	// ����
	sort(PreFilter.begin(), PreFilter.end());
	WriteLog(L"PreFilterRead : ���� ������ �Ϸ��߽��ϴ�.\n");

	// �ҿ�ð� ���
	dwEnd = GetTickCount(); WriteLog(L"PreFilterRead : --- Elasped Time : %dms ---\n", dwEnd - dwStart);

	return true;
}

bool filter::post_load()
{
	WCHAR lpEztPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	FILTERSTRUCT fs;
	wstring Path;

	DWORD dwStart, dwEnd;
	dwStart = GetTickCount();

	GetLoadPath(lpEztPath, MAX_PATH);
	Path = lpEztPath;
	Path += L"\\Ehnd\\PostFilter*.txt";

	int post_line = 0;
	bool IsUnicode;
	PostFilter.clear();

	HANDLE hFind = FindFirstFile(Path.c_str(), &FindFileData);

	do
	{
		if (hFind == INVALID_HANDLE_VALUE) break;
		else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		if (!wcsncmp(FindFileData.cFileName, L"PostFilterU", 11))
			IsUnicode = true;
		else IsUnicode = false;

		Path = lpEztPath;
		Path += L"\\Ehnd\\";

		filter_load(PostFilter, Path.c_str(), FindFileData.cFileName, 2, IsUnicode, post_line);

	} while (FindNextFile(hFind, &FindFileData));

	// ����
	sort(PostFilter.begin(), PostFilter.end());
	WriteLog(L"PostFilterRead : ���� ������ �Ϸ��߽��ϴ�.\n");

	// �ҿ�ð� ���
	dwEnd = GetTickCount();
	WriteLog(L"PostFilterRead : --- Elasped Time : %dms ---\n", dwEnd - dwStart);

	return true;
}

bool filter::userdic_load()
{
	WCHAR lpEztPath[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	wstring Path;

	DWORD dwStart, dwEnd;
	dwStart = GetTickCount();

	GetLoadPath(lpEztPath, MAX_PATH);
	Path = lpEztPath;
	Path += L"\\Ehnd\\UserDict*.txt";

	int userdic_line = 0;
	UserDic.clear();

	HANDLE hFind = FindFirstFile(Path.c_str(), &FindFileData);

	do
	{
		if (hFind == INVALID_HANDLE_VALUE) break;
		else if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			continue;

		Path = lpEztPath;
		Path += L"\\Ehnd\\";

		userdic_load2(Path.c_str(), FindFileData.cFileName, userdic_line);

	} while (FindNextFile(hFind, &FindFileData));

	// ����
	sort(UserDic.begin(), UserDic.end());
	WriteLog(L"UserDicRead : ����� ���� ������ �Ϸ��߽��ϴ�.\n");

	// �ҿ�ð� ���
	dwEnd = GetTickCount();
	WriteLog(L"UserDicRead : --- Elasped Time : %dms ---\n", dwEnd - dwStart);

	return true;
}

bool filter::skiplayer_load()
{
	WCHAR lpEztPath[MAX_PATH], Buffer[1024], Context[1024];
	wstring Path;
	FILE *fp;

	DWORD dwStart, dwEnd;
	dwStart = GetTickCount();

	GetLoadPath(lpEztPath, MAX_PATH);
	Path = lpEztPath;
	Path += L"\\Ehnd\\SkipLayer.txt";

	int vaild_line = 0;

	SkipLayer.clear();

	if (_wfopen_s(&fp, Path.c_str(), L"rt,ccs=UTF-8") != 0)
	{
		WriteLog(L"[SkipLayerRead] ��ŵ���̾� �ε� ����.\n");
		return false;
	}
	WriteLog(L"SkipLayerRead : SkipLayer �ε�.\n");
	
	for (int line = 0; fgetws(Buffer, 1000, fp) != NULL; line++)
	{
		int tab = 0;
		if (Buffer[0] == L'/' && Buffer[1] == L'/') continue;

		SKIPLAYERSTRUCT ss;
		ss.line = line;

		for (UINT i = 0, prev = 0; i < wcslen(Buffer) + 1; i++)
		{
			if (Buffer[i] == L'\t' || Buffer[i] == L'\n' || (Buffer[i] == L'/' && Buffer[i - 1] == L'/') || i == wcslen(Buffer))
			{
				switch (tab)
				{
				case 0:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					tab++;
					prev = i + 1;
					ss.wtype = Context;
					if (!wcsncmp(Context, L"PRE", 3)) ss.type = 1;
					else if (!wcsncmp(Context, L"POST", 4)) ss.type = 2;
					else i = wcslen(Buffer) + 1;
					break;
				case 1:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					tab++;
					prev = i + 1;
					ss.layer = _wtoi(Context);
					ss.wlayer = Context;
					break;
				case 2:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					tab++;
					prev = i + 1;
					ss.cond = Context;
					break;
				}
			}
		}
		if (tab < 2) continue;

		try
		{
			wregex ex(ss.cond);
		}
		catch (regex_error ex)
		{
			WCHAR lpWhat[255];
			int len = MultiByteToWideChar(949, MB_PRECOMPOSED, ex.what(), -1, NULL, NULL);
			MultiByteToWideChar(949, MB_PRECOMPOSED, ex.what(), -1, lpWhat, len);

			WriteLog(L"SkipLayerRead : ���Խ� ����! : [%s:%d] %s | %s | %s\n", L"SkipLayer.txt", line, ss.wtype, ss.wlayer, ss.cond);

			continue;
		}
		vaild_line++;
		SkipLayer.push_back(ss);
	}
	fclose(fp);
	WriteLog(L"SkipLayerRead : �� %d���� ��ŵ���̾ �о����ϴ�.\n", vaild_line);

	// �ҿ�ð� ���
	dwEnd = GetTickCount();
	WriteLog(L"SkipLayerRead : --- Elasped Time : %dms ---\n", dwEnd - dwStart);
	return true;
}

bool filter::filter_load(vector<FILTERSTRUCT> &Filter, LPCWSTR lpPath, LPCWSTR lpFileName, int FilterType, bool IsUnicode, int &g_line)
{
	FILE *fp;
	WCHAR Buffer[1024], Context[1024];

	wstring Path;
	Path = lpPath;
	Path += lpFileName;
	int vaild_line = 0;

	if (_wfopen_s(&fp, Path.c_str(), L"rt,ccs=UTF-8") != 0)
	{
		if (FilterType == 1 && IsUnicode) WriteLog(L"PreFilterRead : ��ó�� �����ڵ� ���� ���� '%s' �ε� ����!\n", lpFileName);
		else if (FilterType == 1 && !IsUnicode) WriteLog(L"PreFilterRead : ��ó�� ���� '%s' �ε� ����!\n", lpFileName);
		else if (FilterType == 2 && IsUnicode) WriteLog(L"PostFilterRead : ��ó�� �����ڵ� ���� ���� '%s' �ε� ����!\n", lpFileName);
		else if (FilterType == 2 && !IsUnicode) WriteLog(L"PostFilterRead : ��ó�� ���� '%s' �ε� ����!\n", lpFileName);
		return false;
	}

	if (FilterType == 1 && IsUnicode) WriteLog(L"PreFilterRead : ��ó�� �����ڵ� ���� ���� \"%s\" �ε�.\n", lpFileName);
	if (FilterType == 1 && !IsUnicode) WriteLog(L"PreFilterRead : ��ó�� ���� \"%s\" �ε�.\n", lpFileName);
	else if (FilterType == 2 && IsUnicode) WriteLog(L"PostFilterRead : ��ó�� �����ڵ� ���� ���� \"%s\" �ε�.\n", lpFileName);
	else if (FilterType == 2 && !IsUnicode) WriteLog(L"PostFilterRead : ��ó�� ���� \"%s\" �ε�.\n", lpFileName);

	for (int line = 0; fgetws(Buffer, 1000, fp) != NULL; line++, g_line++)
	{
		if (Buffer[0] == L'/' && Buffer[1] == L'/') continue;	// �ּ�

		FILTERSTRUCT fs;
		fs.g_line = g_line;
		fs.line = line;
		fs.db = lpFileName;
		fs.unicode = IsUnicode;
		
		int tab = 0;
		for (UINT i = 0, prev = 0; i < wcslen(Buffer); i++)
		{
			if (Buffer[i] == L'\t' || Buffer[i] == L'\n' || (Buffer[i] == L'/' && Buffer[i - 1] == L'/') || i == wcslen(Buffer))
			{
				switch (tab)
				{
				case 0:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					fs.src = Context;
					break;
				case 1:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					fs.dest = Context;
					break;
				case 2:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					fs.layer = _wtoi(Context);
					break;
				case 3:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					fs.regex = _wtoi(Context);
					break;
				}
				if (Buffer[i] == L'/' && Buffer[i - 1] == L'/') break;
			}
		}

		if (tab < 3) continue;
		if (fs.regex == 1)
		{
			try
			{
				wregex ex(fs.src);
			}
			catch (regex_error ex)
			{
				WCHAR lpWhat[255];
				int len = MultiByteToWideChar(949, MB_PRECOMPOSED, ex.what(), -1, NULL, NULL);
				MultiByteToWideChar(949, MB_PRECOMPOSED, ex.what(), -1, lpWhat, len);

				if (FilterType == 1) WriteLog(L"PreFilterRead : ���Խ� ����! : [%s:%d] %s | %s | %d | %d\n", lpFileName, line, fs.src.c_str(), fs.dest.c_str(), fs.layer, fs.regex);
				else if (FilterType == 2) WriteLog(L"PostFilterRead : ���Խ� ����! : [%s:%d] %s | %s | %d | %d\n", lpFileName, line, fs.src.c_str(), fs.dest.c_str(), fs.layer, fs.regex);
				continue;
			}
		}
		vaild_line++;
		Filter.push_back(fs);
	}
	fclose(fp);
	if (FilterType == 1 && IsUnicode) WriteLog(L"PreFilterRead : �� %d���� �����ڵ� ���� ��ó�� ���� \"%s\"�� �о����ϴ�.\n", vaild_line, lpFileName);
	if (FilterType == 1 && !IsUnicode) WriteLog(L"PreFilterRead : �� %d���� ��ó�� ���� \"%s\"�� �о����ϴ�.\n", vaild_line, lpFileName);
	else if (FilterType == 2 && IsUnicode) WriteLog(L"PostFilterRead : �� %d���� �����ڵ� ���� ��ó�� ���� \"%s\"�� �о����ϴ�.\n", vaild_line, lpFileName);
	else if (FilterType == 2 && !IsUnicode) WriteLog(L"PostFilterRead : �� %d���� ��ó�� ���� \"%s\"�� �о����ϴ�.\n", vaild_line, lpFileName);
	return true;
}

bool filter::userdic_load2(LPCWSTR lpPath, LPCWSTR lpFileName, int &g_line)
{
	FILE *fp;
	WCHAR Buffer[1024], Context[1024];
	wstring Path, Jpn, Kor, Part, Attr;
	int vaild_line = 0;
	Path = lpPath;
	Path += lpFileName;

	if (_wfopen_s(&fp, Path.c_str(), L"rt,ccs=UTF-8") != 0)
	{
		WriteLog(L"UserDictRead : ����� ���� '%s' �ε� ����!\n", lpFileName);
		return false;
	}

	WriteLog(L"UserDictRead : ����� ���� \"%s\" �ε�.\n", lpFileName);

	for (int line = 0; fgetws(Buffer, 1000, fp) != NULL; line++, g_line++)
	{
		if (Buffer[0] == L'/' && Buffer[1] == L'/') continue;	// �ּ�

		USERDICSTRUCT us;
		us.hidden = 0x00;
		memset(us.jpn, 0, sizeof(us.jpn));
		memset(us.kor, 0, sizeof(us.kor));
		memset(us.part, 0, sizeof(us.part));
		memset(us.attr, 0, sizeof(us.attr));

		int tab = 0;
		for (UINT i = 0, prev = 0; i < wcslen(Buffer); i++)
		{
			if (Buffer[i] == L'\t' || Buffer[i] == L'\n' || (Buffer[i] == L'/' && Buffer[i - 1] == L'/') || i == wcslen(Buffer))
			{
				switch (tab)
				{
				case 0:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					Jpn = Context;
					break;
				case 1:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					Kor = Context;
					break;
				case 2:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					if (_wtoi(Context))
						Part = L"I110";
					else
						Part = L"A9D0";
					break;
				case 3:
					wcsncpy_s(Context, Buffer + prev, i - prev);
					prev = i + 1;
					tab++;
					Attr = Context;
					break;
				}
				if (Buffer[i] == L'/' && Buffer[i - 1] == L'/') break;
			}
		}

		if (tab < 3) continue;
		int len;

		if ((len = WideCharToMultiByte(932, 0, Jpn.c_str(), -1, NULL, NULL, NULL, NULL)) > 31)
		{
			WriteLog(L"UserDicRead : ���� : ���� �ܾ��� ���̴� 15��(30Byte)�� �ʰ��� �� �����ϴ�.\n");
			WriteLog(L"UserDicRead : ���� : [%s:%d] : <<%s>> | %s | %s | %s\n", lpFileName, line, Jpn, Kor, Part, Attr);
		}
		WideCharToMultiByte(932, 0, Jpn.c_str(), -1, us.jpn, len, NULL, NULL);

		if ((len = WideCharToMultiByte(949, 0, Kor.c_str(), -1, NULL, NULL, NULL, NULL)) > 31)
		{
			WriteLog(L"UserDicRead : ���� : ���� �ܾ��� ���̴� 15��(30Byte)�� �ʰ��� �� �����ϴ�.\n");
			WriteLog(L"UserDicRead : ���� : [%s:%d] : %s | <<%s>> | %s | %s\n", lpFileName, line, Jpn, Kor, Part, Attr);
		}
		WideCharToMultiByte(949, 0, Kor.c_str(), -1, us.kor, len, NULL, NULL);

		if ((len = WideCharToMultiByte(932, 0, Attr.c_str(), -1, NULL, NULL, NULL, NULL)) > 31)
		{
			WriteLog(L"UserDicRead : ���� :  �ܾ� �Ӽ��� 42Byte�� �ʰ��� �� �����ϴ�.\n");
			WriteLog(L"UserDicRead : ���� : [%s:%d] : %s | %s | %s | <<%s>>\n", lpFileName, line, Jpn, Kor, Part, Attr);
		}
		WideCharToMultiByte(932, 0, Kor.c_str(), -1, us.kor, len, NULL, NULL);

		len = WideCharToMultiByte(932, 0, Part.c_str(), -1, NULL, NULL, NULL, NULL);
		WideCharToMultiByte(932, 0, Part.c_str(), -1, us.part, len, NULL, NULL);

		vaild_line++;
		UserDic.push_back(us);
	}
	WriteLog(L"UserDictRead : �� %d���� ����� ���� \"%s\"�� �о����ϴ�.\n", vaild_line, lpFileName);
	fclose(fp);
	return true;
}
bool filter::pre()
{
	return true;
}

bool filter::post()
{
	return true;
}