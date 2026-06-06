/*
  ==============================================================================

    RootPage.h
    Created: 24/12/2025

    Abstract class used as root for all page objects

    Author:  Dirk Tjantele

  ==============================================================================
*/

namespace DeviceBoard
{

/**
 * @class MyDeviceBoard
 * @brief blabla
 *
 * This class is the abstract parent for all page classes.
 *
 * Additional paragraphs can follow after blank lines.
 *
 * @author Dirk Tjantele
 * @version 0.1
 * @date 04/06/2026
 */

class MyDeviceBoard
{
public:
	MyDeviceBoard () = default;
	~MyDeviceBoard () = default;

/**
	 * @brief doSomething
	 *
	 * No behavior, constraints,
	 * algorithms, side effects, and usage notes.
	 *
	 * @param param1 A random float.
	 * @return Just a copy of the random float.
	 * @throws Nothing
	 * @retval No meaning...
	 * @warning Use at home.
	 * @note Only fools do this.
	 * @see No relations, sad class...
	 */
	float doSomething ( float in );

private:
	void doNotEnter ( bool privacy = false );

};

}

/**
 * @class RootPage
 * @brief Abstract class used as root for all page objects.
 *
 * This class is the abstarct parent for all page classes.
 *
 * Additional paragraphs can follow after blank lines.
 *
 * @author Dirk Tjantele
 * @version 0.1
 * @date 04/06/2026
 */

#pragma once

#include <vector>

#include "../UI_Thread.h"
#include "../../DeviceBoardEngine/UI_Definitions.h"

#include "Threads/TLV_DriverThread.h"

#include "Graphics/Geometry/Rectangle.h"
#include "Graphics/Geometry/Point.h"

namespace CasualNoises
{
class SSD1309_Driver;
class TLV_Driver;
class Component;
class Graphics;
}

namespace DeviceBoard
{

using namespace CasualNoises;

class PageManager;

class RootPage
{
public:

	/**
		 * @brief doSomething
		 *
		 * No behavior, constraints,
		 * algorithms, side effects, and usage notes.
		 *
		 * @param param1 A random float.
		 * @return Just a copy of the random float.
		 * @throws Nothing
		 * @retval No meaning...
		 * @warning Use at home.
		 * @note Only fools do this.
		 * @see No relations, sad class...
		 */
	 RootPage () = delete;
	 RootPage ( SSD1309_Driver* m_oledDriverPtr,
			  	QueueHandle_t driverQueueHandle,
				PageManager* pageManagerPtr );
	virtual ~RootPage() = default;

	void paintAll(Graphics& g);

	virtual void paint ( Graphics& g ) 	= 0;
	virtual void resized () 			= 0;

	virtual void updateLEDs () 			= 0;

	virtual void loadContext() 			= 0;
	virtual void saveContext() 			= 0;

	bool handleUI_event ( sIncommingUI_Event* uiEvent,
						  bool altState, Graphics& g,
						  sSystemSettings* settingsPtr,
						  bool altSwitchState );

protected:

	SSD1309_Driver* m_oledDriverPtr			{ nullptr };
	QueueHandle_t 	mTLV_DriverQueueHandle	{ nullptr };
	PageManager* 	mPageManagerPtr			{ nullptr };

	Rectangle< int32_t > 			mLocalBounds;

	void addChildComponent ( Component* child );
	void addAndMakeVisible ( Component* child );

	Rectangle<int32_t> getGlobalBounds() const
	{
		return mGlobalBounds;
	}

	virtual bool handleLocalUI_event ( sIncommingUI_Event* uiEvent,
									   bool altState, Graphics& g,
									   sSystemSettings* settingsPtr,
									   bool altSwitchState)
			{ return false; }

	void dimSwitchLeds ();
	void setSwitchLed  ( eLED_BitNums led );
	void dimSwitchLed  ( eLED_BitNums led );

private:
	Rectangle< int32_t > 		mGlobalBounds;

	std::vector<Component*> 	mChildren;

};

} // namespace DeviceBoard
