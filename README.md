Lua-SFGUI-Loader
================
Easy to use loader of Lua files that creates a table filled with desired wigets.

This code is under zlib license, see http://opensource.org/licenses/Zlib for full text. <br />
It depends on: <br />
SFML - zlib license http://www.sfml-dev.org/  <br />
SFGUI - zlib license http://sfgui.sfml-dev.de/ <br />
Lua - Mit license http://www.lua.org/ <br />
PhysicsFS - zlib license http://icculus.org/physfs/ <br />
 <br />
 Physiscs dependancy is not necessary, if you don't comment the #define in InterfaceLoader.h you'll not be dependand on it.
 You'll also not need Phys*.h and .cpp files if you don't use Physics.
 Also, you need to recompile SFGUI with a nonbreaking change that adds std::bind to Signals, as explained here:<br />
 http://en.sfml-dev.org/forums/index.php?topic=6112.msg75955#msg75955
