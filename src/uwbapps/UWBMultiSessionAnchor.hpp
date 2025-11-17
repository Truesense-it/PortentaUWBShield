// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Truesense Srl

#ifndef UWBMULTISESSIONANCHOR_HPP
#define UWBMULTISESSIONANCHOR_HPP

#include "UWB.hpp"
#include "UWBSession.hpp"

/**
 * @brief Multi-Session Anchor class for tracking multiple tags simultaneously
 * 
 * This class creates a UWB anchor that can manage multiple concurrent ranging
 * sessions with different tags. Each session uses unique parameters (session ID,
 * MAC addresses, preamble codes) to avoid interference.
 * 
 * Typical use case: A single anchor tracking multiple tags in asset tracking,
 * indoor positioning, or warehouse management systems.
 */
class UWBMultiSessionAnchor : public UWBSession {

public:     
    /**
     * @brief Construct a multi-session anchor for a specific session
     * 
     * @param session_ID Unique identifier for this session (e.g., 0x111111, 0x222222)
     * @param srcAddr MAC address of this anchor for this session
     * @param dstAddr MAC address of the target tag for this session
     * @param preambleCode Preamble code (must be unique per session to avoid interference)
     */
    UWBMultiSessionAnchor(uint32_t session_ID, UWBMacAddress srcAddr, 
                          UWBMacAddress dstAddr, uint8_t preambleCode = 10)
    {
        sessionID(session_ID);
        sessionType(uwb::SessionType::RANGING);
        
        rangingParams.deviceRole(uwb::DeviceRole::RESPONDER);
        rangingParams.deviceType(uwb::DeviceType::CONTROLEE);
        rangingParams.multiNodeMode(uwb::MultiNodeMode::UNICAST);
        rangingParams.rangingRoundUsage(uwb::RangingMethod::DS_TWR);
        rangingParams.scheduledMode(uwb::ScheduledMode::TIME_SCHEDULED);
        rangingParams.deviceMacAddr(srcAddr);
        
        appParams.noOfControlees(1);
        appParams.destinationMacAddr(dstAddr);
        appParams.frameConfig(uwb::RfFrameConfig::SP3);
        appParams.slotPerRR(25);
        appParams.rangingDuration(200);
        appParams.stsConfig(uwb::StsConfig::StaticSts);
        appParams.stsSegments(1);
        appParams.sfdId(2);
        appParams.preambleCodeIndex(preambleCode); //unique preamble per session
        appParams.channel(9);
    }
};

#endif /* UWBMULTISESSIONANCHOR_HPP */