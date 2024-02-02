#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <memory>
#include <openvr_driver.h>
#include <d3d11.h>

#pragma comment(lib,"d3d11.lib")

#define HMD_DLL_EXPORT extern "C" __declspec(dllexport)

void logprintf(const char* fmt, ...) {
    FILE* fp = fopen("C:/tmp/driver.log", "a");
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
    fclose(fp);
}

// class MyHMDDisplayComponent: public vr::IVRDisplayComponent
// {
//     bool IsDisplayOnDesktop() {
//         logprintf("MyHMDDisplayComponent::IsDisplayOnDesktop called\n");
//         return false;
//     }

//     bool IsDisplayRealDisplay() {
//         logprintf("MyHMDDisplayComponent::IsDisplayRealDisplay called\n");
//         return false;
//     }

//     void GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) {
//         logprintf("MyHMDDisplayComponent::GetWindowBounds called\n");
//         *pnX = 0;
//         *pnY = 0;
//         *pnWidth = 1920;
//         *pnHeight = 1080;
//     }

//     void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) {
//         logprintf("MyHMDDisplayComponent::GetRecommendedRenderTargetSize called\n");
//         *pnWidth = 640;
//         *pnHeight = 360;
//     }

//     void GetEyeOutputViewport( vr::EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) {
//         logprintf("MyHMDDisplayComponent::GetEyeOutputViewport called (eye=%d)\n", eEye);
//         *pnX = 0;
//         *pnY = 0;
//         *pnWidth = 640 / 2;
//         *pnHeight = 360;
//         if (eEye == vr::Eye_Right) {
//             *pnX = *pnWidth;
//         }
//     }

//     void GetProjectionRaw( vr::EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom ) {
//         logprintf("MyHMDDisplayComponent::GetProjectionRaw called (eye=%d)\n", eEye);
//         *pfLeft = -1.0;
//         *pfRight = 1.0;
//         *pfTop = -1.0;
//         *pfBottom = 1.0;
//     }

//     vr::DistortionCoordinates_t __stdcall ComputeDistortion( vr::EVREye eEye, float fU, float fV ) {
//         // logprintf("MyHMDDisplayComponent::ComputeDistortion called (eye=%d, fU=%f, fV=%f)\n", eEye, fU, fV);
//         vr::DistortionCoordinates_t coordinates{};
//         coordinates.rfBlue[ 0 ] = fU;
//         coordinates.rfBlue[ 1 ] = fV;
//         coordinates.rfGreen[ 0 ] = fU;
//         coordinates.rfGreen[ 1 ] = fV;
//         coordinates.rfRed[ 0 ] = fU;
//         coordinates.rfRed[ 1 ] = fV;

//         // logprintf("MyHMDDisplayComponent::ComputeDistortion done (eye=%d, fU=%f, fV=%f)\n", eEye, fU, fV);
//         return coordinates;
//     }

//     bool ComputeInverseDistortion( vr::HmdVector2_t *pResult, vr::EVREye eEye, uint32_t unChannel, float fU, float fV ) {
//         logprintf("MyHMDDisplayComponent::ComputeInverseDistortion called (eye=%d, unChannel=%d, fU=%f, fV=%f)\n", eEye, unChannel, fU, fV);
//         return false;
//     }
// };

class MyHMDControlerDeviceDriver: public vr::ITrackedDeviceServerDriver, vr::IVRVirtualDisplay
{
    ID3D11Device* d3d11Device;

    // std::unique_ptr<MyHMDDisplayComponent> display_component;
    vr::EVRInitError Activate(uint32_t unObjectId) {
        logprintf("MyHMDControlerDeviceDriver::Activate called (unObjectId=%d)\n", unObjectId);
        // display_component = std::make_unique<MyHMDDisplayComponent>();

        D3D_FEATURE_LEVEL deviceFeatureLevel;
        HRESULT result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_VIDEO_SUPPORT, NULL, 0, D3D11_SDK_VERSION, &this->d3d11Device, &deviceFeatureLevel, NULL);
        if (result != S_OK) {
            logprintf("failed to create D3D11 device\n");
            return vr::VRInitError_Driver_Failed;
        }
        else {
            logprintf("successfly created d3d11 device (feature level=%d.%d)\n", deviceFeatureLevel >> 12, (deviceFeatureLevel >> 8) & 0xF);
        }

        vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
        vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "RemoteXR HMD");
    	vr::VRProperties()->SetFloatProperty(container, vr::Prop_SecondsFromVsyncToPhotons_Float, 0.11f );

        logprintf("MyHMDControlerDeviceDriver::Activate done\n");
        return vr::VRInitError_None;
    }

    void Deactivate() {
        logprintf("MyHMDControlerDeviceDriver::Deactivate called\n");
        delete this->d3d11Device;
        logprintf("MyHMDControlerDeviceDriver::Deactivate finished\n");
    }

    void* GetComponent(const char* pchComponentNameAndVersion) {
        logprintf("MyHMDControlerDeviceDriver::GetComponent called (pCNAV=%s)\n", pchComponentNameAndVersion);
        if (0 == strcmp(vr::IVRVirtualDisplay_Version, pchComponentNameAndVersion)) {
            // return display_component.get();
            logprintf("Returning IVRVirtualDisplay\n");
            return static_cast<vr::IVRVirtualDisplay*>(this);
        }
        return NULL;
    }

    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
        logprintf("MyHMDControlerDeviceDriver::DebugRequest called (pchRequest=%s)\n", pchRequest);
    }

    vr::DriverPose_t GetPose() {
        logprintf("MyHMDControlerDeviceDriver::GetPose called\n");
        vr::DriverPose_t pose = { 0 };
        pose.poseIsValid = true;
        pose.result = vr::TrackingResult_Running_OK;
        pose.deviceIsConnected = true;
        pose.qWorldFromDriverRotation = { 0, 0, 0, 1 };
        pose.qDriverFromHeadRotation = { 0, 0, 0, 1 };
        return pose;
    }

    void EnterStandby() {
        logprintf("MyHMDControlerDeviceDriver::EnterStandby called\n");
    }

    void Present(const vr::PresentInfo_t* pPresentInfo, uint32_t unPresentInfoSize) {
        logprintf("MyHMDControlerDeviceDriver::Present called\n");
    }

    void WaitForPresent() {
        logprintf("MyHMDControlerDeviceDriver::WaitForPresent called\n");
    }

    bool GetTimeSinceLastVsync(float* pfSecondsSinceLastVsync, uint64_t* pulFrameCounter) {
        logprintf("MyHMDControlerDeviceDriver::GetTimeSinceLastVsync called\n");
        *pfSecondsSinceLastVsync = 0.1;
        *pulFrameCounter = 1;
    

        return true;
    }
};

class TrackedDeviceProvider: public vr::IServerTrackedDeviceProvider
{
    std::unique_ptr<MyHMDControlerDeviceDriver> hmd;

    vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) {
        logprintf("TrackedDeviceProvider::Init called\n");
        VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

        hmd = std::make_unique<MyHMDControlerDeviceDriver>();

        if (!vr::VRServerDriverHost()->TrackedDeviceAdded("remotexr_hmd", vr::TrackedDeviceClass_HMD, hmd.get())) {
            logprintf("Failed to create HMD device\n");

            return vr::VRInitError_Driver_HmdUnknown;
        }

        logprintf("TrackedDeviceProvider::Init finish\n");

        return vr::VRInitError_None;
    };

    const char* const* GetInterfaceVersions() {
        return vr::k_InterfaceVersions;
    }

    void Cleanup() {
        logprintf("TrackedDeviceProvider::Cleanup called\n");
        hmd = NULL;
        VR_CLEANUP_SERVER_DRIVER_CONTEXT();
    }

    void RunFrame() {
        // TODO
    }

    bool ShouldBlockStandbyMode() {
        return false;
    }

    void EnterStandby() {}
    void LeaveStandby() {}
};

HMD_DLL_EXPORT void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode) {
    logprintf("HmdDriverFactory called (pin=%s)\n", pInterfaceName);
    if (0 == strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName)) {
        // TODO: return IServerTrackedDeviceProvider
        logprintf("Returning TrackedDeviceProvider\n");
        return new TrackedDeviceProvider();
        // return NULL;
    }
    return NULL;
}