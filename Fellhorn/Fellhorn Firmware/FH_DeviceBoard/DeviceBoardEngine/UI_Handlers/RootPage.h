/*
  ==============================================================================

    RootPage.h
    Created: 24/12/2025

    Abstract class used as root for all page objects

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "../UI_Thread.h"
#include "../../DeviceBoardEngine/UI_Definitions.h"

#include <Threads/TLV_DriverThread.h>

#include <Graphics/Geometry/Rectangle.h>
#include <Graphics/Geometry/Point.h>

namespace CasualNoises
{

class PageManager;
class SSD1309_Driver;
class TLV_Driver;
class Component;
class Graphics;

class RootPage
{
public:

	 RootPage() = delete;
	 RootPage(SSD1309_Driver* m_oledDriverPtr,
			  QueueHandle_t driverQueueHandle,
			  PageManager* pageManagerPtr);
	virtual ~RootPage() = default;

	void paintAll(Graphics& g);

	virtual void paint(Graphics& g) 	= 0;
	virtual void resized() 				= 0;

	virtual void loadContext() 			= 0;
	virtual void saveContext() 			= 0;

	bool handleUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g);

protected:

	SSD1309_Driver* m_oledDriverPtr			{ nullptr };
	QueueHandle_t 	mTLV_DriverQueueHandle	{ nullptr };
	PageManager* 	mPageManagerPtr			{ nullptr };

	void addChildComponent ( Component* child );
	void addAndMakeVisible ( Component* child );

	Rectangle<int> getBounds() const;

	virtual bool handleLocalUI_event ( sIncommingUI_Event* uiEvent, bool altState, Graphics& g ) { return false; };

	void dimSwitchLeds ();
	void setSwitchLed  ( eLED_BitNums led );

private:
	std::vector<Component*> 	mChildren;

	Rectangle< int > 			mLocalBounds;

};

} // namespace CasualNoises
