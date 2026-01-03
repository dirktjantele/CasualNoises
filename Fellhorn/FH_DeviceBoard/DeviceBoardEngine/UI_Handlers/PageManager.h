/*
  ==============================================================================

    PageManager.h
    Created: 22/12/2025

    Handles opening pages and moving around pages

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "RootPage.h"

#include "../UI_Thread.h"
#include "Graphics/Contexts/Graphics.h"

#include "main.h"

namespace CasualNoises
{

enum class ePageId
{
	empty			= 0x00000000,
	mainPage		= 0x6e69616d,			// 'main' main page
	setupPage		= 0x75707473,			// 'stpu' setup pag
	loadPage		= 0x64616f6c,			// 'load' load page
	calibrationPage	= 0x62696c63,			// 'clib' calibration page

};

class PageManager final
{
public:

	 PageManager() = default;
	 PageManager(SSD1309_Driver* oledDriverPt, TLV_Driver* TLV_DriverPtr);
	~PageManager();

	void handleUI_event(sIncommingUI_Event* uiEvent);

	void createNewPage (ePageId pageId);

protected:
	SSD1309_Driver* m_oledDriverPtr			{ nullptr };
	TLV_Driver* 	mTLV_DriverPtr			{ nullptr };

private:
	constexpr uint32_t static cPageIdStackSize 						= 24;
	ePageId					  mPageIdStack[cPageIdStackSize];					  // Contains the current page id stack (saved in flash)
	uint32_t				  mPageIdStackPtr 						{ 0 };		  // Points to next unused entry in the page stack (saved in flash)
	RootPage*				  mPageObjectStack[cPageIdStackSize];				  // Stack holding pointers to page object instances
	Graphics*				  mGraphics								{ nullptr };  // Graphics object used during painting of components

	void createPage (ePageId pageId, bool updateIdStack, uint32_t stackPtr);
	void setExitSwitchLedIntensity ();
	void handleExitSwitch (bool altState, bool doPaint = true);

};

} // namespace CasualNoises
