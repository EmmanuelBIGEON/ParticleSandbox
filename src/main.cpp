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

// Strange ! AVX seems available when the cpu says no.. maybe detection of arch malfunctionning.
#if 0
    // test a AVX command
    float avxvalue = 4.0f;
    __m128 avx = _mm_set1_ps(avxvalue);
    // basic operation
    __m128 avxresult = _mm_add_ps(avx, avx);
    // store the result in a float array
    float result[4];
    _mm_store_ps(result, avxresult);
    // display the result
    std::cout << "AVX result : " << result[0] << " " << result[1] << " " << result[2] << " " << result[3] << std::endl;
#endif

#ifdef _MSC_VER
    // Check for AVX support
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    g_bIsAVXSupported = cpuInfo[2] & (1 << 28);
    __cpuid(cpuInfo, 7);
    g_bIsAVX2Supported = cpuInfo[1] & (1 << 5);
    g_bIsAVX512Supported = cpuInfo[1] & (1 << 16);
    g_bIsSSSE3Supported = cpuInfo[2] & (1 << 9);
    g_bIsSSSE4Supported = cpuInfo[2] & (1 << 19);
    
    DisplayArchSupport();
#endif

    g_bIsAVX2Supported = false;


    Application* app = Application::GetInstance();
    app->Run();
    return 0;
}

