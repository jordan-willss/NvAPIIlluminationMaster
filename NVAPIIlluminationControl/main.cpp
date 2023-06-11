#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <string>
#include "ExternalLibs/nvapi.h"

#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif

NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS GetIlluminationZones(NvPhysicalGpuHandle hPhysicalGpu);
NvAPI_Status SetIlluminationZones(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS illuminationZonesInfo);

extern "C" {
    DLL_EXPORT void GetPhysicalGPUs();
    DLL_EXPORT int GetIlluminationType(int gpuIndex, int zoneIndex);
    DLL_EXPORT int GetIlluminationControlMode(int gpuIndex, int zoneIndex);
    DLL_EXPORT BOOL SetGPUIlluminationRGB(int gpuIndex, int zoneIndex, int r, int g, int b, int brightness);
    DLL_EXPORT BOOL SetGPUIlluminationColorFixed(int gpuIndex, int zoneIndex, int brightness);
    DLL_EXPORT BOOL SetGPUIlluminationRGBW(int gpuIndex, int zoneIndex, int r, int g, int b, int w, int brightness);
    DLL_EXPORT BOOL SetGPUIlluminationSingleColor(int gpuIndex, int zoneIndex, int brightness);
}

NvPhysicalGpuHandle* gpuHandles;

int main()
{
    GetPhysicalGPUs();

    SetGPUIlluminationRGBW(0, 0, 255, 0, 0, 0, 255);

    return 0;
}

void GetPhysicalGPUs()
{
    NvAPI_Status nvapiStatus = NVAPI_OK;

    // Initialize NVAPI
    if ((nvapiStatus = NvAPI_Initialize()) != NVAPI_OK)
    {
        printf("NvAPI_Initialize() failed with status %d\n", nvapiStatus);
        printf("Failed to initialize NVAPI\n");
        return;
    }

    NvPhysicalGpuHandle* hGPU_a = (NvPhysicalGpuHandle*)malloc(NVAPI_MAX_PHYSICAL_GPUS * sizeof(NvPhysicalGpuHandle));
    NvU32 gpuCount = 0;
    nvapiStatus = NvAPI_EnumPhysicalGPUs(hGPU_a, &gpuCount);
    if (nvapiStatus != NVAPI_OK)
    {
        printf("NvAPI_EnumPhysicalGPUs() failed with status %d\n", nvapiStatus);
        printf("Failed to enumerate physical GPUs\n");
        free(hGPU_a); // Free the allocated memory before returning NULL
        return;
    }
    printf("Total number of GPUs = %u\n", gpuCount);

    if (gpuCount > 0)
    {
        printf("GPU Handles:\n");
        for (int i = 0; i < gpuCount; i++)
        {
            // F*ck you print function, I'll do what I want ty
            printf("%08p\n", hGPU_a[i]);
        }
        printf("\n");
    }

    // Clean up NVAPI.
    NvAPI_Unload();

    gpuHandles = hGPU_a;
}

const char* NvAPI_GetErrorMessage(NvAPI_Status status)
{
    static NvAPI_ShortString errorMessage;
    NvAPI_GetErrorMessage(status, errorMessage);
    return errorMessage;
}

BOOL SetGPUIlluminationRGB(int gpuIndex, int zoneIndex, int r, int g, int b, int brightness)
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS initialIlluminationZoneParams = GetIlluminationZones(gpuHandles[gpuIndex]);

    initialIlluminationZoneParams.zones[zoneIndex].data.rgb.data.manualRGB.rgbParams.colorR = r;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgb.data.manualRGB.rgbParams.colorG = g;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgb.data.manualRGB.rgbParams.colorB = b;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgb.data.manualRGB.rgbParams.brightnessPct = brightness;

    NvAPI_Status nvapiStatus = SetIlluminationZones(gpuHandles[0], initialIlluminationZoneParams);
    if (nvapiStatus == NVAPI_OK) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

BOOL SetGPUIlluminationColorFixed(int gpuIndex, int zoneIndex, int brightness)
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS initialIlluminationZoneParams = GetIlluminationZones(gpuHandles[gpuIndex]);

    initialIlluminationZoneParams.zones[zoneIndex].data.colorFixed.data.manualColorFixed.colorFixedParams.brightnessPct = brightness;

    NvAPI_Status nvapiStatus = SetIlluminationZones(gpuHandles[0], initialIlluminationZoneParams);
    if (nvapiStatus == NVAPI_OK) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

BOOL SetGPUIlluminationRGBW(int gpuIndex, int zoneIndex, int r, int g, int b, int w, int brightness)
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS initialIlluminationZoneParams = GetIlluminationZones(gpuHandles[gpuIndex]);

    initialIlluminationZoneParams.zones[zoneIndex].data.rgbw.data.manualRGBW.rgbwParams.colorR = r;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgbw.data.manualRGBW.rgbwParams.colorG = g;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgbw.data.manualRGBW.rgbwParams.colorB = b;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgbw.data.manualRGBW.rgbwParams.colorW = w;
    initialIlluminationZoneParams.zones[zoneIndex].data.rgbw.data.manualRGBW.rgbwParams.brightnessPct = brightness;

    NvAPI_Status nvapiStatus = SetIlluminationZones(gpuHandles[0], initialIlluminationZoneParams);
    if (nvapiStatus == NVAPI_OK) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

BOOL SetGPUIlluminationSingleColor(int gpuIndex, int zoneIndex, int brightness)
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS initialIlluminationZoneParams = GetIlluminationZones(gpuHandles[gpuIndex]);

    initialIlluminationZoneParams.zones[zoneIndex].data.singleColor.data.manualSingleColor.singleColorParams.brightnessPct = brightness;

    NvAPI_Status nvapiStatus = SetIlluminationZones(gpuHandles[0], initialIlluminationZoneParams);
    if (nvapiStatus == NVAPI_OK) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

int GetIlluminationType(int gpuIndex, int zoneIndex) {
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS illuminationZonesInfo = GetIlluminationZones(gpuHandles[gpuIndex]);
    return illuminationZonesInfo.zones[zoneIndex].type;
}

int GetIlluminationControlMode(int gpuIndex, int zoneIndex) {
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS illuminationZonesInfo = GetIlluminationZones(gpuHandles[gpuIndex]);
    return illuminationZonesInfo.zones[zoneIndex].ctrlMode;
}

NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS GetIlluminationZones(NvPhysicalGpuHandle hPhysicalGpu) {
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS illuminationZonesInfo;
    memset(&illuminationZonesInfo, 0, sizeof(illuminationZonesInfo));
    illuminationZonesInfo.version = NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_VER;

    /*typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1
    {
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE  type;
    NV_GPU_CLIENT_ILLUM_CTRL_MODE  ctrlMode;
    union
    {
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB           rgb;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED   colorFixed;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGBW          rgbw;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR  singleColor;
        NvU8                                                rsvd[64];
    } data;
    NvU8    rsvd[64];
    } NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1;*/

    NvAPI_Status nvapiStatus = NvAPI_GPU_ClientIllumZonesGetControl(hPhysicalGpu, &illuminationZonesInfo);
    if (nvapiStatus == NVAPI_OK)
    {
        for (NvU32 i = 0; i < illuminationZonesInfo.numIllumZonesControl; i++)
        {
            std::string zoneTypeValues[5] = {
                "NV_GPU_CLIENT_ILLUM_ZONE_TYPE_INVALID",
                "NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB",
                "NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED",
                "NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW",
                "NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR"
            };

            printf("----------------------------\n\n");

            NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1 zone = illuminationZonesInfo.zones[i];
            printf("NvAPI_GPU_ClientIllumZonesGetControl() for Zone %u:\n", i);
            printf("Type: %s\n", zoneTypeValues[static_cast<int>(zone.type)].c_str());
            printf("CtrlMode: %u\n", zone.ctrlMode);

            /* Zone Types
            NV_GPU_CLIENT_ILLUM_ZONE_TYPE_INVALID = 0,
            NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB,
            NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED,
            NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW,
            NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR,
            */

            if (zone.type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB) {
                NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB rgbData = zone.data.rgb;

                int red = rgbData.data.manualRGB.rgbParams.colorR;
                int green = rgbData.data.manualRGB.rgbParams.colorG;
                int blue = rgbData.data.manualRGB.rgbParams.colorB;
                int brightness = rgbData.data.manualRGB.rgbParams.brightnessPct;

                printf("RGB data: Red = %d, Green = %d, Blue = %d, Brightness = %d\n", red, green, blue, brightness);
            }
            else if (zone.type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED) {
                NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED fixedColorData = zone.data.colorFixed;

                int brightness = fixedColorData.data.manualColorFixed.colorFixedParams.brightnessPct;

                printf("Fixed Color data: Brightness = %d\n", brightness);
            }
            else if (zone.type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW) {
                NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGBW rgbwData = zone.data.rgbw;

                int red = rgbwData.data.manualRGBW.rgbwParams.colorR;
                int green = rgbwData.data.manualRGBW.rgbwParams.colorG;
                int blue = rgbwData.data.manualRGBW.rgbwParams.colorB;
                int white = rgbwData.data.manualRGBW.rgbwParams.colorW;
                int brightness = rgbwData.data.manualRGBW.rgbwParams.brightnessPct;

                printf("RGBW data: Red = %d, Green = %d, Blue = %d, White = %d, Brightness = %d\n", red, green, blue, white, brightness);
            }
            else if (zone.type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR) {
                NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR singleColorData = zone.data.singleColor;

                int brightness = singleColorData.data.manualSingleColor.singleColorParams.brightnessPct;

                printf("Single Color data: Brightness = %d\n", brightness);
            }

            printf("\n");
        }
    }
    else
    {
        printf("NvAPI_GPU_ClientIllumZonesGetControl() failed with status %d: %s\n", nvapiStatus, NvAPI_GetErrorMessage(nvapiStatus));
    }

    return illuminationZonesInfo;
}

NvAPI_Status SetIlluminationZones(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS illuminationZonesInfo) {
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS* illuminationZonesInfoPtr = &illuminationZonesInfo;
    NvAPI_Status nvapiStatus = NvAPI_GPU_ClientIllumZonesSetControl(hPhysicalGpu, illuminationZonesInfoPtr);
    if (nvapiStatus == NVAPI_OK) {
        GetIlluminationZones(hPhysicalGpu);
    }
    else {
        printf("Failed to set illumination zones for %08p\n\n", hPhysicalGpu);
    }
    return nvapiStatus;
}