# OO
Object Organizer 1996 - A multi-OS object engine

The object engine is made of objects in C language, with some objects specific to each OS when necessary.
Applications source code can then be OS free by using OO objects.
For now, the engine and the tests compile and run on Amiga and Windows.

OO is based on ideas from Boopsi and goes farther in object handling, events notifications, building applications, portability...
Objects have single inheritance (like java, unlike c++).
Entire applications can be OO driven, even with no UI.
Entire application object tree can be declared as just data, then let OO instantiate all.
The next planned step was to read the application description from a text file, and transform it to a data objects tree. All this opens the door for an application builder...
A 3D engine is provided as objects, like handler, space, solid, camera (simple), light, mike, speaker, ...
Objects attributes modifications can send notifications to others, with attributs mappings, functions mappings, to achieve automatic application processing.

Even if the code is well tested the product was not released because of Commodore stopping Amiga.
