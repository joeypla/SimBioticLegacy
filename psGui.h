#ifndef PS_GUI_H
#define PS_GUI_H

#include "SRender.h"

#include <sstream>
namespace psGui{
	enum TextAlignment{
		SINGLE_TOP_LEFT,
		SINGLE_TOP_RIGHT,
		SINGLE_TOP_CENTER,
		SINGLE_CENTER_LEFT,
		SINGLE_CENTER_RIGHT,
		SINGLE_CENTER_CENTER,
		SINGLE_BOTTOM_LEFT,
		SINGLE_BOTTOM_RIGHT,
		SINGLE_BOTTOM_CENTER 
	};
	//GUI STUFF
	class Component{
	public:
		Component(void);
		~Component(void);
		virtual void setRelativePosition(float rx, float ry);
		float getRelativeX(void);
		float getRelativeY(void);

		virtual void setAbsolutePosition(float ax, float ay);
		virtual void update(void);
		virtual void sendSystemMessage(UINT message, UINT param);
		virtual void setVisible(bool visible);
		void setActive(bool active);
		bool getActive(void);
	protected:
		float rx;
		float ry;
		bool active;
	};
	class Frame{
	public:
		Frame(void);
		Frame(float x, float y);
		Frame(float x, float y, float width, float height);
		~Frame(void);

		

		void setPosition(float x, float y);
		void setSize(float x, float y);
		void setColor(DWORD color);
		void update(void);
		void sendSystemMessage(UINT message, UINT param);

		void addComponent(Component* component);

		void setVisible(bool visible);
		bool getVisible(void);

		bool getMouseWithinFrame(void);
	private:
		S2D* frame;
		std::vector <Component*> components;
	protected:
		float x;
		float y;

		float width;
		float height;

		bool visible;
	};
	
	class Checkbox : public Component{
	private:
		S2D* square;
		SText* check;
		bool state;

		bool visible;
		//we need this for the checkbox for some reason because the check just stays. Very annoying
	public: 
		Checkbox(void);
		~Checkbox(void);

		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);
		void setVisible(bool visible);

		float getRelativeX(void);
		float getRelativeY(void);

		void setSize(float size);
		void setCheckSize(int size);

		void setColor(DWORD color);
		void setCheckColor(DWORD color);

		bool getState(void);
		void setState(bool state);

		void update(void);
	};

	class Label : public Component{
	public:
		Label(void);
		Label(const wchar_t* text);
		~Label(void);

		void setText(const wchar_t* text);
		void setTextSize(int size);
		void setSize(float width, float height);
		void setTextColor(DWORD color);
		void setTextAlignment(DWORD flags);

		
		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);
		void setVisible(bool visible);
		void update(void);
	private:
		SText* label;

	};

	class Simple : public Component{
	public:
		Simple(void);
		
		~Simple(void);

		void setSize(float width, float height);
		void setColor(DWORD color);
		
		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);
		void setVisible(bool visible);
		void update(void);
		

		bool getMouseWithin(void);
		void x_SetTexture(LPDIRECT3DTEXTURE9 tex);
		void setTextureFromPath(std::wstring path);

		void setRotation(float rotation);
		void setImage(SImage* image);
	private:
		S2D* box;

	};

	class Slider : public Component{
	public:
		Slider(void);
		Slider(int min, int max);
		~Slider(void);

		void setMin(int min);
		void setMax(int max);
		void setRange(int min, int max);

		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);

		void setSize(float width, float height);

		void setValue(int value);
		int getValue(void);

		void setLineThickness(float thickness);
		void setSliderThickness(float thickness);
		void setSliderColor(DWORD color);
		void setLineColor(DWORD color);
		void setVisible(bool visible);
		void update(void);
		
	private:
		int min, max;
		int value;
		float lineThickness;
		float sliderThickness;
		S2D* line;
		S2D* slider;
	};

	class SimpleButton : public Component{
	public:
		SimpleButton(void);
		~SimpleButton(void);
		
		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);
		void setVisible(bool visible);

		void setCaption(wchar_t* caption);
		void setSize(float x, float y);
		void setColor(DWORD color);
		void setCaptionColor(DWORD color);
		void setCaptionSize(int size);
		void update(void);
		bool getMouseWithin(void);

		void setFunctionPointer(void (*pF)(void));
		void setInnerImage(SImage* image);
		void setInnerColor(DWORD color);
		void setOuterColor(DWORD color);
		void setTextHorizontalProportion(float proportion);
		void setTextAlignment(DWORD alignment);
	private:
		float textHorizontalProportion;
		S2D* inner;
		S2D* outer;
		SText* text;

		void (*pFunc)(void);
	};

	class TextField : public Component{
	public:
		TextField(void);
		~TextField(void);

		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);
		void update(void);
		void setVisible(bool visible);
		

		void setText(const wchar_t* text);
		void setSize(float x, float y);
		void setFieldColor(DWORD color);
		void setTextColor(DWORD color);
		void setTextSize(int size);
		void setPasswordProtected(bool pass);

		std::wstring getText(void);
		bool getMouseWithin(void);
	private:
		S2D* field;

		SText* text;
		std::wstring stream;
	};

	struct ListboxEntry
	{
		std::vector<std::wstring> values;
	};
	class Listbox : public Component{
	public:
		Listbox(int maxVisibleEntries, float x, float y, float width, float height, float scrollSize, int columns, float headerHeight);
		~Listbox(void);

		void setRelativePosition(float rx, float ry);
		void setAbsolutePosition(float ax, float ay);
		void sendSystemMessage(UINT message, UINT param);
		void update(void);
		void setVisible(bool visible);
		
		void setHeaderColor(DWORD color);
		void setScrollBackColor(DWORD color);
		void setScrollBarColor(DWORD color);
		void setSize(float x, float y);
		void setBackColor1(DWORD color);
		void setBackColor2(DWORD color);
		void setTextColor(DWORD color);
		void setTextSize(int size);
		void setScrollButtonSize(float size); 

		void setColumnTitle(int columnIndex, std::wstring title);
		void addRow_1col(std::wstring text1);
		void addRow_2col(std::wstring text1, std::wstring text2);
		void addRow_3col(std::wstring text1, std::wstring text2, std::wstring text3);
		
		void addRow(ListboxEntry* entry);
		std::string getStrAtIndexByCol(int column);
		void clear(void);

		std::wstring getText(void);
		bool getMouseWithin(void);
	private:
		int _maxVisibleEntries;
		DWORD backColor1;
		DWORD backColor2;
		DWORD backColorSelected;

		int numColumns;
		S2D* headerBar;

		//total width and height
		float width;
		float height;

		std::vector<float> columnWidthDistribution;
		void normalizeColumnWidths(void);


		std::vector<SText*> columnLabels;

		std::vector<ListboxEntry> rows;

		S2D* topScrollButton;
		S2D* bottomScrollButton;
		S2D* scrollBack;

		float scrollBarHeight;
		float scrollBarY;
		S2D* scrollBar;

		
		S2D** entryBacks;
		S2D* selectionBack;

		SText*** entryTexts;

		int _scrollIndex;
		int _selectedIndex;

		void updateEntryDisplay(void);
	};
}

#endif