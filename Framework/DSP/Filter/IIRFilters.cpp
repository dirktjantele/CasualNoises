/*
 * IIRFilters.cpp
 *
 *  Created on: Jul 28, 2025
 *      Author: dirktjantele
 */

#include "IIRFilters.h"

namespace CasualNoises {
namespace Filter {

IIRFilters::IIRFilters(int channels) :
	channelsNum(channels)
{
	for (auto i = 0; i < channelsNum; i++)
	{
		channelFeeds.push_back(new ChannelFeed);
	}
}

ChannelFeed & IIRFilters::getChannelFeed(int channelNum)
{
	return *channelFeeds[channelNum];
}

void IIRFilters::setFilterParameters(float cutoff, float q, IIRType filterType, IIRDesign filterDesign, float sampleRate)
{
	switch (filterDesign)
	{
	case IIRDesign::Generic:
		activeFilter = &genericFilter;
		break;
	case IIRDesign::Butterworth:
		activeFilter = &butterFilter;
		break;
	case IIRDesign::LinkwitzRiley:
		activeFilter = &linkwitzRileyFilter;
		break;
	default:
		activeFilter = &genericFilter;
	}

	activeFilter->updateFilterCoefficients(cutoff, q, sampleRate, filterType);
}

float IIRFilters::applyFiltering(float xn, ChannelFeed& c)
{
	if (activeFilter == nullptr)
		return xn;

	return activeFilter->applyFiltering(xn, c);
}
} // namespace Filter
} // namespace CasualNoises
