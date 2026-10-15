#include "SceTypes.hpp"
#include "prx/libc/include/general/VabiMacros.hpp"

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
    bool threw = false;
    try {
        sceHmd2Open();
    } catch (const std::runtime_error&) {
        threw = true;
    }
    Require(threw, "open after failed initialization did not throw");
    std::puts("HMD2 tests passed");
    return 0;
}
