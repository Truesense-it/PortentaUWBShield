// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Truesense Srl

#ifndef UWBINBANDDATARX_HPP
#define UWBINBANDDATARX_HPP

// Global buffer that firmware writes to during ranging
uint8_t dataReceived[116];  // MAX_APP_DATA_SIZE from uwb_types.hpp


#include "UWB.hpp"
#include "UWBSession.hpp"
#include "UWBMacAddress.hpp"

/*
* This class will setup a ranging session with in-band data reception capabilities.
* When the data is received it will be signaled in the userCallback by receiving a
* notification of type DataReceived.
* The data will be in the global dataReceived[] buffer.
*/

class UWBInBandDataRx : public UWBSession {
public:
    UWBInBandDataRx(uint32_t session_ID, UWBMacAddress srcAddr,
                    UWBMacAddress dstAddr, uint32_t dataBlocks = 12)
        : destination(dstAddr)
    {
        sessionID(session_ID);
        sessionType(uwb::SessionType::DATA_TRANSFER);

        // Set ranging parameters using HAL types
        rangingParams.deviceRole(uwb::DeviceRole::RESPONDER);
        rangingParams.deviceType(uwb::DeviceType::CONTROLEE);
        rangingParams.multiNodeMode(uwb::MultiNodeMode::UNICAST);
        rangingParams.rangingRoundUsage(uwb::RangingMethod::DS_TWR);
        rangingParams.scheduledMode(uwb::ScheduledMode::TIME_SCHEDULED);
        rangingParams.deviceMacAddr(srcAddr);

        // Set app parameters
        appParams.noOfControlees(1);
        appParams.destinationMacAddr(dstAddr);
        applyDefaults();

        // Timing parameters for ranging session
        // appParams.rangingDuration(200);     // 200ms ranging duration
        // appParams.slotPerRR(10);            // 10 slots per ranging round
        // appParams.slotDuration(2400);       // 2400 RSTU = 2ms per slot

        // Frame config for SP1 (supports data transfer)
        appParams.frameConfig(1);  // SP1 configuration

        // Vendor specific data transfer parameters
        uwb::VendorAppConfig txBlocks;
        txBlocks.param_id = uwb::VendorAppConfigId::SESSION_INBAND_DATA_TX_BLOCKS;
        txBlocks.param_type = uwb::AppParamType::U32;
        txBlocks.param_value.vu32 = 0;
        vendorParams.addOrUpdateParam(txBlocks);

        uwb::VendorAppConfig rxBlocks;
        rxBlocks.param_id = uwb::VendorAppConfigId::SESSION_INBAND_DATA_RX_BLOCKS;
        rxBlocks.param_type = uwb::AppParamType::U32;
        rxBlocks.param_value.vu32 = dataBlocks;
        vendorParams.addOrUpdateParam(rxBlocks);

        // Clear the buffer
        memset(dataReceived, 0, sizeof(dataReceived));
    }

private:
    UWBMacAddress destination;
};

#endif /* UWBINBANDDATARX */