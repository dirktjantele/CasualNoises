/*
  ==============================================================================

    SystemInfoPage.h
    Created: 17/03/2026

    Handle various system information display as an aid for firmware development

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "RootPage.h"

namespace CasualNoises
{

//==============================================================================
//          SystemInfoPage
//==============================================================================
class Box;
class Label;
class LabelGroup;
class ViewPort;

class SystemInfoPage : public RootPage
{
public:

	 SystemInfoPage() = delete;
	 SystemInfoPage( SSD1309_Driver* m_oledDriverPtr,
			 	 	 QueueHandle_t driverQueueHandle,
					 PageManager* pageManagerPtr);
	~SystemInfoPage() override;

	void paint(Graphics& g) override;
	void resized() override;

	void loadContext() override {};
	void saveContext() override {};

private:

	Box*			mOuterBoxPtr		{ nullptr };

	LabelGroup*		mLabelGroupPtr		{ nullptr };
	ViewPort*		mViewPortPtr		{ nullptr };

//	sMainPageState	mPreviousSavedContext;

};

} // namespace CasualNoises
