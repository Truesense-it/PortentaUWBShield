// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Truesense Srl

#ifndef UWBULTDOAANCHOR
#define UWBULTDOAANCHOR

#include "UWB.hpp"
#include "UWBSession.hpp"
#include "UWBMacAddress.hpp"
#include "hal/uwb_types.hpp"

class UWBUltdoaAnchor : public UWBSession {
public:     
    UWBUltdoaAnchor(uint32_t session_ID, UWBMacAddress srcAddr, uint32_t duration=1200) 
    {
        sessionID(session_ID);
        sessionType(uwb::SessionType::RANGING);
        rangingParams.deviceRole(uwb::DeviceRole::UT_ANCHOR);
        rangingParams.deviceType(uwb::DeviceType::CONTROLEE);
        rangingParams.multiNodeMode(uwb::MultiNodeMode::MULTICAST);
        rangingParams.rangingRoundUsage(uwb::RangingMethod::TDOA); 
        rangingParams.scheduledMode(uwb::ScheduledMode::TIME_SCHEDULED);
        rangingParams.deviceMacAddr(srcAddr);
        

        appParams.frameConfig(uwb::RfFrameConfig::Sfd_Sts);
        appParams.stsConfig(uwb::StsConfig::StaticSts);
        appParams.rangingDuration(duration);
        appParams.uplinkTdoaTimestamp(2); 
        appParams.addOrUpdateParam(buildScalar(uwb::AppConfigId::SessionInfoNtf, 1));
        appParams.sfdId(0);
        appParams.channel(9);
        appParams.preambleCodeIndex(10);
        appParams.macFcsType(0);
        appParams.noOfControlees(1);
        


        
    };
};

#endif /* UWBULTDOAANCHOR */
