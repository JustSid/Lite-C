#include <acknex.h>
#include "Bulldozer.h"

#include "Tests\\Heap.c"
#include "Tests\\Stack.c"

void main()
{
	BZInit(); // Initialize Bulldozer!
	
	
	runHeapTests(); // Defined in Heap.c
	runStackTests(); // Defined in Stack.c
	
	BZCollectorCollect(0); // Collect all dangling pointers...
	
	sys_exit(NULL);
}
