#ifndef _GUI_H_
#define _GUI_H_

#include "types.h"

void initGUI();
void updateGUI();
void generateUserImagePreview(BMAP *bmap);
void generateUserImagePreview(BMAP *bmap, int posX, int posY);
void generateFunctionPanel(filter_header_t* header, int xPos, int yPos);
void generateUserVariable();
void generateUserVariable(int posX, int posY);
void generateUserVariable(float value, int posX, int posY);
void generateSidebarEntry(filter_header_t* entry);


#include "gui.c"

#endif /* _GUI_H_ */