// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Truesense Srl

#ifndef UWBINBANDDATATX_HPP
#define UWBINBANDDATATX_HPP

// Global buffer that firmware reads during ranging
uint8_t dataToSend[116];  // MAX_APP_DATA_SIZE from uwb_types.hpp


#include "UWB.hpp"
#include "UWBSession.hpp"
#include "UWBMacAddress.hpp"

class UWBInBandDataTx : public UWBSession {
public:
    UWBInBandDataTx(uint32_t session_ID, UWBMacAddress srcAddr,
                    UWBMacAddress dstAddr, uint32_t dataBlocks = 12)
        : destination(dstAddr)
    {
        sessionID(session_ID);
        sessionType(uwb::SessionType::DATA_TRANSFER);

        // Set ranging parameters using HAL types
        rangingParams.deviceRole(uwb::DeviceRole::INITIATOR);
        rangingParams.deviceType(uwb::DeviceType::CONTROLLER);
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
        txBlocks.param_value.vu32 = dataBlocks;
        vendorParams.addOrUpdateParam(txBlocks);

        uwb::VendorAppConfig rxBlocks;
        rxBlocks.param_id = uwb::VendorAppConfigId::SESSION_INBAND_DATA_RX_BLOCKS;
        rxBlocks.param_type = uwb::AppParamType::U32;
        rxBlocks.param_value.vu32 = 0;
        vendorParams.addOrUpdateParam(rxBlocks);

        // Initialize buffer
        memset(dataToSend, 0, sizeof(dataToSend));
    }

    // Copy data to global buffer and Trigger Send
    uwb::Status sendData(uint8_t* data, uint16_t data_size) {
        if (data_size > sizeof(dataToSend)) {
            data_size = sizeof(dataToSend);
        }
        memcpy(dataToSend, data, data_size);

        uwb::DataPacket packet;
        packet.session_handle = sessionHdl;
        memcpy(packet.mac_address, destination.getData(), destination.getSize());
        packet.data = dataToSend; // Point to the global buffer
        packet.data_size = data_size;
        packet.sequence_number = 0; // Sequence is handled by HAL/FW usually

        uwb::Status status = UWBHAL.sendData(packet);
        if (status != uwb::Status::SUCCESS) {
            UWBHAL.Log_E("Failed to send data: %d", status);
        }
        return status;
    }

    // Helper to just set buffer without sending (Legacy support)
    void setDataToSend(const uint8_t* data, uint16_t size) {
        if (size > sizeof(dataToSend)) {
            size = sizeof(dataToSend);
        }
        memcpy(dataToSend, data, size);
    }

private:
    UWBMacAddress destination;
};

#endif /* UWBINBANDDATATX */