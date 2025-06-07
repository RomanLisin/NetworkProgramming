#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"
#include"IPUtils.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
    InitCommonControls(); // ??
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		{
	case WM_INITDIALOG:
		HWND hPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hPrefix, UDM_SETRANGE, 0, MAKELPARAM(32, 0));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		SetFocus(GetDlgItem(hwnd, IDC_IPADDRESS));
		}
		break;

    case WM_NOTIFY:
    {
        NMUPDOWN* pNMUpDown = (NMUPDOWN*)lParam;
        if (pNMUpDown->hdr.idFrom == IDC_SPIN_PREFIX)
        {
            int prefix = SendMessage(pNMUpDown->hdr.hwndFrom, UDM_GETPOS, 0, 0);
            prefix += pNMUpDown->iDelta;

            if (prefix < 0) prefix = 0;
            if (prefix > 32) prefix = 32;

            DWORD mask = PrefixToMaskForIPControl(prefix);
            HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
            SendMessage(hIPmask, IPM_SETADDRESS, 0, mask);

            // get current IP
            DWORD dwIPraw;
            SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS), IPM_GETADDRESS, 0, (LPARAM)&dwIPraw);
            DWORD dwIPaddress = IPControlFormatToDWORD(dwIPraw);

            // calculate and set network address
            DWORD networkAddr = CalculateNetworkAddress(dwIPaddress, mask);
            SendMessage(GetDlgItem(hwnd, IDC_IPADDRESS), IPM_SETADDRESS, 0, networkAddr);

            // Обновляем значение в edit control
            SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);

            return TRUE;
        }
    }
    break;
    case WM_COMMAND:
    {
        HWND hIP = GetDlgItem(hwnd, IDC_IPADDRESS);
        HWND hMask = GetDlgItem(hwnd, IDC_IPMASK);
        HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
        DWORD ipRaw = 0;
        DWORD maskRaw = 0;

        switch (LOWORD(wParam))
        {
        case IDC_IPADDRESS:
            if (HIWORD(wParam) == EN_CHANGE)
            {
                SendMessage(hIP, IPM_GETADDRESS, 0, (LPARAM)&ipRaw);
                DWORD ip = IPControlFormatToDWORD(ipRaw);
                if (FIRST_IPADDRESS(ipRaw) < 128) SendMessage(hMask, IPM_SETADDRESS, 0, 0xFF000000);
                else if (FIRST_IPADDRESS(ipRaw) < 192) SendMessage(hMask, IPM_SETADDRESS, 0, 0xFFFF0000);
                else if (FIRST_IPADDRESS(ipRaw) < 224) SendMessage(hMask, IPM_SETADDRESS, 0, 0xFFFFFF00);

                // обновляем префикс при изменении маски
                SendMessage(hMask, IPM_GETADDRESS, 0, (LPARAM)&maskRaw);
                int prefix = MaskToPrefix(maskRaw);
                SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);

                // вычисляем и устанавливаем адрес сети
                DWORD networkAddr = CalculateNetworkAddress(ip, maskRaw);
                SendMessage(hIP, IPM_SETADDRESS, 0, DWORDToIPControlFormat(networkAddr));
            }
            break;

        case IDC_IPMASK: // если маска меняется вручную
            if (HIWORD(wParam) == EN_CHANGE)
            {
                SendMessage(hIP, IPM_GETADDRESS, 0, (LPARAM)&ipRaw);
                SendMessage(hMask, IPM_GETADDRESS, 0, (LPARAM)&maskRaw);

                // обновляем префикс
                int prefix = MaskToPrefix(maskRaw);
                SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);

                // вычисляем и устанавливаем адрес сети
                DWORD networkAddr = CalculateNetworkAddress(ipRaw, maskRaw);
                SendMessage(hIP, IPM_SETADDRESS, 0, networkAddr);
            }
            break;

        case IDC_EDIT_PREFIX:
            if (HIWORD(wParam) == EN_CHANGE)
            {
                BOOL translated;
                int prefix = GetDlgItemInt(hwnd, IDC_EDIT_PREFIX, &translated, FALSE);

                if (translated && prefix >= 0 && prefix <= 32)
                {
                    // Преобразуем префикс в маску
                    DWORD mask = PrefixToMaskForIPControl(prefix);

                    //BYTE b1 = (mask >> 24) & 0xff;
                    //BYTE b2 = (mask >> 16) & 0xff;
                    //BYTE b3 = (mask >> 8) & 0xff;
                    //BYTE b4 = mask & 0xff;
                    //DWORD addr = MAKEIPADDRESS(b4, b2, b3, b1); // IPM_SETADDRESS ожидает Little Endian порядок
                    SendMessage(hMask, IPM_SETADDRESS, 0, mask);

                    // получаем текущий IP
                    SendMessage(hIP, IPM_GETADDRESS, 0, (LPARAM)&ipRaw);

                    // вычисляем и устанавливаем адрес сети
                    DWORD networkAddr = CalculateNetworkAddress(ipRaw, mask);
                    SendMessage(hIP, IPM_SETADDRESS, 0, networkAddr);
                }
            }
            break;
       }
    }
    break;
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
	
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}
 
int MaskToPrefix(DWORD mask)
{
    int prefix = 0;
    for (int i = 31; i >= 0; i--)
    {
        if (mask & (1 << i))
            prefix++;
        else
            break;
    }
    return prefix;
}

DWORD PrefixToMask(int prefix)
{
    if (prefix <= 0)  return 0;
    if (prefix >= 32) return 0xFFFFFFFF;

    return (0xFFFFFFFF << (32 - prefix));
}

