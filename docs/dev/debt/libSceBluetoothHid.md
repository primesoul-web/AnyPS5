# libSceBluetoothHid technical debt

### Silent stubs

- [libSceBluetoothHid](../../../core/libs/prx/libSceBluetoothHid/Export.cpp) implements only the no-device path: initialization and callback and device registration succeed, the callback is never called because no device connects, and calls that need a connected device (`sceBluetoothHidGetDeviceName`, `sceBluetoothHidGetInputReport`, `sceBluetoothHidSetOutputReport`) throw

### Unknown function info

- [libSceBluetoothHid](../../../core/libs/prx/libSceBluetoothHid/Export.cpp) (libSceBluetoothHid) - signatures taken from PPSA01317: `sceBluetoothHidInit()` and `sceBluetoothHidUnregisterCallback()` without arguments, `sceBluetoothHidRegisterCallback(callback, 0, param)` with a 16-byte param, and `sceBluetoothHidRegisterDevice` / `sceBluetoothHidUnregisterDevice(vendorId, productId)`; the report and device name signatures follow [SharpProspero](https://github.com/SvenGDK/SharpProspero/blob/5dfdb4b0b166e83dc9ae275001afb695f12c7174/src/SharpProspero/Interop/Bluetooth/SceBluetoothHid.cs), which recovered the argument shapes from the module binary: `sceBluetoothHidGetDeviceName(handle, name)`, `sceBluetoothHidGetInputReport(report, reportId)` and `sceBluetoothHidSetOutputReport(handle, reportId, data, length)`. The module reports failures as negative `0x8154xxxx` values there, but which constant a call without a connected device returns is unknown, so these throw instead of guessing one
