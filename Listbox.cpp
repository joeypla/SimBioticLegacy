#include "psGui.h"

#include "SInput.h"
psGui::Listbox::Listbox(int maxVisibleEntries, float x, float y, float width, float height, float scrollSize, int columns, float headerHeight)
{
	rx = x;
	ry = y;
	this->width = width;
	this->height = height;
	_scrollIndex = 0;
	float aspectRatio = (float)SRender::getSingleton()->getWidth() / (float)SRender::getSingleton()->getHeight();
	float scrollButtonHeight = scrollSize;
	float scrollButtonWidth = scrollSize / aspectRatio;

	_maxVisibleEntries = maxVisibleEntries;
	numColumns = columns;

	for (int i = 0; i < numColumns; i++) columnWidthDistribution.push_back(1.0f / (float)numColumns);

	float startPositionX = x - width / 2.0f;
	
	for (int i = 0; i < numColumns; i++)
	{
		float columnWidth = columnWidthDistribution[i]*(width - scrollButtonWidth);

		SText* sText = SRender::getSingleton()->createText();
		sText->setAlignment(DT_VCENTER | DT_LEFT);
		sText->setText(L"DEFAULT");
		sText->setPosition(startPositionX + 0.5f*columnWidth, y + height / 2.0f - headerHeight / 2.0f);
		sText->setDimensions(columnWidth, headerHeight);

		columnLabels.push_back(sText);

		if ( i < numColumns - 1)
			startPositionX += 0.5f*columnWidthDistribution[i] + 0.5f*columnWidthDistribution[i+1];
	}
	

	backColor1 = 0xff00ffff;
	backColor2 = 0xffff0000;
	backColorSelected = 0xffffffff;

	entryBacks = 0;
	if (_maxVisibleEntries > 0)
		entryBacks = new S2D*[maxVisibleEntries];


	

	float entryBackHeight = (height - headerHeight) / (float)_maxVisibleEntries;
	headerBar = SRender::getSingleton()->createS2D();
	headerBar->setColor(0xffffff00);
	headerBar->setDimensions(width - scrollButtonWidth, headerHeight);
	headerBar->setPosition(x - scrollButtonWidth / 2.0f, y + height/2.0f - headerHeight /2.0f);

	float backStartPosition = y + height /2.0f - headerHeight - entryBackHeight/2.0f;
	for (int i = 0; i < _maxVisibleEntries; i++)
	{
		entryBacks[i] = SRender::getSingleton()->createS2D();
		entryBacks[i]->setDimensions(width - scrollButtonWidth, entryBackHeight);
		entryBacks[i]->setColor((i % 2 == 0) ? backColor1 : backColor2);
		entryBacks[i]->setPosition(x - scrollButtonWidth / 2.0f, backStartPosition);
		backStartPosition -= entryBackHeight;
	}

	selectionBack = SRender::getSingleton()->createS2D();
	selectionBack->setDimensions(width - scrollButtonWidth, entryBackHeight);
	selectionBack->setColor(backColorSelected);
	selectionBack->setPosition(x, y);//position doesn't matter too much right now



	topScrollButton = SRender::getSingleton()->createS2D();
	topScrollButton->setDimensions(scrollButtonWidth, scrollButtonHeight);
	topScrollButton->setPosition(x + width/2.0f - scrollButtonWidth / 2.0f, y + height/2.0f - scrollButtonHeight / 2.0f);
	topScrollButton->setColor(0xffffffff);

	bottomScrollButton = SRender::getSingleton()->createS2D();
	bottomScrollButton->setDimensions(scrollButtonWidth, scrollButtonHeight);
	bottomScrollButton->setPosition(x + width/2.0f - scrollButtonWidth / 2.0f, y - height/2.0f + scrollButtonHeight / 2.0f);
	bottomScrollButton->setColor(0xffffffff); 

	scrollBack = SRender::getSingleton()->createS2D();
	scrollBack->setDimensions(scrollButtonWidth, height - 2*scrollButtonHeight);
	scrollBack->setPosition(x + width*0.5f - scrollButtonWidth*0.5f, y);
	scrollBack->setColor(0xff00ff00); 

	

	scrollBar = SRender::getSingleton()->createS2D();
	scrollBar->setDimensions(scrollButtonWidth, 0.2f);
	scrollBar->setPosition(x + width*0.5f - scrollButtonWidth*0.5f, y);
	scrollBar->setColor(0xff0000ff); 


	entryTexts = new SText**[_maxVisibleEntries];
	float startPositionY = y + height / 2.0f - headerHeight - entryBackHeight/2.0f;
	for (int i = 0; i < _maxVisibleEntries; i++)
	{
		startPositionX = x - width / 2.0f;
		entryTexts[i] = new SText*[numColumns];

		for (int j = 0; j < numColumns; j++)
		{
			float columnWidth = columnWidthDistribution[j]*(width - scrollButtonWidth);
			entryTexts[i][j] = SRender::getSingleton()->createText();
			entryTexts[i][j]->setText(L"THIS IS TEXT");
			entryTexts[i][j]->setPosition(startPositionX + 0.5f*columnWidth, startPositionY);
			entryTexts[i][j]->setDimensions(columnWidth, entryBackHeight);
			entryTexts[i][j]->setColor(0xff000000);
			entryTexts[i][j]->setAlignment(DT_VCENTER | DT_LEFT);
			if (j < numColumns - 1)
				startPositionX += 0.5f*columnWidthDistribution[j] + 0.5f*columnWidthDistribution[j+1];
		}

		startPositionY -= entryBackHeight;
	}

	updateEntryDisplay();
}

psGui::Listbox::~Listbox(void)
{
	SRender::getSingleton()->releaseS2D(headerBar);
	for (int i = 0; i < columnLabels.size(); i++) SRender::getSingleton()->releaseText(columnLabels.at(i));

	SRender::getSingleton()->releaseS2D(topScrollButton);
	SRender::getSingleton()->releaseS2D(bottomScrollButton);
	SRender::getSingleton()->releaseS2D(scrollBar);
	SRender::getSingleton()->releaseS2D(scrollBack);
	SRender::getSingleton()->releaseS2D(selectionBack);
	for (int i = 0; i < _maxVisibleEntries; i++)
	{
		SRender::getSingleton()->releaseS2D(entryBacks[i]);
		for (int j = 0; j < numColumns; j++)
			SRender::getSingleton()->releaseText(entryTexts[i][j]);
	}

}

void psGui::Listbox::setRelativePosition(float newx, float newy)
{
	
}

void psGui::Listbox::setAbsolutePosition(float newx, float newy)
{
}

void psGui::Listbox::sendSystemMessage(UINT message, UINT param)
{
}

void psGui::Listbox::update(void)
{

	if (S_INPUT::mousePressed(0))
	{
		//if its in the top button, we advance the index --
		//if its in the bottom button, we advance the index++
		int mouseX = S_INPUT::getMouseX();
		int mouseY = S_INPUT::getMouseY();
		//normalise mouse coords
		SRender* sr = SRender::getSingleton();
		float screenWidth = sr->getWidth();
		float screenHeight = sr->getHeight();
		float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
		float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);


		if (x < topScrollButton->getPositionX() + topScrollButton->getDimensionsX()*0.5f &&
		x > topScrollButton->getPositionX() - topScrollButton->getDimensionsX()*0.5f &&
		y < topScrollButton->getPositionY() + topScrollButton->getDimensionsY()*0.5f &&
		y > topScrollButton->getPositionY() - topScrollButton->getDimensionsY()*0.5f)
		{
			_scrollIndex--;
			if (_scrollIndex < 0) _scrollIndex = 0;
		}

		if (x < bottomScrollButton->getPositionX() + bottomScrollButton->getDimensionsX()*0.5f &&
		x > bottomScrollButton->getPositionX() - bottomScrollButton->getDimensionsX()*0.5f &&
		y < bottomScrollButton->getPositionY() + bottomScrollButton->getDimensionsY()*0.5f &&
		y > bottomScrollButton->getPositionY() - bottomScrollButton->getDimensionsY()*0.5f)
		{
			_scrollIndex++;
			//if (_scrollIndex > 0) _scrollIndex = 0;
		}


		//now check to see if the mouse is clicked and the user is selecting a row
		if (x > rx - width / 2.0f && x < rx + (width / 2.0f) - bottomScrollButton->getDimensionsX() &&
			y > ry - height / 2.0f && y < ry + (height / 2.0f) - headerBar->getDimensionsY())

		{
			
			//find how much backs we have to check
			int lastEntryIndex = _scrollIndex + _maxVisibleEntries - 1;
			int entryBackIndex = 0;
			for (int i = _scrollIndex; i <= lastEntryIndex; i++)
			{
				if (x > entryBacks[entryBackIndex]->getPositionX() - entryBacks[entryBackIndex]->getDimensionsX()*0.5f &&
					x < entryBacks[entryBackIndex]->getPositionX() + entryBacks[entryBackIndex]->getDimensionsX()*0.5f &&
					y > entryBacks[entryBackIndex]->getPositionY() - entryBacks[entryBackIndex]->getDimensionsY()*0.5f &&
					y < entryBacks[entryBackIndex]->getPositionY() + entryBacks[entryBackIndex]->getDimensionsY()*0.5f)
				{
					_selectedIndex = i;
					break;
				}
				entryBackIndex++;
			}
		}

		updateEntryDisplay();
	}
}

void psGui::Listbox::setVisible(bool visible)
{
	headerBar->setVisible(visible);
	bottomScrollButton->setVisible(visible);
	topScrollButton->setVisible(visible);
	scrollBack->setVisible(visible);
	scrollBar->setVisible(visible);
	selectionBack->setVisible(visible);
	for (int i = 0; i < numColumns; i++)
	{
		columnLabels[i]->setVisible(visible);
	}
	for (int i = 0; i < _maxVisibleEntries; i++)
	{
		entryBacks[i]->setVisible(visible);
		for (int j = 0; j < numColumns; j++)
		{
			entryTexts[i][j]->setVisible(visible);
		}
	}

	if (visible) updateEntryDisplay();
}

void psGui::Listbox::setBackColor1(DWORD color)
{
	backColor1 = color;
	for (int i = 0; i < _maxVisibleEntries; i++)
		entryBacks[i]->setColor((i%2) ? backColor1 : backColor2);
}

void psGui::Listbox::setBackColor2(DWORD color)
{
	backColor2 = color;
	for (int i = 0; i < _maxVisibleEntries; i++)
		entryBacks[i]->setColor((i%2) ? backColor1 : backColor2);
}

void psGui::Listbox::setScrollBackColor(DWORD color)
{
	scrollBack->setColor(color);
}

void psGui::Listbox::setScrollBarColor(DWORD color)
{
	scrollBar->setColor(color);
}
void psGui::Listbox::setHeaderColor(DWORD color)
{
	headerBar->setColor(color);
}

void psGui::Listbox::setSize(float x, float y)
{
}

void psGui::Listbox::setTextColor(DWORD color)
{
}

void psGui::Listbox::setTextSize(int size)
{
}

void psGui::Listbox::setScrollButtonSize(float size)
{
}

void psGui::Listbox::setColumnTitle(int columnIndex, std::wstring title)
{
	if (columnIndex < 0 || columnIndex >= numColumns) return;

	columnLabels.at(columnIndex)->setText(title.c_str());


}

void psGui::Listbox::normalizeColumnWidths(void)
{
	float sum = 0.0f;

	for (int i = 0; i < numColumns; i++)
		sum+=columnWidthDistribution.at(i)*columnWidthDistribution.at(i);

	float magnitude = sqrt(sum);
	
	for (int i = 0; i < numColumns; i++)
		columnWidthDistribution.at(i) = columnWidthDistribution.at(i) / magnitude;

}
 
void psGui::Listbox::addRow(ListboxEntry* entry)
{

	rows.push_back(*entry);

	updateEntryDisplay();
}

void psGui::Listbox::updateEntryDisplay(void)
{
	int totalNumberOfEntries = rows.size();
	int lastEntryIndex = _scrollIndex + _maxVisibleEntries - 1;

	for (int i = 0; i < _maxVisibleEntries; i++)
		for (int j = 0; j < numColumns; j++)
			entryTexts[i][j]->setVisible(false);

	int entryTextIndex = 0;
	selectionBack->setVisible(false);
	for (int i = _scrollIndex; i <= lastEntryIndex; i++)
	{
		if (i == _selectedIndex)
		{
			selectionBack->setVisible(true);
			selectionBack->setPosition(entryBacks[entryTextIndex]->getPositionX(), entryBacks[entryTextIndex]->getPositionY());
		}
		if (i < totalNumberOfEntries)
		{
			for (int j = 0; j < numColumns; j++)
			{
				entryTexts[entryTextIndex][j]->setVisible(true);
				entryTexts[entryTextIndex][j]->setText(rows.at(i).values.at(j).c_str());
				
			}

			entryTextIndex++;
		} else break;
	}
}

void psGui::Listbox::clear(void)
{
	rows.clear();
}

std::string psGui::Listbox::getStrAtIndexByCol(int col)
{
	if (col >= numColumns) return std::string("<ERROR>");
	if (_selectedIndex >= rows.size()) return std::string("<ERROR>");
	std::wstring wStr = rows.at(_selectedIndex).values.at(col);
	std::string str(wStr.begin(), wStr.end());
	return str;
}