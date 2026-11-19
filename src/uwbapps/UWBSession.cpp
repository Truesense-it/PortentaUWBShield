// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Truesense Srl

#include "UWBSession.hpp"


UWBSession::UWBSession()
{
    sessID = 0;
    sessionHdl = 0;
    type = uwb::SessionType::RANGING;
    isActive = false;

    // Initialize the ranging parameters with default antenna config
    static uint8_t antennaeConfigurationRx[] = { 1, 0x01, (1)};
    static const uint8_t antennaeConfigurationRx_size = 3;
    uwb::VendorAppConfig antennaParam;
    antennaParam.param_id = uwb::VendorAppConfigId::ANTENNAE_CONFIGURATION_RX;
    antennaParam.param_type = uwb::AppParamType::ARRAY_U8;
    antennaParam.param_value.au8.param_value = antennaeConfigurationRx;//&antennaeConfigurationRx[0];
    antennaParam.param_value.au8.param_len = antennaeConfigurationRx_size;
    vendorParams.addOrUpdateParam(antennaParam);
    
}

UWBSession::UWBSession(const UWBSession &other)
    : rangingParams(other.rangingParams),
      appParams(other.appParams),
      vendorParams(other.vendorParams),
      sessID(other.sessID),
      sessionHdl(other.sessionHdl),
      type(other.type),
      isActive(other.isActive)
{
    appParams.rebindOwnedArrays(other.appParams);
}


void UWBSession::sessionID(uint32_t id)
{
    sessID = id;
}

void UWBSession::sessionHandle(uint32_t handle)
{
    sessionHdl = handle;
}

void UWBSession::sessionType(uwb::SessionType sessionType)
{
    type = sessionType;
}

uint32_t UWBSession::sessionID()
{
    return sessID;
}

uint32_t UWBSession::sessionHandle()
{
    return sessionHdl;
}

uwb::SessionType UWBSession::sessionType()
{
    return type;
}

uwb::Status UWBSession::init()
{
    uwb::Status res = uwb::Status::SUCCESS;
    res= UWBHAL.sessionInit(sessID, type, sessionHdl);
    if (res != uwb::Status::SUCCESS)
    {
        UWBHAL.Log_E("could not init session");
        return res;
    }
    // Set ranging (core) params first to ensure device mode/addressing is configured
    res=UWBHAL.setRangingParams(sessionHdl, rangingParams);
    if (res != uwb::Status::SUCCESS)
    {
        UWBHAL.Log_E("could not set ranging params");
        return res;
    }

    // Then set application parameters
    if (appParams.getSize())
    {
        res=UWBHAL.setAppConfigMultiple(sessionHdl, appParams);
        if (res != uwb::Status::SUCCESS)
        {
            UWBHAL.Log_E("could not set app params: %d", res);
            return res;
        }
    }
    else
        UWBHAL.Log_E("no app params");

    if(vendorParams.getSize())
    {
        res=UWBHAL.setVendorAppConfig(sessionHdl, vendorParams);
        if(res != uwb::Status::SUCCESS)
        {
            UWBHAL.Log_E("could not set vendor params - %d", res);
            return res;
        }
    }
    else
        UWBHAL.Log_E("no vendor params");

    return res;
}

uwb::Status UWBSession::deInit()
{
    return UWBHAL.sessionDeinit(sessionHdl);
}

uwb::Status UWBSession::sendData(uwb::DataPacket& pSendData)
{
    return UWBHAL.sendData(pSendData);
}

uwb::Status UWBSession::appConfig(uwb::AppConfigId param_id, uint32_t param_value)
{
    return UWBHAL.setAppConfig(sessionHdl, param_id, param_value);
}

uwb::Status UWBSession::enableRangingDataNtf(uint8_t enableRangingDataNtf, uint16_t proximityNear, uint16_t proximityFar)
{
    return UWBHAL.enableRangingNotifications(sessionHdl, enableRangingDataNtf, proximityNear, proximityFar);
}

uwb::Status UWBSession::staticSts(uint16_t vendorId, uint8_t *staticStsIv)
{
    // Convert the 6-byte array to vector
    std::vector<uint8_t> stsIvVector(staticStsIv, staticStsIv + 6);
    
    return UWBHAL.setStaticSts(sessionHdl, vendorId, stsIvVector);
}

uwb::Status UWBSession::state(uint8_t& state)
{
    return UWBHAL.getSessionState(sessionHdl, state);
}

uwb::Status UWBSession::sendData(uint8_t *sendData, uint8_t len, uint8_t seq_num, uint8_t *dst_mac)
{
    uwb::DataPacket data;
    uint8_t offset = 0;
    
    data.session_handle = this->sessionHdl;
    data.data_size = len;
    data.data = sendData;
    data.sequence_number = seq_num;
    for (offset = 0; offset < 8; offset++)
        data.mac_address[offset] = dst_mac[offset];
    return UWBHAL.sendData(data);
}

uwb::Status UWBSession::start()
{
    return UWBHAL.startRanging(sessionHdl);
}
uwb::Status UWBSession::stop()
{
    return UWBHAL.stopRanging(sessionHdl);
}

void UWBSession::applyDefaults()
{
    appParams.sfdId(2);
    appParams.slotPerRR(25);
    appParams.rangingDuration(25*8);
    appParams.stsConfig(uwb::StsConfig::StaticSts);
    appParams.stsSegments(1);
    appParams.frameConfig(3);    
}
