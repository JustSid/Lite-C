#include "la_loader.h"
#include "la_sound.h"

SOUND *la_snd_create(STRING *file)
{
    while(la_checkFile(file) == 0)
	{
	    wait(1);
	}

    return snd_create(file);
}