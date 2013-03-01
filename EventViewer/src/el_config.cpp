static long WINAPI ConfigDialogProc(HANDLE hDlg, int Msg,int Param1,long Param2)
{
  return Info.DefDlgProc(hDlg,Msg,Param1,Param2);
}

enum
{
  CONFIG_BORDER=0,
  CONFIG_ADDDISK,
  CONFIG_DISKHOTKEY,
  CONFIG_DISKHOTKEYLABEL,
  CONFIG_ADDMENU,
  CONFIG_SEP1,
  CONFIG_BROWSEEVT,
  CONFIG_STRIPEXT,
  CONFIG_RESTORE,
  CONFIG_SEP2,
  CONFIG_FORWARD,
  CONFIG_BACKWARD,
  CONFIG_SHOWHEADER,
  CONFIG_SHOWDESC,
  CONFIG_SHOWDATA,
  CONFIG_SEP3,
  CONFIG_VSEP,
  CONFIG_PREFIXLABEL,
  CONFIG_PREFIX,
  CONFIG_SEP4,
  CONFIG_SAVE,
  CONFIG_CANCEL,
};

static int Config()
{
  //Show dialog
  /*
    0000000000111111111122222222223333333333444444444455555555556666666666777777
    0123456789012345678901234567890123456789012345678901234567890123456789012345
  00                                                                            00
  01   浜様様様様様様様様様様様様� Event viewer 様様様様様様様様様様様様様様�   01
  02   � [x] Add to Disks menu                                              �   02
  03   �   2 Disks menu hotkey ('1'-'9'). Leave empty to autoassign         �   03
  04   � [ ] Add to Plugins menu                                            �   04
  05   把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超   05
  06   � [ ] Browse .evt files                                              �   06
  07   � [ ] Strip second extension                                         �   07
  08   把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超   08
  09   � (*) Scan forward                                                   �   09
  10   � ( ) Scan backward                                                  �   10
  11   把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超   11
  12   � Command line prefix to start the nt events:                        �   12
  13   � evt                                                                �   13
  14   把陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳超   14
  15   �                         [ Ok ]  [ Cancel ]                         �   15
  16   藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕   16
  17                                                                            17
    0000000000111111111122222222223333333333444444444455555555556666666666777777
    0123456789012345678901234567890123456789012345678901234567890123456789012345
  */

  static struct InitDialogItem InitItems[]={
  /* 0*/  {DI_DOUBLEBOX,3,1,72,18,0,0,0,0,(wchar_t *)mName},
  /* 1*/  {DI_CHECKBOX,5,2,0,0,0,0,0,0,(wchar_t *)mConfigAddToDisksMenu},
  /* 2*/  {DI_FIXEDIT,7,3,7,3,1,0,0,0,L""},
  /* 3*/  {DI_TEXT,9,3,0,0,0,0,0,0,(wchar_t *)mConfigDisksMenuDigit},
  /* 4*/  {DI_CHECKBOX,5,4,0,0,0,0,0,0,(wchar_t *)mConfigAddToPluginMenu},
  /* 5*/  {DI_TEXT,-1,5,0,0,0,0,DIF_SEPARATOR,0,L""},
  /* 6*/  {DI_CHECKBOX,5,6,0,0,0,0,0,0,(wchar_t *)mConfigBrowseEvtFiles},
  /* 7*/  {DI_CHECKBOX,5,7,0,0,0,0,0,0,(wchar_t *)mConfigStripExtension},
  /* 7*/  {DI_CHECKBOX,5,8,0,0,0,0,0,0,(wchar_t *)mConfigRestore},
  /* 8*/  {DI_TEXT,-1,9,0,0,0,0,DIF_SEPARATOR,0,L""},
  /* 9*/  {DI_RADIOBUTTON,5,10,0,0,0,0,DIF_GROUP,0,(wchar_t *)mConfigScan1},
  /*10*/  {DI_RADIOBUTTON,5,11,0,0,0,0,0,0,(wchar_t *)mConfigScan2},
  /*10*/  {DI_CHECKBOX,39,10,0,0,0,0,0,0,(wchar_t *)mConfigShowHeader},
  /*10*/  {DI_CHECKBOX,39,11,0,0,0,0,0,0,(wchar_t *)mConfigShowDescription},
  /*10*/  {DI_CHECKBOX,39,12,0,0,0,0,0,0,(wchar_t *)mConfigShowData},
  /*11*/  {DI_TEXT,-1,13,0,0,0,0,DIF_SEPARATOR,0,L""},
  /* 3*/  {DI_VTEXT,37,9,0,0,0,0,0,0,L"xxxxx"},
  /*12*/  {DI_TEXT,5,14,0,0,0,0,0,0,(wchar_t *)mConfigPrefix},
  /*13*/  {DI_FIXEDIT,5,15,19,12,0,(int)L"AAAAAAAAAAAAAAA",DIF_MASKEDIT,0,L""},
  /*14*/  {DI_TEXT,-1,16,0,0,0,0,DIF_SEPARATOR,0,L""},
  /*15*/  {DI_BUTTON,0,17,0,0,0,0,DIF_CENTERGROUP,1,(wchar_t *)mConfigSave},
  /*16*/  {DI_BUTTON,0,17,0,0,0,0,DIF_CENTERGROUP,0,(wchar_t *)mConfigCancel}
  };
  struct FarDialogItem DialogItems[ArraySize(InitItems)];
  InitDialogItems(InitItems,DialogItems,ArraySize(InitItems));
  TCHAR DisksMenuDigitText[21],PrefixText[21];

  DialogItems[CONFIG_ADDDISK].Selected=Opt.AddToDisksMenu;
  if (Opt.DisksMenuDigit)
  {
    FSF.sprintf(DisksMenuDigitText,L"%d",Opt.DisksMenuDigit);
    DialogItems[CONFIG_DISKHOTKEY].PtrData=DisksMenuDigitText;
  }
  DialogItems[CONFIG_ADDMENU].Selected=Opt.AddToPluginsMenu;
  DialogItems[CONFIG_BROWSEEVT].Selected=Opt.BrowseEvtFiles;
  DialogItems[CONFIG_STRIPEXT].Selected=Opt.StripExt;
  DialogItems[CONFIG_RESTORE].Selected=Opt.Restore;
  DialogItems[CONFIG_FORWARD+Opt.ScanType].Selected=1;
  FSF.sprintf(PrefixText,L"%s",Opt.Prefix);
  DialogItems[CONFIG_PREFIX].PtrData=PrefixText;
  DialogItems[CONFIG_SHOWHEADER].Selected=QVOpt.ShowHeader;
  DialogItems[CONFIG_SHOWDESC].Selected=QVOpt.ShowDescription;
  DialogItems[CONFIG_SHOWDATA].Selected=QVOpt.ShowData;

  CFarDialog dialog;
  int DlgCode=dialog.Execute(Info.ModuleNumber,-1,-1,76,20,L"Config",DialogItems,ArraySize(DialogItems),0,0,ConfigDialogProc,0);
  if (DlgCode!=CONFIG_SAVE)
    return(FALSE);
  Opt.AddToDisksMenu=dialog.Check(CONFIG_ADDDISK);
  Opt.DisksMenuDigit=FSF.atoi(dialog.Str(CONFIG_DISKHOTKEY));
  Opt.AddToPluginsMenu=dialog.Check(CONFIG_ADDMENU);
  Opt.BrowseEvtFiles=dialog.Check(CONFIG_BROWSEEVT);
  Opt.StripExt=dialog.Check(CONFIG_STRIPEXT);
  Opt.Restore=dialog.Check(CONFIG_RESTORE);
  QVOpt.ShowHeader=dialog.Check(CONFIG_SHOWHEADER);
  QVOpt.ShowDescription=dialog.Check(CONFIG_SHOWDESC);
  QVOpt.ShowData=dialog.Check(CONFIG_SHOWDATA);
  wcscpy(Opt.Prefix,dialog.Str(CONFIG_PREFIX));
  FSF.Trim(Opt.Prefix);
  int i=CONFIG_FORWARD;
  Opt.ScanType=0;
  while(!dialog.Check(i)) {i++; Opt.ScanType++;}
  HKEY hKey;
  DWORD Disposition;
  if((RegCreateKeyEx(HKEY_CURRENT_USER,PluginRootKey,0,NULL,0,KEY_WRITE,NULL,&hKey,&Disposition))==ERROR_SUCCESS)
  {
    RegSetValueEx(hKey,L"AddToDisksMenu",0,REG_DWORD,(LPBYTE)&Opt.AddToDisksMenu,sizeof(Opt.AddToDisksMenu));
    RegSetValueEx(hKey,L"DisksMenuDigit",0,REG_DWORD,(LPBYTE)&Opt.DisksMenuDigit,sizeof(Opt.DisksMenuDigit));
    RegSetValueEx(hKey,L"AddToPluginsMenu",0,REG_DWORD,(LPBYTE)&Opt.AddToPluginsMenu,sizeof(Opt.AddToPluginsMenu));
    RegSetValueEx(hKey,L"BrowseEvtFiles",0,REG_DWORD,(LPBYTE)&Opt.BrowseEvtFiles,sizeof(Opt.BrowseEvtFiles));
    RegSetValueEx(hKey,L"StripExt",0,REG_DWORD,(LPBYTE)&Opt.StripExt,sizeof(Opt.StripExt));
    RegSetValueEx(hKey,L"ScanType",0,REG_DWORD,(LPBYTE)&Opt.ScanType,sizeof(Opt.ScanType));
    RegSetValueEx(hKey,L"Prefix",0,REG_SZ,(LPBYTE)Opt.Prefix,(wcslen(Opt.Prefix)+1)*sizeof(TCHAR));

    RegSetValueEx(hKey,L"Restore",0,REG_DWORD,(LPBYTE)&Opt.Restore,sizeof(Opt.Restore));
    RegSetValueEx(hKey,L"ShowHeader",0,REG_DWORD,(LPBYTE)&QVOpt.ShowHeader,sizeof(QVOpt.ShowHeader));
    RegSetValueEx(hKey,L"ShowDescription",0,REG_DWORD,(LPBYTE)&QVOpt.ShowDescription,sizeof(QVOpt.ShowDescription));
    RegSetValueEx(hKey,L"ShowData",0,REG_DWORD,(LPBYTE)&QVOpt.ShowData,sizeof(QVOpt.ShowData));
    RegCloseKey(hKey);
  }
  return(TRUE);
}
