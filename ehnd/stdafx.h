// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"
#define EHND_VER "V3.00.150215"
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#include <WinBase.h>
#include <windef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <Psapi.h>

#include <iostream>
#include <vector>
#include <regex>

using namespace std;
#define PREFILTER 1
#define POSTFILTER 2

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "ehnd.h"
#include "hook.h"
#include "log.h"
#include "filter.h"

extern HINSTANCE g_hInst;
extern Cehnd *pEhnd;
extern filter *pFilter;

extern LPBYTE lpfnRetn;
extern LPBYTE lpfnfopen;
extern HMODULE hEzt, hMsv;

#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#else
#define WIDEN(x)
#endif