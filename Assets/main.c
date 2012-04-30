#include <acknex.h>
#include "libAsset\\libAsset.h"

ENTITY *stone, *marine;

action rotate()
{
	while(1)
	{
		my.pan += 12 * time_step;
		//my.tilt += 9 * time_step;
		
		wait(1);
	}
}



void main()
{
	la_assetInit();
	level_load(NULL);	
	
	wait(1); // Wait until the video device is ready
	
	
	// A stone
	stone = la_ent_create("stones.mdl", vector(1024, 0, -120), NULL);
	// A marine
	marine = la_ent_create("marine.mdl", vector(1024, 0, 40), rotate);
	

	// Ten boxes full of evil madness
	int i;
	for(i=0; i<10; i++)
	{
		la_ent_create("box.mdl", vector(2048, -820 + (i * 180), -35), NULL);
	}
}
