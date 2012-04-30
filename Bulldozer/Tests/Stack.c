void stackTest()
{
    malloc_gc(512);
    // We allocate memory but lose any reference to it immediatly, normally this is a leak!
}

void runStackTests()
{
    stackTest();
    BZCollectorCollect(0); // Collect the dangling pointer we created in stackTest()!
}