#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<cstdio>
#include"resource.h"
#include<stdio.h>
#include<iostream>

#define BUFFER_SIZE 1024

void PrintInfo(HWND hwnd);

CHAR* IPtoString(DWORD dwIPaddress, CHAR szIPaddress[]);

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hPrefix, UDM_SETRANGE, 0, MAKELPARAM(30, 1));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		SetFocus(GetDlgItem(hwnd, IDC_IPADDRESS));

		AllocConsole(); // создает новую консоль
		freopen("CONOUT$", "w", stdout); // перенаправляет стандартный вывод (stdout) в созданную консоль. Без этого вызовы printf или std::cout не будут работать
	}
	break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
		{
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			if (FIRST_IPADDRESS(dwIPaddress) < 128)SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)"8");/*hIPmask, IPM_SETADDRESS, 0, 0xFF000000);*/
			else if (FIRST_IPADDRESS(dwIPaddress) < 192)SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)"16");//SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 224)SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)"24");//SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);
		}
		break;
		case IDC_SPIN_PREFIX:  //https://learn.microsoft.com/en-us/windows/win32/controls/udn-deltapos
		{
			std::cout << "WM_NOTYFY:IDC_SPIN_PREFIX" << std::endl;
			// NMUPDOWN - структура, содержащая информацию о движении спиннера
			DWORD dwPrefix = ((NMUPDOWN*)lParam)->iPos; // текущее значение спиннера
			INT iDelta = ((NMUPDOWN*)lParam)->iDelta; // шаг изменения (+1 или -1)
			dwPrefix += iDelta; // новое значение после изменения
			//MessageBox(hwnd, "IDC_EDIT_PREFIX_CHANGED", "Info", IDOK);
			//CHAR sz_prefix[3] = {};
			//SendMessage(hEditPrefix, WM_GETTEXT, 3, (LPARAM)sz_prefix);
			//DWORD dwPrefix = atoi(sz_prefix); //atoi(sz_string) - ASCII-string to INT
			std::cout << dwPrefix << std::endl;
			std::cout << iDelta << std::endl;
			DWORD dwIPmask = 0xFFFFFFFF << (32 - dwPrefix);
			//for (int i = 0; i < (32 - dwPrefix); i++) dwIPmask <<= 1;
			SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
		}
		break;

		case IDC_EDIT_PREFIX: //IDC_IPMASK:
		{
			CHAR sz_prefix[4] = {};
			SendMessage(hEditPrefix, WM_GETTEXT, 3, (LPARAM)sz_prefix);
			DWORD dwPrefix = atoi(sz_prefix);  // преобразует текст в число
			DWORD dwIPmask = ~(0xFFFFFFFF >> dwPrefix); // Пример для /24: 0xFFFFFFFF >> 24 = 0x000000FF → ~0x000000FF = 0xFFFFFF00.
			SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPmask);
			PrintInfo(hwnd);
		}
		break;
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
	}
	break;
	case WM_NOTIFY:
	{
		std::cout << "WM_NOTIFY:";
		std::cout << "wParam: " << wParam << std::endl;
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		DWORD dwIPmask = 0;

		switch (((NMHDR*)lParam)->idFrom)
		{
		case IDC_IPMASK: // IDC_EDIT_PREFIX:
		{
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwIPprefix = 0;
			for (DWORD iMask = dwIPmask; iMask & 0x80000000; dwIPprefix++)iMask <<= 1; // считает количество ведущих единиц в маске
			CHAR sz_prefix[4];
			sprintf(sz_prefix, "%i", dwIPprefix); // число -> строка
			SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)sz_prefix); // обновляет Edit
		}
		break;
		}
	}
	break;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		FreeConsole();
	}
	return FALSE;
}

void PrintInfo(HWND hwnd)
{
	HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
	HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
	HWND hStaticInfo = GetDlgItem(hwnd, IDC_STATIC_INFO);
	DWORD dwIPaddress = 0;
	DWORD dwIPmask = 0;
	SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
	SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
	DWORD dwNetworkAddress = dwIPaddress & dwIPmask;
	DWORD dwBroadcastAddress = dwIPaddress | ~dwIPmask;
	DWORD dwNumberOfAddresses = ~dwIPmask + 1;
	DWORD dwNumberOfHost = ~dwIPmask - 1;

	CHAR szNetworkAddress[16] = {};
	CHAR szBroadcastAddress[16] = {}; // 

	CHAR sz_info[BUFFER_SIZE] = {};
	sprintf
	(
		sz_info,
		"Info:\nАдрес сети: \t\t\t%s;\nШироковещательный адрес:\t%s;\nКоличество IP-адресов:\t%i;\nКоличество узлов:\t\t\%i",
		IPtoString(dwNetworkAddress, szNetworkAddress),
		IPtoString(dwBroadcastAddress, szBroadcastAddress),
		dwNumberOfAddresses,
		dwNumberOfHost

	);
	SendMessage(hStaticInfo, WM_SETTEXT, 0, (LPARAM)sz_info);
}

CHAR* IPtoString(DWORD dwIPaddress, CHAR szIPaddress[])
{
	sprintf(szIPaddress,
		"%i.%i.%i.%i",
		FIRST_IPADDRESS(dwIPaddress),
		SECOND_IPADDRESS(dwIPaddress),
		THIRD_IPADDRESS(dwIPaddress),
		FOURTH_IPADDRESS(dwIPaddress)
	);
	return szIPaddress;
}