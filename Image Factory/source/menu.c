#include <strio.c>
#include "menu.h"
#include "gui.h"

#define kMenuActionFileNew  1
#define kMenuActionFileOpen 2
#define kMenuActionFileSave 3
#define kMenuActionFileExit 4

#define kMenuActionWorkspaceExport 			10
#define kMenuActionWorkspaceInsertNumber 	11
#define kMenuActionWorkspaceInsertImage 	12

#define kMenuActionFilterSave 20
#define kMenuActionFilterLoad 21

void menu_create()
{
	long menu;
	long fileMenu, imageMenu, filterMenu;

	menu = CreateMenu();
	fileMenu = CreateMenu();
	imageMenu = CreateMenu();
	filterMenu = CreateMenu();

	InsertMenu(fileMenu, 0, MF_BYPOSITION | MF_STRING, kMenuActionFileNew, "New");
	InsertMenu(fileMenu, 1, MF_BYPOSITION | MF_STRING, kMenuActionFileOpen, "Open");
	InsertMenu(fileMenu, 2, MF_BYPOSITION | MF_STRING, kMenuActionFileSave, "Save");
	InsertMenu(fileMenu, 3, MF_BYPOSITION | MF_STRING, kMenuActionFileExit, "Exit");
	
	InsertMenu(imageMenu, 0, MF_BYPOSITION | MF_STRING, kMenuActionWorkspaceExport, "Save final image");
	InsertMenu(imageMenu, 1, MF_BYPOSITION | MF_STRING, kMenuActionWorkspaceInsertNumber, "Insert number");
	InsertMenu(imageMenu, 2, MF_BYPOSITION | MF_STRING, kMenuActionWorkspaceInsertImage, "Insert image");
	
	InsertMenu(filterMenu, 0, MF_BYPOSITION | MF_STRING, kMenuActionWorkspaceExport, "Save as filter");
	InsertMenu(filterMenu, 1, MF_BYPOSITION | MF_STRING, kMenuActionWorkspaceInsertNumber, "Load custom filter");

	InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING | MF_POPUP, fileMenu, "File");
	InsertMenu(menu, 1, MF_BYPOSITION | MF_STRING | MF_POPUP, imageMenu, "Workspace");
	InsertMenu(menu, 2, MF_BYPOSITION | MF_STRING | MF_POPUP, filterMenu, "Filter");
	
	SetMenu(hWnd, menu);
}

void menu_setOpenFile(const char *file)
{
	STRING *title = str_create("Image Factory - ");
	str_cat(title, file);
	
	video_window(NULL, NULL, 0, title);
}

long menu_process(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_COMMAND)
	{ 
		switch(wParam)
		{
			case kMenuActionFileExit:
				sys_exit(NULL);
				break;
			
			case kMenuActionWorkspaceExport:
			{
				BMAP *bmap = library_outputBMAP;
				if(!bmap)
				{
					printf("You need to create an image first...");
					break;
				}

				STRING *file = file_dialog_save("Save to...", "*.jpg;*.png;*.bmp");
				if(!file)
					break;
				
				bmap_save(bmap, file);
				SetCurrentDirectory(_chr(work_dir));
				break;
			}
			
			case kMenuActionWorkspaceInsertNumber:
			{
				generateUserVariable();
				break;
			}
				
			case kMenuActionWorkspaceInsertImage:
			{
				STRING *file = file_dialog("Select image", "*.jpg;*.png;*.bmp;*.tga");
				if(!file)
					break;
				
				BMAP *bmap = bmap_create(file);
				
				if(bmap)
					generateUserImagePreview(bmap);

				SetCurrentDirectory(_chr(work_dir));
				break;
			}
		}
		
		return 1;
	}
	
	return 0;
}
