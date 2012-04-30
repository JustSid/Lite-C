#include "la_loader.h"
#include "la_entity.h"

ENTITY *la_ent_create(STRING *file, VECTOR *pos, EVENT entAction)
{
	while(la_checkFile(file) == 0)
	{
	   wait(1);
	}
	
	return ent_create(file, pos, entAction);
}

ENTITY *la_ent_createlocal(STRING *file, VECTOR *pos, EVENT entAction)
{
   while(la_checkFile(file) == 0)
	{
	   wait(1);
	}

    return ent_createlocal(file, pos, entAction);
}

ENTITY *la_ent_createlayer(STRING *file, var flags2, var layer)
{
	while(la_checkFile(file) == 0)
	{
	   wait(1);
	}
	
    return ent_createlayer(file, flags2, layer);
}
