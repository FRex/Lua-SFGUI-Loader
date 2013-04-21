#include "InterfaceLoader.h"
#include <SFGUI/SFGUI.hpp>
#include "PhysAsciiFile.h"
#include "LuaTable.h"
//#include <edy/core/binaryflags.h>
#include "binaryflags.h"
#include "PhysInputStream.h"
#define	EE_CALLMEMBER(instance,funptr) ((instance).*(funptr))
namespace {
	class AttachmentOptions
	{
	public:
		//AttachmentOptions():Optx(3),Opty(3),Rect(0,0,1,1),Padding(){}//careful with bitflags
		AttachmentOptions(const edy::script::LuaTable& desc);
		int							Optx;//3 means both fill and expand
		int							Opty;//as above
		sf::Rect<sf::Uint32>		Rect;
		sf::Vector2f				Padding;
	};
	AttachmentOptions::AttachmentOptions(const edy::script::LuaTable& tab):
	Optx(3),Opty(3),Rect(0,0,1,1),Padding()
	{
		tab("x").tryNumber(Rect.left);
		tab("spanx").tryNumber(Rect.width);
		tab("y").tryNumber(Rect.top);
		tab("spany").tryNumber(Rect.height);
		tab("optx").tryNumber(Optx);
		tab("opty").tryNumber(Opty);
		tab("paddingx").tryNumber(Padding.x);
		tab("paddingy").tryNumber(Padding.y);
	}
	void getAttachmentOptions(AttachmentOptions& options,const edy::script::LuaTable& tab)
	{
		tab("x").tryNumber(options.Rect.left);
		tab("spanx").tryNumber(options.Rect.width);
		tab("y").tryNumber(options.Rect.top);
		tab("spany").tryNumber(options.Rect.height);
		tab("optx").tryNumber(options.Optx);
		tab("opty").tryNumber(options.Opty);
		tab("paddingx").tryNumber(options.Padding.x);
		tab("paddingy").tryNumber(options.Padding.y);
	}
	inline void attachWithOpts(sfg::Table::Ptr table,sfg::Widget::Ptr widget,const AttachmentOptions& options)
	{
		table->Attach(widget,options.Rect,options.Optx,options.Opty,options.Padding);
	}
}
namespace edy{
	namespace gui{
		InterfaceLoader::InterfaceLoader()
		{
			//register widget adders:
			m_WidgetAdders["Spinner"]=&InterfaceLoader::addSpinner;
			m_WidgetAdders["Entry"]=&InterfaceLoader::addEntry;
			m_WidgetAdders["SpinButton"]=&InterfaceLoader::addSpinButton;
			m_WidgetAdders["Image"]=&InterfaceLoader::addImage;
			m_WidgetAdders["Label"]=&InterfaceLoader::addLabel;
			m_WidgetAdders["Progressbar"]=&InterfaceLoader::addProgressBar;
			m_WidgetAdders["Scale"]=&InterfaceLoader::addScale;
			m_WidgetAdders["Scrollbar"]=&InterfaceLoader::addScrollbar;
			m_WidgetAdders["Button"]=&InterfaceLoader::addButton;
			m_WidgetAdders["ToggleButton"]=&InterfaceLoader::addToggleButton;
			m_WidgetAdders["CheckButton"]=&InterfaceLoader::addCheckButton;
			m_WidgetAdders["RadioButton"]=&InterfaceLoader::addRadioButton;
			m_WidgetAdders["ComboBox"]=&InterfaceLoader::addComboBox;
			//register callback adders:
			m_CallbackAdders["OnGainFocus"]=&sfg::Widget::OnGainFocus;
			m_CallbackAdders["OnKeyPress"]=&sfg::Widget::OnKeyPress;
			m_CallbackAdders["OnKeyRelease"]=&sfg::Widget::OnKeyRelease;
			m_CallbackAdders["OnLeftClick"]=&sfg::Widget::OnLeftClick;
			m_CallbackAdders["OnLostFocus"]=&sfg::Widget::OnLostFocus;
			m_CallbackAdders["OnMouseEnter"]=&sfg::Widget::OnMouseEnter;
			m_CallbackAdders["OnMouseLeave"]=&sfg::Widget::OnMouseLeave;
			m_CallbackAdders["OnMouseLeftPress"]=&sfg::Widget::OnMouseLeftPress;
			m_CallbackAdders["OnMouseLeftRelease"]=&sfg::Widget::OnMouseLeftRelease;
			m_CallbackAdders["OnMouseMove"]=&sfg::Widget::OnMouseMove;
			m_CallbackAdders["OnMouseRightPress"]=&sfg::Widget::OnMouseRightPress;
			m_CallbackAdders["OnMouseRightRelease"]=&sfg::Widget::OnMouseRightRelease;
			m_CallbackAdders["OnRightClick"]=&sfg::Widget::OnRightClick;
			m_CallbackAdders["OnSizeAllocate"]=&sfg::Widget::OnSizeAllocate;
			m_CallbackAdders["OnSizeRequest"]=&sfg::Widget::OnSizeRequest;
			m_CallbackAdders["OnStateChange"]=&sfg::Widget::OnStateChange;
			m_CallbackAdders["OnText"]=&sfg::Widget::OnText;
		}
		bool InterfaceLoader::load(const char * filename,bool clearlast)
		{
			if(clearlast) clear();
			script::LuaTable table;
			{
				io::PhysAsciiFile file(filename);
				if(!file.loaded()) return false;
				table.loadFromString(file.getContents());
			}
			for(auto it=table.begin();it!=table.end();++it)
			{
				if(it->second("type")=="Window")
				{
					addWindow(it->second);
				}
			}
			m_Images.clear();
			m_RadioGroups.clear();
			return true;
		}
		void InterfaceLoader::clear()
		{
			m_Windows.clear();//is that all?
			//add clearing here
		}
		void InterfaceLoader::setCreationCallback(CreationFun callback)
		{
			m_CreationCallback=callback;
		}
		void InterfaceLoader::setCallCallback(CallFun callback)
		{
			m_MessageCallback=callback;
		}
		sfg::Table::Ptr InterfaceLoader::getLastTable()
		{
			return sfg::DynamicPointerCast<sfg::Table>(m_Windows.back()->GetChild());
		}
		void InterfaceLoader::addWindow(const script::LuaTable& windesc)
		{
			int style=sfg::Window::TOPLEVEL;//dafult 
			bool tmp;
			if(windesc("background").tryBool(tmp)) style=core::assignField(style,1,tmp);
			if(windesc("titlebar").tryBool(tmp)) style=core::assignField(style,0,tmp);
			if(windesc("resize").tryBool(tmp)) style=core::assignField(style,2,tmp);
			if(windesc("shadow").tryBool(tmp)) style=core::assignField(style,3,tmp);

			sfg::Window::Ptr window=sfg::Window::Create(style);
			window->SetTitle(windesc("caption").asString());
			m_Windows.push_back(window);
			sf::Vector2f pos;
			pos.x=static_cast<float>(windesc("x").asNumber());
			pos.y=static_cast<float>(windesc("y").asNumber());
			window->SetPosition(pos);

			sfg::Table::Ptr table=sfg::Table::Create();
			window->Add(table);
			table->SetRowSpacings(static_cast<float>(windesc("rowspacing").asNumber()));
			table->SetColumnSpacings(static_cast<float>(windesc("colspacing").asNumber()));

			const script::LuaTable& widgets=windesc("widgets");

			for(auto it=widgets.begin();it!=widgets.end();++it)
			{
				const std::string& type=it->second("type").asString();
				auto find=m_WidgetAdders.find(it->second("type").asString());
				if(find!=m_WidgetAdders.end())
				{
					EE_CALLMEMBER(*this,m_WidgetAdders[type])(it->second);
				}
			}


		}//::addWindow
		void InterfaceLoader::addCallbacks(sfg::Widget::Ptr widget,const script::LuaTable& desc)
		{
			for(auto it=desc.begin();it!=desc.end();++it)
			{
				auto find=m_CallbackAdders.find(it->first);
				if(find!=m_CallbackAdders.end())
				{
					widget->GetSignal(*(find->second)).Connect(std::bind(&InterfaceLoader::call,this,WeakPtr(widget),it->second.asString()));
				}
			}
			std::string clid;
			if(desc("class").tryString(clid)) widget->SetClass(clid);
			if(desc("id").tryString(clid)) widget->SetId(clid);
			if(m_CreationCallback) m_CreationCallback(widget,desc);
		}
		void InterfaceLoader::call(WeakPtr ptr,const std::string& call)
		{
			if(m_MessageCallback) m_MessageCallback(ptr,call);
		}
		//adders:
		void InterfaceLoader::addSpinner(const script::LuaTable& desc)
		{
			sfg::Spinner::Ptr spin=sfg::Spinner::Create();
			
			if(desc("start").asBoolean()) spin->Start();
			attachWithOpts(getLastTable(),spin,AttachmentOptions(desc));
			addCallbacks(spin,desc);
		}
		void InterfaceLoader::addEntry(const script::LuaTable& desc)
		{
			sfg::Entry::Ptr entry=sfg::Entry::Create(desc("text").asString());
			entry->SetMaximumLength(static_cast<size_t>(desc("maxlength").asNumber()));

			if(desc("hideas").getType()==script::LuaTable::String)
			{
				std::string str=desc("hideas").asString();
				entry->HideText(str[0]);
			}
			else if(desc("hideas").getType()==script::LuaTable::Number)
			{
				sf::Uint32 c;
				desc("hideas").tryNumber(c);
				entry->HideText(c);
			}

			attachWithOpts(getLastTable(),entry,AttachmentOptions(desc));
			addCallbacks(entry,desc);
			std::string str;
			if(desc("OnTextChanged").tryString(str))
				entry->GetSignal(sfg::Entry::OnTextChanged).Connect(std::bind(&InterfaceLoader::call,this,entry,str));
		}
		void InterfaceLoader::addSpinButton(const script::LuaTable& desc)
		{
			sfg::SpinButton::Ptr spin;
			if(desc("adjustment").getType()==script::LuaTable::Array)
			{
				spin=sfg::SpinButton::Create(getAdjusment(desc("adjustment")));
			}
			else
			{
				float min=0.f,max=10.f,step=1.f;
				desc("min").tryNumber(min);
				desc("max").tryNumber(max);
				desc("step").tryNumber(step);
				spin=sfg::SpinButton::Create(min,max,step);
			}
			spin->SetMaximumLength(static_cast<size_t>(desc("maxlength").asNumber()));
			spin->SetText(desc("text").asString());

			attachWithOpts(getLastTable(),spin,AttachmentOptions(desc));
			addCallbacks(spin,desc);
			std::string str;
			if(desc("OnValueChanged").tryString(str))
				spin->GetSignal(sfg::SpinButton::OnValueChanged).Connect(std::bind(&InterfaceLoader::call,this,spin,str));
			if(desc("OnTextChanged").tryString(str))
				spin->GetSignal(sfg::SpinButton::OnTextChanged).Connect(std::bind(&InterfaceLoader::call,this,spin,str));
		}
		void InterfaceLoader::addImage(const script::LuaTable& desc)
		{
			std::string name;
			if(!desc("image").tryString(name)) return;

			sfg::Image::Ptr img=getImage(name);

			attachWithOpts(getLastTable(),img,AttachmentOptions(desc));
			addCallbacks(img,desc);
		}
		void InterfaceLoader::addLabel(const script::LuaTable& desc)
		{

			sfg::Label::Ptr label=sfg::Label::Create(desc("text").asString());
			attachWithOpts(getLastTable(),label,AttachmentOptions(desc));
			addCallbacks(label,desc);
		}
		void InterfaceLoader::addProgressBar(const script::LuaTable& desc)
		{
			sfg::ProgressBar::Orientation orientation=sfg::ProgressBar::HORIZONTAL;//default..
			if(desc("vertical").asBoolean())
			{
				orientation=sfg::ProgressBar::VERTICAL;
			}
			sfg::ProgressBar::Ptr progres=sfg::ProgressBar::Create(orientation);

			float frac;
			if(desc("fraction").tryNumber(frac)) progres->SetFraction(frac);

			attachWithOpts(getLastTable(),progres,AttachmentOptions(desc));
			addCallbacks(progres,desc);
		}
		void InterfaceLoader::addScale(const script::LuaTable& desc)
		{
			sfg::Scale::Ptr scale;
			sfg::Scale::Orientation orient=sfg::Scale::HORIZONTAL;
			if(desc("vertical").asBoolean()) orient=sfg::Scale::VERTICAL;
			if(desc("adjustment").getType()==script::LuaTable::Array)
			{
				scale=sfg::Scale::Create(orient);
				scale->SetAdjustment(getAdjusment(desc("adjustment")));
			}
			else
			{
				float min=0.f,max=1.f,step=0.1f;
				desc("min").tryNumber(min);
				desc("max").tryNumber(max);
				desc("step").tryNumber(step);
				scale=sfg::Scale::Create(min,max,step,orient);
				scale->SetValue(static_cast<float>(desc("value").asNumber()));
				if(desc("minorstep").tryNumber(min)&&desc("majorstep").tryNumber(max))
					scale->SetIncrements(min,max);
			}
			attachWithOpts(getLastTable(),scale,AttachmentOptions(desc));
			addCallbacks(scale,desc);
		}
		void InterfaceLoader::addScrollbar(const script::LuaTable& desc)
		{
			sfg::Scrollbar::Ptr scroll;
			sfg::Scrollbar::Orientation orient=sfg::Scrollbar::HORIZONTAL;
			if(desc("vertical").asBoolean()) orient=sfg::Scale::VERTICAL;
			if(desc("adjustment").getType()==script::LuaTable::Array)
			{
				scroll=sfg::Scrollbar::Create(getAdjusment(desc("adjustment")),orient);
			}
			else
			{
				scroll=sfg::Scrollbar::Create(orient);
				float major,minor;
				if(desc("minorstep").tryNumber(minor)&&desc("majorstep").tryNumber(major))
					scroll->SetIncrements(minor,major);
			}
			attachWithOpts(getLastTable(),scroll,AttachmentOptions(desc));
			addCallbacks(scroll,desc);
		}
		void InterfaceLoader::addButton(const script::LuaTable& desc)
		{

			sfg::Button::Ptr button=sfg::Button::Create(desc("text").asString());

			std::string str;
			if(desc("image").tryString(str))
			{
				button->SetImage(getImage(str));
			}
			attachWithOpts(getLastTable(),button,AttachmentOptions(desc));
			addCallbacks(button,desc);
		}
		void InterfaceLoader::addToggleButton(const script::LuaTable& desc)
		{
			sfg::ToggleButton::Ptr toggle=sfg::ToggleButton::Create(desc("text").asString());
			std::string str;
			if(desc("image").tryString(str))
			{
				toggle->SetImage(getImage(str));
			}
			toggle->SetActive(desc("down").asBoolean());

			attachWithOpts(getLastTable(),toggle,AttachmentOptions(desc));
			addCallbacks(toggle,desc);

			if(desc("OnToggle").tryString(str))
				toggle->GetSignal(sfg::ToggleButton::OnToggle).Connect(std::bind(&InterfaceLoader::call,this,toggle,str));
		}
		void InterfaceLoader::addCheckButton(const script::LuaTable& desc)
		{
			sfg::CheckButton::Ptr check=sfg::CheckButton::Create(desc("text").asString());

			check->SetActive(desc("down").asBoolean());

			attachWithOpts(getLastTable(),check,AttachmentOptions(desc));
			addCallbacks(check,desc);
			std::string str;
			if(desc("OnToggle").tryString(str))
				check->GetSignal(sfg::CheckButton::OnToggle).Connect(std::bind(&InterfaceLoader::call,this,check,str));
		}
		void InterfaceLoader::addRadioButton(const script::LuaTable& desc)
		{
			sfg::RadioButton::Ptr radio=sfg::RadioButton::Create(desc("text").asString());

			std::string str;
			if(desc("group").tryString(str)) radio->SetGroup(getGroup(str));

			radio->SetActive(desc("down").asBoolean());//must be after group so there is no multiple downs

			attachWithOpts(getLastTable(),radio,AttachmentOptions(desc));
			addCallbacks(radio,desc);
			
			if(desc("OnToggle").tryString(str))
				radio->GetSignal(sfg::RadioButton::OnToggle).Connect(std::bind(&InterfaceLoader::call,this,radio,str));
		}
		void InterfaceLoader::addComboBox(const script::LuaTable& desc)
		{
			sfg::ComboBox::Ptr combo=sfg::ComboBox::Create();

			const script::LuaTable& items=desc("items");
			for(int i=1;i<items.getSize();++i)
			{
				combo->AppendItem(items(i).asString());
			}
			sfg::ComboBox::IndexType selected;
			if(desc("selected").tryNumber(selected)) combo->SelectItem(selected);
			attachWithOpts(getLastTable(),combo,AttachmentOptions(desc));
			addCallbacks(combo,desc);
			std::string str;
			if(desc("OnOpen").tryString(str))
				combo->GetSignal(sfg::ComboBox::OnOpen).Connect(std::bind(&InterfaceLoader::call,this,combo,str));
			if(desc("OnSelect").tryString(str))
				combo->GetSignal(sfg::ComboBox::OnSelect).Connect(std::bind(&InterfaceLoader::call,this,combo,str));
		}
		sfg::Image::Ptr InterfaceLoader::getImage(const std::string& name)
		{
			auto find=m_Images.find(name);
			if(find==m_Images.end())
			{
				m_Images[name].loadFromStream(io::PhysInputStream(name.c_str()));
				find=m_Images.find(name);
			}
			return sfg::Image::Create(find->second);
		}
		sfg::Adjustment::Ptr InterfaceLoader::getAdjusment(const script::LuaTable& desc)
		{
			//fucking adjusments..
			sfg::Adjustment::Ptr adj=sfg::Adjustment::Create();
			float number;
			if(desc("lower").tryNumber(number)) adj->SetLower(number);
			if(desc("upper").tryNumber(number)) adj->SetUpper(number);
			if(desc("minorstep").tryNumber(number)) adj->SetMinorStep(number);
			if(desc("majorstep").tryNumber(number)) adj->SetMajorStep(number);
			if(desc("pagesize").tryNumber(number)) adj->SetPageSize(number);
			if(desc("value").tryNumber(number)) adj->SetValue(number);

			std::string str;
			if(desc("OnChange").tryString(str))
				adj->GetSignal(sfg::Adjustment::OnChange).Connect(std::bind(&InterfaceLoader::call,this,adj,str));
			return adj;
		}

		InterfaceLoader::GroupPtr InterfaceLoader::getGroup(const std::string& name)
		{
			auto find=m_RadioGroups.find(name);
			if(find==m_RadioGroups.end())
			{
				m_RadioGroups[name].reset(new sfg::RadioButton::RadioButtonGroup());
				find=m_RadioGroups.find(name);
			}
			return find->second;
		}
	}//gui
}//edy