#include "stdafx.h"
#include "Input.h"


//--- �O���[�o���ϐ�
BYTE g_keyTable[256];
BYTE g_oldTable[256];

HRESULT InitInput()
{
	// ��ԍŏ��̓���
	(void)GetKeyboardState(g_keyTable);
	return S_OK;
}
void UninitInput()
{
}
void UpdateInput()
{
	// �Â����͂��X�V
	memcpy_s(g_oldTable, sizeof(g_oldTable), g_keyTable, sizeof(g_keyTable));
	// ���݂̓��͂��擾
	(void)GetKeyboardState(g_keyTable);
}

bool IsKeyPress(BYTE key)
{
	return g_keyTable[key] & 0x80;
}

bool IsKeyTrigger(BYTE key)
{
	return (g_keyTable[key] ^ g_oldTable[key]) & g_keyTable[key] & 0x80;
}

bool IsKeyRelease(BYTE key)
{
	return (g_keyTable[key] ^ g_oldTable[key]) & g_oldTable[key] & 0x80;
}

// �L�[���s�[�g
bool IsKeyRepeat(BYTE key)
{
	return IsKeyPress(key) && (g_keyTable[key] & 0x80) && (g_oldTable[key] & 0x80);
}
