#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

extern "C" {
std::int32_t APS5_VABI sceHmd2Initialize(const void* param);
std::int32_t APS5_VABI sceHmd2GetDeviceInformation(Hmd2DeviceInformation* info);
std::int32_t APS5_VABI sceHmd2GetFieldOfViewWithoutHandle(Hmd2FieldOfView* field_of_view);
std::int32_t APS5_VABI sceHmd2ReprojectionEnableVrMode(std::uint64_t output_mode);
std::int32_t APS5_VABI sceHmd2ReprojectionSetTiming(std::int32_t timing);
Hmd2SizeAlign APS5_VABI sceHmd2ReprojectionQueryBufferSizeAlign();
std::int32_t APS5_VABI sceHmd2ReprojectionInitialize(const Hmd2ReprojectionInitializeParam* param);
int APS5_VABI sceHmd2Open();
}

namespace {

void Require(bool condition, const char* message) {
    if (!condition) {
        std::fprintf(stderr, "HMD2: %s\n", message);
        std::abort();
    }
}

}

int main() {
    static_assert(sizeof(Hmd2DeviceInformation) == 28);
    static_assert(sizeof(Hmd2FieldOfView) == 16);
    static_assert(sizeof(Hmd2ReprojectionState) == 16);
    static_assert(alignof(Hmd2ReprojectionState) == 8);
    static_assert(sizeof(Hmd2ReprojectionInitializeParam) == 72);
    static_assert(offsetof(Hmd2ReprojectionInitializeParam, reprojection_timing) == 48);
    const std::uint8_t param[16]{};
    Require(sceHmd2Initialize(param) == static_cast<std::int32_t>(0x81110016), "initialization did not report the unsupported feature");
    Require(sceHmd2Initialize(nullptr) == static_cast<std::int32_t>(0x81110016), "initialization without a param did not report the unsupported feature");
    Hmd2DeviceInformation info{};
    Require(sceHmd2GetDeviceInformation(&info) == static_cast<std::int32_t>(0x8A720002), "device information did not report not initialized");
    Require(sceHmd2GetDeviceInformation(nullptr) == static_cast<std::int32_t>(0x8A720002), "device information without an output did not report not initialized");
    Require(info.status == 0 && info.hmu_mount == 0, "device information was filled without a runtime");
    Hmd2FieldOfView fieldOfView{};
    Require(sceHmd2GetFieldOfViewWithoutHandle(&fieldOfView) == static_cast<std::int32_t>(0x8A720002), "the field of view did not report not initialized");
    Require(sceHmd2GetFieldOfViewWithoutHandle(nullptr) == static_cast<std::int32_t>(0x8A720002), "the field of view without an output did not report not initialized");
    Require(sceHmd2ReprojectionEnableVrMode(0x2000C) == static_cast<std::int32_t>(0x8A72000B), "enabling VR mode did not report reprojection not initialized");
    Require(sceHmd2ReprojectionSetTiming(3000) == static_cast<std::int32_t>(0x8A72000B), "setting the timing did not report reprojection not initialized");
    const Hmd2SizeAlign sizeAlign = sceHmd2ReprojectionQueryBufferSizeAlign();
    Require(sizeAlign.size == 16 && sizeAlign.align == 8, "the reprojection work size did not follow the state layout");
    alignas(8) std::uint8_t work[16]{};
    std::uint8_t display[64]{};
    std::uint8_t seeThrough[64]{};
    Hmd2ReprojectionInitializeParam initParam{};
    Require(sceHmd2ReprojectionInitialize(nullptr) == static_cast<std::int32_t>(0x8A720008), "initialization without a param did not report the null parameter");
    Require(sceHmd2ReprojectionInitialize(&initParam) == static_cast<std::int32_t>(0x8A720008), "initialization without buffers did not report the null parameter");
    initParam.reprojection_buffer = work;
    Require(sceHmd2ReprojectionInitialize(&initParam) == static_cast<std::int32_t>(0x8A720008), "initialization without a display buffer did not report the null parameter");
    initParam.display_buffer = display;
    initParam.see_through_buffer = seeThrough;
    bool threw = false;
    try {
        sceHmd2ReprojectionInitialize(&initParam);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    Require(threw, "initialization with a see-through buffer did not throw");
    initParam.see_through_buffer = nullptr;
    initParam.reprojection_timing = 0;
    Require(sceHmd2ReprojectionInitialize(&initParam) == 0, "initialization did not succeed");
    const auto* state = reinterpret_cast<const Hmd2ReprojectionState*>(work);
    Require(state->output_mode == 0 && state->timing_us == 3000, "the initial state did not carry the default timing");
    Require(sceHmd2ReprojectionInitialize(&initParam) == static_cast<std::int32_t>(0x8A72000C), "a second initialization did not report reprojection already initialized");
    Require(sceHmd2ReprojectionSetTiming(2500) == 0, "setting the timing did not succeed");
    Require(state->timing_us == 2500, "the timing was not recorded in the work buffer");
    threw = false;
    try {
        sceHmd2ReprojectionSetTiming(1999);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    Require(threw, "a timing below the range did not throw");
    threw = false;
    try {
        sceHmd2ReprojectionSetTiming(7001);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    Require(threw, "a timing above the range did not throw");
    Require(sceHmd2ReprojectionEnableVrMode(0x2000C) == static_cast<std::int32_t>(0x80290018), "enabling VR mode did not report no device");
    Require(sceHmd2ReprojectionEnableVrMode(0x2000F) == static_cast<std::int32_t>(0x80290018), "the second VR mode did not report no device");
    Require(sceHmd2ReprojectionEnableVrMode(0x1234) == static_cast<std::int32_t>(0x80290001), "an unknown output mode did not report the invalid value");
    threw = false;
    try {
        sceHmd2Open();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    Require(threw, "open after failed initialization did not throw");
    std::puts("HMD2 tests passed");
    return 0;
}
