/*
  ==============================================================================

    XML_FactoryDefaults.h
    Created: 05/06/2026

    Reads factory defaults from mcu flash into the local external flash

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "queue.h"

#include "XML/TinyXML/tinyxml2.h"

#include "XML_FactoryPresets.h"

using namespace tinyxml2;

namespace DeviceBoard
{

//==============================================================================
//          inStallXML_FactoryDefaults ()
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
bool inStallXML_FactoryDefaults ( QueueHandle_t TLV_DriverQueue )
{

	// Save new XML only when not found on flash or size is different
	uint32_t index = findNextTLV ( gYellowPages.gTLV_DriverThreadQueueHandle, (uint32_t)eTLV_Tag::FactoryPresets, 0 );
	uint32_t oldSize = 0;
	if ( index != 0 )
		oldSize = getTLV_LengthBytes ( gYellowPages.gTLV_DriverThreadQueueHandle, index );
	uint32_t newSize = strlen ( xmlData );
	bool doUpdate = oldSize != newSize;

	// Update XML data in flash when required
	if ( doUpdate )
	{
		updateTLV_Bytes ( TLV_DriverQueue,
						  ( uint32_t ) eTLV_Tag::FactoryPresets,
						  newSize,
						  ( uint32_t* ) xmlData );
	}
/*
	// ToDo remove test code
	XMLDocument doc;
	doc.Parse ( xmlData );

    XMLElement* root = doc.FirstChildElement("PERFORMANCES");
    if (!root) {
        return false;
    }

    for (XMLElement* perf = root->FirstChildElement("PERFORMANCE");
         perf != nullptr;
         perf = perf->NextSiblingElement("PERFORMANCE"))
    {
        // --- INFO ---
        XMLElement* info = perf->FirstChildElement("INFO");
        if (info) {
            const char* name = info->FirstChildElement("NAME")->GetText();
            const char* type = info->FirstChildElement("TYPE")->GetText();
            const char* desc = info->FirstChildElement("DESCRIPTION")->GetText();

            uint32_t size = strlen ( desc );
        }

        // --- POT CONFIGURATION ---
        XMLElement* potConfig = perf->FirstChildElement("POT_CONFIGURATION");
        if (potConfig) {
            for (XMLElement* pot = potConfig->FirstChildElement("POT");
                 pot != nullptr;
                 pot = pot->NextSiblingElement("POT"))
            {
                int page = pot->IntAttribute("page");
                int id   = pot->IntAttribute("id");
                int value = pot->IntAttribute("value");
                const char* potName = pot->Attribute("name");

                uint32_t size = strlen ( potName );
            }
        }

    }
*/
	return true;
}

} // namespace DeviceBoard
