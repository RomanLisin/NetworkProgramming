#pragma once
#include <Windows.h>
#include <CommCtrl.h>

// преобразует префикс (0-32) в маску и форматирует для IP-контрола
inline DWORD PrefixToMaskForIPControl(int prefix)
{
	DWORD mask = (prefix == 0) ? 0 : (0xFFFFFFFF << (32 - prefix));

	BYTE b1 = (mask >> 24) & 0xFF;
	BYTE b2 = (mask >> 16) & 0xFF;
	BYTE b3 = (mask >> 8) & 0xFF;
	BYTE b4 = mask & 0xFF;

	return MAKEIPADDRESS(b1, b2, b3, b4);
}

// получает маску из IP-контрола и преобразует в префикс
inline int MaskToPrefix(DWORD ipControlMask)
{
	BYTE b1 = FIRST_IPADDRESS(ipControlMask);
	BYTE b2 = SECOND_IPADDRESS(ipControlMask);
	BYTE b3 = THIRD_IPADDRESS(ipControlMask);
	BYTE b4 = FOURTH_IPADDRESS(ipControlMask);

	DWORD mask = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

	int prefix = 0;
	for (int i = 31; i >= 0; i--)
	{
		if ((mask >> i) & 1)
			prefix++;
		else
			break;
	}
	return prefix;
}

// преобразует обычный DWORD IP в формат для IP - контрола
inline DWORD DWORDToIPControlFormat(DWORD ip)
{
	BYTE b1 = (ip >> 24) & 0xFF;
	BYTE b2 = (ip >> 16) & 0xFF;
	BYTE b3 = (ip >> 8) & 0xFF;
	BYTE b4 = ip & 0xFF;

	return MAKEIPADDRESS(b1, b2, b3, b4);
}

// преобразует IP -контрольный DWORD в обычный DWORD IP
inline DWORD IPControlFormatToDWORD(DWORD ctrlIP)
{
	BYTE b1 = FIRST_IPADDRESS(ctrlIP);
	BYTE b2 = SECOND_IPADDRESS(ctrlIP);
	BYTE b3 = THIRD_IPADDRESS(ctrlIP);
	BYTE b4 = FOURTH_IPADDRESS(ctrlIP);

	return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

inline DWORD CalculateNetworkAddress(DWORD ip, DWORD mask)
{
	return ip & mask;
}
