// shell.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	//º”‘ÿÕº±Í						
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_main));	
	decryptShell();

	return 0;
}



