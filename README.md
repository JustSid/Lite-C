### Overview
This is a collection of some source code that I made for Lite-C which is part of the Gamestudio Authoring system. The license for all projects is the MIT license, so you are free to do whatever you want to do with it.

### libAsset
This project was born when someone asked for an asynchronous way to load models in Lite-C, its fairly basic but shows how to implement a queue with threads, load files into buffers and how to use them with Gamestudio. There are some missing things, for example when you try to load a model with external textures, the textures must be either loaded by you in the background, or there will be a hickup when the engine instantiates the model from the buffer and then loads the textures on the main thread.

### Bulldozer
A simple prove of conecept Garbage Collector written in Lite-C and some C inside of an dll. It is a nice demonstration project to see how to use "normal" dlls and the WinAPI to dynamically link functions with exported symbols from it, but can't be used as a real garbage collector due to limitations in Lite-C. For example, the GC works by getting the address of the current stack frame and then working up to a known address at the top of the stack, however, the "known" address from the struct is extracted in the Lite-Cs main() function which sadly returns so its not a really good idea to assume that the address is still part of the stack. It also fails to work with wait(), since I can't check the stack snapshot the function creates and might end up freeing memory that shouldn't be freed.

### Lite Foundation
A OOP library with classes for everyday tasks, heavily influenced by the Foundation and Core Foundation frameworks on Mac OS X.