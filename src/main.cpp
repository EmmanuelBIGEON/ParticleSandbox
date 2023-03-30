#include <iostream>

#include "common/Application.h"
#include "common/Object.h"
#include <stdio.h>
#include <immintrin.h>
#include <limits.h>
#include <intrin.h>
#include <stdint.h>

#include "arch_support.h"

bool g_bIsAVXSupported = false;
bool g_bIsAVX2Supported = false;
bool g_bIsAVX512Supported = false;
bool g_bIsSSSE3Supported = false;
bool g_bIsSSSE4Supported = false;


int main()
{
    // initialize rand with timestamp.
    srand(time(NULL));

    // check for support of AVX 512 
    // The program guess that 256 is available. 
    // The application won't work on a computer that doesn't support at least 256.
    // TODO : check for support of 512, and if ok, implement it in the application for better performance.

    // Check for AVX support
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    bool avxSupported = cpuInfo[2] & (1 << 28);

    if (avxSupported) {
        // AVX is supported
        g_bIsAVXSupported = true;
    } else {
        // AVX is not supported
        g_bIsAVXSupported = false;
    }

    // check for AVX2 support
    __cpuid(cpuInfo, 7);
    bool avx2Supported = cpuInfo[1] & (1 << 5);

    if (avx2Supported) {
        // AVX2 is supported
        g_bIsAVX2Supported = true;
    } else {
        // AVX2 is not supported
        g_bIsAVX2Supported = false;
    }

    // check for AVX512 support
    __cpuid(cpuInfo, 7);
    bool avx512Supported = cpuInfo[1] & (1 << 16);

    if (avx512Supported) {
        // AVX512 is supported
        g_bIsAVX512Supported = true;
    } else {
        // AVX512 is not supported
        g_bIsAVX512Supported = false;
    }

    // check for SSSE3 support
    __cpuid(cpuInfo, 1);
    bool ssse3Supported = cpuInfo[2] & (1 << 9);

    if (ssse3Supported) {
        // SSSE3 is supported
        g_bIsSSSE3Supported = true;
    } else {
        // SSSE3 is not supported
        g_bIsSSSE3Supported = false;
    }

    // check for SSSE4 support
    __cpuid(cpuInfo, 1);
    bool ssse4Supported = cpuInfo[2] & (1 << 19);

    if (ssse4Supported) {
        // SSSE4 is supported
        g_bIsSSSE4Supported = true;
    } else {
        // SSSE4 is not supported
        g_bIsSSSE4Supported = false;
    }

    DisplayArchSupport();


    Application* app = Application::GetInstance();
    app->Run();

    return 0;
}

