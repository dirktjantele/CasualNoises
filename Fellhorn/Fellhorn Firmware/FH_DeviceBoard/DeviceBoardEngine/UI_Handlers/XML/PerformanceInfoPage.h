/*
  ==============================================================================

    PerformanceInfoPage.h
    Created: 07/06/2026

    Handles the load performance info page

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "../RootPage.h"

namespace CasualNoises
{
class Box;
class Label;
}

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          PerformanceInfoPage
//==============================================================================

class PerformanceInfoPage : public RootPage
{
public:

	 PerformanceInfoPage() = delete;
	 PerformanceInfoPage( SSD1309_Driver* m_oledDriverPtr,
			 	 	 	  QueueHandle_t driverQueueHandle,
						  PageManager* pageManagerPtr,
						  void* paramsPtr );
	~PerformanceInfoPage() override;

	virtual bool handleLocalUI_event ( sIncommingUI_Event* uiEvent,
									   bool altState, Graphics& g,
									   sSystemSettings* settingsPtr,
									   bool altSwitchState ) override;

	void paint(Graphics& g) override;
	void resized() override;

	void updateLEDs () override;

	void loadContext() override;
	void saveContext() override;

private:

	Box*			mOuterBoxPtr			{ nullptr };

	Label*			mNameLabelPtr			{ nullptr };
	Label*			mTypeLabelPtr			{ nullptr };
	Label*			mDescriptionLabel1_Ptr	{ nullptr };
	Label*			mDescriptionLabel2_Ptr	{ nullptr };

	uint32_t		mPerformanceId			{ 0 };
	String			mPerformanceName		{ "" };

	void updateLabelsFromXML ();

};

} // namespace DeviceBoard
