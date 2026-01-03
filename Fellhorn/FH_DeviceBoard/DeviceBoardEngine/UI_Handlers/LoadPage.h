/*
  ==============================================================================

    LoadPage.h
    Created: 25/12/2025

    Handle load page UI interaction

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "RootPage.h"

#include "Drivers/OLED/SSD1309_Driver.h"
//#include "Components/StringSelectionComponent.h"

namespace CasualNoises
{
/*
typedef struct
{
	uint32_t	version;			// 'v0.1'
	uint32_t	foo;
} sMainPageState;
*/
class LoadPage final : public RootPage
{
public:

	 LoadPage() = default;
	 LoadPage(SSD1309_Driver* m_oledDriverPtr,
			  TLV_Driver* mTLV_DriverPtr,
			  PageManager* pageManagerPtr);
	~LoadPage() = default;

	void paint(Graphics& g) override;
	void resized() override;

	void loadContext() override;
	void saveContext() override;

//	bool handleUI_event(sIncommingUI_Event* uiEvent, bool altState) override;

private:
//	SSD1309_Driver* m_oledDriverPtr			{ nullptr };
//	TLV_Driver* 	mTLV_DriverPtr			{ nullptr };
/*
	const uint32_t	mVersion				{ 0x312e3076 };

	sMainPageState	mPageState;

	StringSelectionComponent*	mSelectionPtr;
*/
};

} // namespace CasualNoises
