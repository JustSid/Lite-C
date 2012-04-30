#include "la_loader.h"
#include "la_bmap.h"

BMAP *la_bmap_create(STRING *file)
{
    while(la_checkFile(file) == 0)
	{
	    wait(1);
	}


    return bmap_create(file);
}
