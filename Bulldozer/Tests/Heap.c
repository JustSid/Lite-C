typedef struct
{
    void *pointer;
} TESTSTRUCT;


TESTSTRUCT *createStruct()
{
    TESTSTRUCT *foo = malloc_gc(sizeof(TESTSTRUCT));
    foo->pointer = malloc_gc(512);
    
    return foo;
}


void runHeapTests()
{
    TESTSTRUCT *foo = createStruct();
    
    BZCollectorCollect(0);
    // At this point, only foo is visible, but foo->pointer isn't since its just on the heap
    // Bulldozer won't collect it anyway since foo still points to the memory!
    
    foo = NULL; // And now we "lose" the reference to the pointer
    BZCollectorCollect(0); // And suddenly, the memory is gone (foo and foo->pointer)
}
