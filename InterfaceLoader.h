#pragma once
#define EE_NOPHYS_IN_LOADER // comment this line out to make text and images load via physics
#include <vector>
#include <map>
#include <string>
#include <SFML/Graphics/Image.hpp>
#include <SFGUI/Window.hpp>
#include <SFGUI/Table.hpp>
#include <SFGUI/Image.hpp>
#include <SFGUI/Adjustment.hpp>
#include <SFGUI/RadioButton.hpp>
/*
neat class to manage simple guis that dont have tooooo tooo much shinyness and shit to them, there is 
huge consraint: window with table in it is base for everything, but there can be several,
also there is messages and creation callbacks so its possible to catch right widgets and do
w.e with them after storing their shared ptr, the binds and calls take weak ptr so that there is no
strong counted reference from bind to widget

also, this class has HORRIBLE length of some lines and is generally spammed with methods for creation etc.
that allow for mapping string to them instead of if else chain nonsense
*/
namespace edy{
	namespace script{
		class LuaTable;
	}
	namespace gui{
		class InterfaceLoader
		{
		public:
			typedef sfg::WeakPtr<sfg::Object>		WeakPtr;
			typedef std::function<void(WeakPtr,const std::string& message)> CallFun;
			typedef std::function<void(sfg::Widget::Ptr,const script::LuaTable&)> CreationFun;
													InterfaceLoader();
			bool									load(const char * filename,bool clearlast=true);
			void									clear();
			const std::vector<sfg::Window::Ptr>&	getWindows()const{return m_Windows;}
			void									setCreationCallback(CreationFun callback);
			void									setCallCallback(CallFun callback);
		private:
			//typedefs for map values and stuff
			typedef void(InterfaceLoader::*WidgetAdder)(const script::LuaTable&);//looks lolzy..
			typedef sfg::Signal::SignalID*			CallAdder;
			typedef sfg::SharedPtr<sfg::RadioButton::RadioButtonGroup> GroupPtr;
			//convenience function:
			sfg::Table::Ptr							getLastTable();
			void									addWindow(const script::LuaTable& windesc);
			//window will call these to populate its table:
			void									addSpinner(const script::LuaTable& desc);
			void									addEntry(const script::LuaTable& desc);
			void									addSpinButton(const script::LuaTable& desc);
			void									addImage(const script::LuaTable& desc);
			void									addLabel(const script::LuaTable& desc);
			void									addProgressBar(const script::LuaTable& desc);
			void									addScale(const script::LuaTable& desc);
			void									addScrollbar(const script::LuaTable& desc);
			void									addButton(const script::LuaTable& desc);
			void									addToggleButton(const script::LuaTable& desc);
			void									addCheckButton(const script::LuaTable& desc);
			void									addRadioButton(const script::LuaTable& desc);
			void									addComboBox(const script::LuaTable& desc);

			sfg::Image::Ptr							getImage(const std::string& name);
			sfg::Adjustment::Ptr					getAdjusment(const script::LuaTable& desc);
			GroupPtr								getGroup(const std::string& name);
			//callbacks:
			CallFun									m_MessageCallback;
			CreationFun								m_CreationCallback;
			//widgets will call these to add callbacks:
			void									addCallbacks(sfg::Widget::Ptr widget,const script::LuaTable& desc);
			//for callbacks:
			void									call(WeakPtr ptr,const std::string& call);
			//member vars:
			std::vector<sfg::Window::Ptr>			m_Windows;
			std::map<std::string,WidgetAdder>		m_WidgetAdders;
			std::map<std::string,CallAdder>			m_CallbackAdders;
			std::map<std::string,sf::Image>			m_Images;
			std::map<std::string,GroupPtr>			m_RadioGroups;
		};
	}
}