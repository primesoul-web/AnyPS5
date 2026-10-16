#include <cstdint>
#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <string>
#include "SceTypes.hpp"
#include "prx/libc/include/General.hpp"

namespace {

constexpr std::int32_t SCE_HMD2_ERROR_UNSUPPORTED_FEATURE = static_cast<std::int32_t>(0x81110016);
constexpr std::int32_t SCE_HMD2_ERROR_NOT_INITIALIZED = static_cast<std::int32_t>(0x8A720002);
constexpr std::int32_t SCE_HMD2_ERROR_PARAMETER_NULL = static_cast<std::int32_t>(0x8A720008);
constexpr std::int32_t SCE_HMD2_ERROR_REPROJECTION_NOT_INITIALIZED = static_cast<std::int32_t>(0x8A72000B);
constexpr std::int32_t SCE_HMD2_ERROR_REPROJECTION_ALREADY_INITIALIZED = static_cast<std::int32_t>(0x8A72000C);
constexpr std::int32_t SCE_VIDEO_OUT_ERROR_INVALID_VALUE = static_cast<std::int32_t>(0x80290001);
constexpr std::int32_t SCE_VIDEO_OUT_ERROR_NO_DEVICE = static_cast<std::int32_t>(0x80290018);
constexpr std::uint64_t HMD2_OUTPUT_MODE_89_91HZ = 0x2000C;
constexpr std::uint64_t HMD2_OUTPUT_MODE_119_88HZ = 0x2000F;
constexpr std::int32_t HMD2_REPROJECTION_TIMING_MIN = 2000;
constexpr std::int32_t HMD2_REPROJECTION_TIMING_MAX = 7000;
constexpr std::uint32_t HMD2_REPROJECTION_TIMING_DEFAULT = 3000;

std::mutex g_reprojection_mutex;
Hmd2ReprojectionState* g_reprojection_state = nullptr;

}

extern "C" {

int APS5_VABI sceHmd2ReprojectionQuerySeeThroughBufferSizeAlign() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

std::int32_t APS5_VABI sceHmd2Initialize(const void* param) {
    (void)param;
    return SCE_HMD2_ERROR_UNSUPPORTED_FEATURE;
}

int APS5_VABI sceHmd2GetFieldOfView() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2GetDeviceInformationByHandle() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2Terminate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2Close() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2GazeGetResult() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2GazeGetResultForFoveatedRendering() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

std::int32_t APS5_VABI sceHmd2GetDeviceInformation(Hmd2DeviceInformation* info) {
    (void)info;
    return SCE_HMD2_ERROR_NOT_INITIALIZED;
}

std::int32_t APS5_VABI sceHmd2GetFieldOfViewWithoutHandle(Hmd2FieldOfView* field_of_view) {
    (void)field_of_view;
    return SCE_HMD2_ERROR_NOT_INITIALIZED;
}

int APS5_VABI sceHmd2Open() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionBeginFrame() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionDisableMirroring() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionDisableVrMode() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionEnableMirroring() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

std::int32_t APS5_VABI sceHmd2ReprojectionEnableVrMode(std::uint64_t output_mode) {
    if (g_reprojection_state == nullptr) {
        return SCE_HMD2_ERROR_REPROJECTION_NOT_INITIALIZED;
    }
    if (output_mode != HMD2_OUTPUT_MODE_89_91HZ && output_mode != HMD2_OUTPUT_MODE_119_88HZ) {
        return SCE_VIDEO_OUT_ERROR_INVALID_VALUE;
    }
    return SCE_VIDEO_OUT_ERROR_NO_DEVICE;
}

int APS5_VABI sceHmd2ReprojectionGetMirroringDisplayBufferSizeAlign() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionGetMirroringWorkMemorySizeAlign() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionGetPredictedDisplayTime() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

std::int32_t APS5_VABI sceHmd2ReprojectionInitialize(const Hmd2ReprojectionInitializeParam* param) {
    std::lock_guard<std::mutex> lock(g_reprojection_mutex);
    if (g_reprojection_state != nullptr) {
        return SCE_HMD2_ERROR_REPROJECTION_ALREADY_INITIALIZED;
    }
    if (param == nullptr || param->reprojection_buffer == nullptr || param->display_buffer == nullptr) {
        return SCE_HMD2_ERROR_PARAMETER_NULL;
    }
    if (param->see_through_buffer != nullptr) {
        throw std::runtime_error(std::string(__func__) + ": see-through buffer not supported");
    }
    auto* state = static_cast<Hmd2ReprojectionState*>(param->reprojection_buffer);
    state->output_mode = 0;
    state->timing_us = param->reprojection_timing != 0 ? static_cast<std::uint32_t>(param->reprojection_timing) : HMD2_REPROJECTION_TIMING_DEFAULT;
    g_reprojection_state = state;
    return 0;
}

Hmd2SizeAlign APS5_VABI sceHmd2ReprojectionQueryBufferSizeAlign() {
    return Hmd2SizeAlign{sizeof(Hmd2ReprojectionState), alignof(Hmd2ReprojectionState)};
}

int APS5_VABI sceHmd2ReprojectionQueryDisplayBufferSizeAlign() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionSetAllowPositionalReprojection() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionSetMirroringOption() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionSetParam() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionSetParamWithBuffer() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionSetRenderConfig() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

std::int32_t APS5_VABI sceHmd2ReprojectionSetTiming(std::int32_t timing) {
    if (g_reprojection_state == nullptr) {
        return SCE_HMD2_ERROR_REPROJECTION_NOT_INITIALIZED;
    }
    if (timing < HMD2_REPROJECTION_TIMING_MIN || timing > HMD2_REPROJECTION_TIMING_MAX) {
        throw std::runtime_error(std::string(__func__) + ": timing out of range");
    }
    g_reprojection_state->timing_us = static_cast<std::uint32_t>((timing + 99) / 100 * 100);
    return 0;
}

int APS5_VABI sceHmd2ReprojectionTerminate() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

int APS5_VABI sceHmd2SetVibration() {
    NotImplemented_nid_no_patch(__func__);
    return 0;
}

}
