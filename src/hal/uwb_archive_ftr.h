// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Truesense Srl

// Feature switches that the precompiled UWB archive was built with and that change the layout of
// structs shared with it (uwb::DeviceInfo, uwb::DeviceCapabilities in uwb_types.hpp).
// uwb_types.hpp checks the resulting sizes with static_assert.

#ifndef UWB_ARCHIVE_FTR_H
#define UWB_ARCHIVE_FTR_H

#define UWBFTR_CCC 1                  // DeviceInfo carries the CCC version fields (100 bytes)
#define UWBIOT_UWBD_SR1XXT_SR2XXT 1   // DeviceCapabilities carries the SR1xx/SR2xx fields (68 bytes)

#endif // UWB_ARCHIVE_FTR_H
