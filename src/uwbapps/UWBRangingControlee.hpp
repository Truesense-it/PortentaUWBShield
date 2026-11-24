// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Truesense Srl

#ifndef UWBRANGINGCONTROLEE_HPP
#define UWBRANGINGCONTROLEE_HPP

#include "UWB.hpp"
#include "UWBSession.hpp"

class  UWBRangingControlee:public UWBSession {

public:

	UWBRangingControlee(uint32_t session_ID, UWBMacAddress srcAddr, UWBMacAddress dstAddr)
	{
		sessionID(session_ID);
		sessionType(uwb::SessionType::RANGING);

		// Ranging parameters
		rangingParams.deviceRole(uwb::DeviceRole::RESPONDER);
		rangingParams.deviceType(uwb::DeviceType::CONTROLEE);
		rangingParams.multiNodeMode(uwb::MultiNodeMode::UNICAST);
		rangingParams.rangingRoundUsage(uwb::RangingMethod::DS_TWR);
		rangingParams.scheduledMode(uwb::ScheduledMode::TIME_SCHEDULED);
		rangingParams.macAddrMode((uint8_t)uwb::MacAddressMode::SHORT);
		rangingParams.deviceMacAddr(srcAddr);

		// Application parameters - must match controller exactly
		appParams.noOfControlees(1);              // Explicit: 1 controlee
		appParams.destinationMacAddr(dstAddr);
		appParams.frameConfig(uwb::RfFrameConfig::SP3);
		appParams.stsConfig(uwb::StsConfig::StaticSts);
		appParams.stsSegments(1);
		appParams.sfdId(2);                       // 64-bit SFD for better accuracy
		appParams.preambleCodeIndex(10);
		appParams.slotDuration(2400);             // 2400 RSTU = 2ms per slot
		appParams.slotPerRR(25);                  // 25 slots per ranging round
		appParams.rangingDuration(200);           // 200ms ranging interval

}

};

#endif/* UWBRANGINGCONTROLEE */
