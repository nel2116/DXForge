#include "stdafx.h"
#include "Logger.h"
#include <Windows.h>
#include <comdef.h>

void OutputLog(const char* format, ...)
{
	char msg[2048];
	memset(msg, '\0', sizeof(msg));
	va_list arg;

	va_start(arg, format);
	vsprintf_s(msg, format, arg);
	va_end(arg);

	// コンソールに出力.
	printf_s("%s", msg);

	// Visual Studioの出力ウィンドウにも表示.
	OutputDebugStringA(msg);
}

void LogHResult(HRESULT hr)
{
	if (FAILED(hr))
	{
		// エラーメッセージを取得
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		// エラーメッセージをログ出力
		ELOG("HRESULT Error: 0x%08X, Message: %s", hr, errMsg);
	}
}
