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
#include "source\\noisescript.h"


const char *testscript = "var a; var c; a = 10 * c;";


void main()
{
	variable_t *a = variable_createFloat(0.0);
	variable_t *c = variable_createFloat(10.0);
	
	noisescript_t *script = ns_parseBytes(testscript);
	ns_bindVariable(script, a, "a");
	ns_bindVariable(script, c, "c");
	
	ns_runScript(script);
	
	
	printf("Result: %f", (double)variable_getFloat(a));
}
