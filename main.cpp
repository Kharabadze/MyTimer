#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "resource.h"

HINSTANCE hInst;

struct timerdata_t{
    unsigned int h,m,s;
    unsigned int oh,om,os;
    //unsigned int total_time;
    bool ring;

    void from_dlg(HWND hwndDlg);
    void to_dlg(HWND hwndDlg);

} timerdata;
void timerdata_t::from_dlg(HWND hwndDlg){
    char buffer[256];
        GetDlgItemTextA(hwndDlg,0x101,buffer,5);
        h=atoi(buffer);
        GetDlgItemTextA(hwndDlg,0x102,buffer,3);
        m=atoi(buffer);
        GetDlgItemTextA(hwndDlg,0x103,buffer,3);
        s=atoi(buffer);
    return;
}
void timerdata_t::to_dlg(HWND hwndDlg){
    char buffer[256];
        sprintf(buffer,"%04i",h);
        SetDlgItemTextA(hwndDlg,0x101,buffer);
        sprintf(buffer,"%02i",m);
        SetDlgItemTextA(hwndDlg,0x102,buffer);
        sprintf(buffer,"%02i",s);
        SetDlgItemTextA(hwndDlg,0x103,buffer);
    return;
}

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //char buffer[256];
    switch(uMsg)
    {
    case WM_INITDIALOG:
    {
        timerdata.ring = false;
        timerdata.oh=timerdata.h=0;
        timerdata.om=timerdata.m=5;
        timerdata.os=timerdata.s=0;
        //put
        timerdata.to_dlg(hwndDlg);

        EnableWindow(GetDlgItem(hwndDlg,0x105),false);
    }
    return TRUE;

    case WM_CLOSE:
    {
        EndDialog(hwndDlg, 0);
    }
    return TRUE;

    case WM_COMMAND:
    {
        switch(LOWORD(wParam))
        {
            case 0x104:{//START
                //Get data
                timerdata.from_dlg(hwndDlg);
                //Test zero
                if((timerdata.h==0)&&(timerdata.m==0)&&(timerdata.s==0))break;
                //Correct data
                timerdata.m+=timerdata.s/60;timerdata.s%=60;//correct sec
                timerdata.h+=timerdata.m/60;timerdata.m%=60;//correct min
                //Remember data
                timerdata.oh=timerdata.h;
                timerdata.om=timerdata.m;
                timerdata.os=timerdata.s;
                //Put Data
                timerdata.to_dlg(hwndDlg);
                //Start countdown
                timerdata.ring=false;
                SetTimer(hwndDlg, 13, 1000, NULL);
                EnableWindow(GetDlgItem(hwndDlg,0x104),false);
                EnableWindow(GetDlgItem(hwndDlg,0x105),true);
                break;
            }
            case 0x105:{//STOP
                if(timerdata.ring){
                    KillTimer(hwndDlg,14);
                    timerdata.ring=false;
                }else{
                    KillTimer(hwndDlg,13);
                }
                EnableWindow(GetDlgItem(hwndDlg,0x104),true);
                EnableWindow(GetDlgItem(hwndDlg,0x105),false);
                break;
            }
            case 0x106:{//RESET
                timerdata.h=timerdata.oh;
                timerdata.m=timerdata.om;
                timerdata.s=timerdata.os;
                timerdata.to_dlg(hwndDlg);
                if(timerdata.ring){
                    KillTimer(hwndDlg,14);
                    timerdata.ring=false;
                    EnableWindow(GetDlgItem(hwndDlg,0x104),true);
                    EnableWindow(GetDlgItem(hwndDlg,0x105),false);
                }
                break;
            }

        }
    }
    return TRUE;

   	case WM_TIMER:{
		if(wParam==13){//Counting
            if(timerdata.s>0){
                timerdata.s--;
            }else if(timerdata.m>0){
                timerdata.s=59;
                timerdata.m--;
            }else if(timerdata.h>0){
                timerdata.s=59;
                timerdata.m=59;
                timerdata.h--;
            }
            //Out
            timerdata.to_dlg(hwndDlg);
            //process ringing
            if((timerdata.h==0)&&(timerdata.m==0)&&(timerdata.s==0)){
                KillTimer(hwndDlg,13);
                SetForegroundWindow(hwndDlg);
                SetActiveWindow(hwndDlg);
                timerdata.ring=true;
                SetTimer(hwndDlg, 14, 500, NULL);
            }
		}else if(wParam==14){//Ring
		    PlaySound("SystemAsterisk", NULL, SND_ASYNC);
		    //HBRUSH hb = ::CreateSolidBrush(RGB(222,231,249));
            //hwndDlg.hbrBackground = hb;
		}

    }
    return TRUE;
    }

    return FALSE;
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}
