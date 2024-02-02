# RemoteXR

at first I want to make any WebXR-ready device (like Vision Pro) to a SteamVR compatbile HMD, but SteamVR driver is too hard to develop, so I give up.

## Prerequisites

* Windows 10 or later, or Windows Server 2022+
* SteamVR
* Visual Studio 2022 (and CMake)

## How to build

in this project, you can build SteamVR driver by (running in `Developer PowerShell for VS 2022`):

```
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
./RemoteXR.sln
```

then run **Release Build** in Visual Studio, and Open `C:\Users\user\AppData\Local\openvr\openvrpaths.vrpath` and add `...\RemoteXR\build\remotexr_driver` path to `external_drivers` array.

It would be like:

```json
{
	"config" : 
	[
		"c:\\program files (x86)\\steam\\config"
	],
	"external_drivers" : 
	[
		"C:\\Users\\user\\Desktop\\RemoteXR\\build\\remotexr_driver"
	],
	"jsonid" : "vrpathreg",
	"log" : 
	[
		"c:\\program files (x86)\\steam\\logs"
	],
	"runtime" : 
	[
		"C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR"
	],
	"version" : 1
}
```

then make `C:\tmp` folder (this driver creates `C:\tmp\driver.log` file).

finally, run SteamVR, and you can see `C:\tmp\driver.log` file log. you can see this log realtime by `Get-Content C:\tmp\driver.log -Wait` in PowerShell.

## Current Status

```
HmdDriverFactory called (pin=IServerTrackedDeviceProvider_004)
Returning TrackedDeviceProvider
TrackedDeviceProvider::Init called
TrackedDeviceProvider::Init finish
MyHMDControlerDeviceDriver::Activate called (unObjectId=0)
successfly created d3d11 device (feature level=11.0)
MyHMDControlerDeviceDriver::Activate done
MyHMDControlerDeviceDriver::GetComponent called (pCNAV=IVRDisplayComponent_003)
MyHMDControlerDeviceDriver::GetComponent called (pCNAV=IVRDriverDirectModeComponent_008)
MyHMDControlerDeviceDriver::GetComponent called (pCNAV=IVRCameraComponent_003)
MyHMDControlerDeviceDriver::GetComponent called (pCNAV=IVRVirtualDisplay_002)
Returning IVRVirtualDisplay
MyHMDControlerDeviceDriver::GetComponent called (pCNAV=IVRControllerComponent_001)
MyHMDControlerDeviceDriver::EnterStandby called
MyHMDControlerDeviceDriver::EnterStandby called
MyHMDControlerDeviceDriver::EnterStandby called
MyHMDControlerDeviceDriver::EnterStandby called
```

`MyHMDControlerDeviceDriver::Present` should be called, but not. Please help me :sob:

## How to Build frontend

(currently there is almost zero frontend implementation, but you want to run it, just `pnpm install` and `pnpm vite`).