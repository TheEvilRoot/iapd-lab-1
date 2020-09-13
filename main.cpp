#pragma comment (lib, "setupapi.lib")

#include <iostream>
#include <cstdint>
#include <iomanip>
#include <memory>

#include <Windows.h>
#include <SetupAPI.h>

#define GetDeviceProp SetupDiGetDeviceRegistryProperty
#define GetDevices SetupDiGetClassDevs
#define NextDevice SetupDiEnumDeviceInfo

auto getDeviceProp(HDEVINFO infoSet, SP_DEVINFO_DATA device, unsigned long prop) {
  unsigned long bufferSize = 16;
  unsigned long requiredSize = 0;
  auto buffer = std::make_unique<unsigned char[]>(bufferSize);

  while (!GetDeviceProp(infoSet, &device, prop, nullptr, buffer.get(), bufferSize, &requiredSize)) {
    auto error = GetLastError();
    if (error == ERROR_INSUFFICIENT_BUFFER) {
      buffer = std::make_unique<unsigned char[]>(requiredSize);
      bufferSize = requiredSize;
    } else {
      std::cerr << "[SetupDiGetDeviceRegistryProperty] Error: " << error << "\n";
      return std::string();
    }
  }
  
  return std::string(reinterpret_cast<char *>(buffer.get()));
}

int main() {
  auto infoSet = GetDevices(nullptr, "PCI", nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT);
  
  SP_DEVINFO_DATA device { };
  device.cbSize = sizeof(device);

  for (unsigned long i = 0; NextDevice(infoSet, i, &device); i++) {
    auto deviceId = getDeviceProp(infoSet, device, SPDRP_DEVICEDESC);
    auto vendorId = getDeviceProp(infoSet, device, SPDRP_MFG);
    std::cout << "Device: " << deviceId << "\n";
    std::cout << "Vendor: " << vendorId << "\n";
    std::cout << "\n";
  }

  SetupDiDestroyDeviceInfoList(infoSet);
}