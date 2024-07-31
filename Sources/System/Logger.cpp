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

	// �R���\�[���ɏo��.
	printf_s("%s", msg);

	// Visual Studio�̏o�̓E�B���h�E�ɂ��\��.
	OutputDebugStringA(msg);
}

void LogHResult(HRESULT hr)
{
	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W���擾
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();

		// �G���[���b�Z�[�W�����O�o��
		ELOG("HRESULT Error: 0x%08X, Message: %s", hr, errMsg);
	}
}
