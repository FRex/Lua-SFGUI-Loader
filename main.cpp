#include "InterfaceLoader.h"
#include "LuaTable.h"
#ifndef EE_NOPHYS_IN_LOADER
#include "physfs/physfs.h"
#endif
#include <vector>
#include <iostream>
#include <SFGUI/SFGUI.hpp>

void fun(const std::string& str)
{
	std::cout<<str<<std::endl;
}
void fun2(const edy::script::LuaTable& table)
{
	std::cout<<table("type").asString()<<std::endl;
}
int main(int argc,char * argv[])
{
	#ifndef EE_NOPHYS_IN_LOADER
	PHYSFS_init(argv[0]);//init physfs
	//PHYSFS_addToSearchPath("D:\\warnch\\Simple\\Simple\\edy",1);
	PHYSFS_addToSearchPath(PHYSFS_getBaseDir(),1);//add things around exe to search path
	#endif
	sf::RenderWindow app(sf::VideoMode(600,480),"LuaInterface");
	app.setFramerateLimit(60);
	app.resetGLStates();
	sfg::SFGUI sfgui;
	sfg::Desktop desk;//usuall sfg setup procedure..

	edy::gui::InterfaceLoader loader;//create instance of the loader
	loader.setCreationCallback(std::bind(fun2,std::placeholders::_2));//bind creation callback
	loader.load("test.lua");//load from that file(internally open via physfs)
	auto& it=loader.getWindows();//get vector of resuting windows
	for(unsigned i=0;i<it.size();++i) desk.Add(it[i]);//add them to desktop

	loader.setCallCallback(std::bind(fun,std::placeholders::_2));//set messages callback

	sf::Clock clo;//typicall loop:
	while(app.isOpen())
	{
		sf::Event eve;
		while(app.pollEvent(eve))
		{
			desk.HandleEvent(eve);
			if(eve.type==sf::Event::Closed) app.close();
		}
		desk.Update(clo.restart().asSeconds());
		app.clear();
		sfgui.Display(app);
		app.display();
	}
	#ifndef EE_NOPHYS_IN_LOADER
	PHYSFS_deinit();//deinit physfs
	#endif
}
