#ifndef _MENU_H_
#define _MENU_H_

void menu_create();
void menu_setOpenFile(const char *file);
long menu_process(UINT message, WPARAM wParam, LPARAM lParam);

#include "menu.c"
#endif
