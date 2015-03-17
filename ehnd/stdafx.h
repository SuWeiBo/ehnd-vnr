// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"
#define EHND_VER "V3.04"
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

#define NORMAL_LOG 0
#define DETAIL_LOG 10
#define TIME_LOG 20
#define SKIPLAYER_LOG 30
#define ERROR_LOG 40

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include "ehnd.h"
#include "hook.h"
#include "log.h"
#include "filter.h"
#include "watch.h"
#include "config.h"

extern HINSTANCE g_hInst;
extern filter *pFilter;
extern watch *pWatch;
extern config *pConfig;
extern int g_initTick;
extern char g_DicPath[MAX_PATH];

extern LPBYTE lpfnRetn;
extern LPBYTE lpfnfopen;
extern LPBYTE lpfnwc2mb;
extern LPBYTE lpfnwc2mb_main;
extern LPBYTE lpfnmb2wc;
extern LPBYTE lpfnmb2wc_main;

extern wstring watchStr;
extern wstring resStr;

extern HMODULE hEzt, hMsv;
extern BOOL initOnce;

#ifdef _UNICODE
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#else
#define WIDEN(x)
#endif