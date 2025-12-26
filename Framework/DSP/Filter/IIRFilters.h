/*
  ==============================================================================

    IIRFilters.h
    Created: 28 jul 2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <vector>

#include "IIRFilterBase.h"
#include "GenericFilter.h"
#include "ButterworthFilter.h"
#include "LinkwitzRileyFilter.h"

namespace CasualNoises {
namespace Filter {

class IIRFilters final
{
public:
	 IIRFilters() = delete;
	 IIRFilters(int channelsNum);
	~IIRFilters() = default;

	ChannelFeed& getChannelFeed(int channelNum);
	void setFilterParameters(float cutoff, float q, IIRType filterType, IIRDesign filterDesign, float sampleRate);
	float applyFiltering(float xn, ChannelFeed& channel);

private:
	int channelsNum;
	std::vector<ChannelFeed*>	channelFeeds;

	// filters
	GenericFilter 				genericFilter;
	ButterworthFilter 			butterFilter;
	LinkwitzRileyFilter 		linkwitzRileyFilter;

	IIRFilterBase* 				activeFilter;

};

} // namespace Filter
} // namespace CasualNoises
