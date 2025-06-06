#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int MaskToPrefix(DWORD mask);

DWORD PrefixToMask(int prefix);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)\
{
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
        int prefix = GetDlgItemInt(hwnd, IDC_EDIT_PREFIX, 0, FALSE);
        DWORD mask = PrefixToMask(prefix);

        // set new mask
        HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
        SendMessage(hIPmask, IPM_SETADDRESS, 0, mask);

        // upgrade value in edit control
        SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);

        return TRUE; 
    }
        break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;


		switch (LOWORD(wParam))
		{

		case IDC_IPADDRESS:
        if (HIWORD(wParam) == EN_CHANGE) //
		{
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			if (FIRST_IPADDRESS(dwIPaddress) < 128) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFF000000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 192) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 224) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);

            // upgrade prefix when mask change
            SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
            int prefix = MaskToPrefix(dwIPmask);
            SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);
		}
		break;
        case IDC_EDIT_PREFIX:
        if (HIWORD(wParam) == EN_CHANGE) //
        {
            BOOL translated;
            int prefix = GetDlgItemInt(hwnd, IDC_EDIT_PREFIX, &translated, FALSE); // преобразует тескт указанного элемента в int
            if(translated)
            {
                DWORD mask = PrefixToMask(prefix);

                //set new mask
                SendMessage(hIPmask, IPM_SETADDRESS, 0, mask);
            }
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
