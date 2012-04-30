#include <acknex.h>
#include <stdio.h>
#include <windows.h>  
#include <default.c>

#define PRAGMA_PATH "gfx";
#define PRAGMA_PATH "shaders";

#include "source\\types.h"
#include "source\\library.h"
#include "source\\gui.h"
#include "source\\menu.h"


void main()
{
    RECT clientRect, windowRect;
    POINT diff;
	fps_max			=	60;
	mouse_pointer	=	 2;
	mouse_mode		=	 4;
	on_scanmessage	= menu_process;

	vec_set(screen_color, vector(77,64,58));
	wait(1);
	
    GetClientRect(hWnd, &clientRect);
    GetWindowRect(hWnd, &windowRect);
  
    diff.x = (windowRect.right - windowRect.left) - clientRect.right;
    diff.y = (windowRect.bottom - windowRect.top) - clientRect.bottom;

	
	video_set(1280, 720, 0, 2);
	video_window(NULL, vector(1280 + diff.x, 720 + diff.y + sys_metrics(SM_CYMENU), 0), 16 + 32, "Image Factory");
	

	createOutputFilter();
	
	initGUI();
	menu_create();
	menu_setOpenFile("Untitled");
	
	createHardHeaders();
	loadHeaders();
	//read_ndefHeader();

	while(1)
	{
		updateGUI();
		proc_mode = PROC_LATE;
		wait(1);
	}
}
