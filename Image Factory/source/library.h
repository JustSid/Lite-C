#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include "types.h"

filter_t *library_outputFilter;
BMAP *library_outputBMAP = NULL;

BOOL library_runShader(filter_t *filter, filter_data_t *data);
BOOL library_bmapCreate(filter_t *filter, filter_data_t *data);


void createOutputFilter();
void createHardHeaders();
void loadHeaders();

#include "library.c"
#endif
