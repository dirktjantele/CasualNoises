/*
  ==============================================================================

    SetupPage.h
    Created: 25/12/2025

    Handle setup page UI interaction

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
class SetupPage final : public RootPage
{
public:

	 SetupPage() = default;
	 SetupPage(SSD1309_Driver* m_oledDriverPtr,
			   QueueHandle_t driverQueueHandle,
			   PageManager* pageManagerPtr);
	~SetupPage() = default;

	void paint(Graphics& g) override;
	void resized() override;

	void loadContext() override;
	void saveContext() override;

private:

};

} // namespace CasualNoises
