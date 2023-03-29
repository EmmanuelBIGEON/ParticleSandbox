#include "arch_support.h"

#include <stdio.h>
#include <immintrin.h>
#include <iostream>



void DisplayArchSupport()
{

    std::cout << "AVX is supported: " << g_bIsAVXSupported << std::endl;
    std::cout << "AVX2 is supported: " << g_bIsAVX2Supported << std::endl;
    std::cout << "AVX512 is supported: " << g_bIsAVX512Supported << std::endl;
    std::cout << "SSSE3 is supported: " << g_bIsSSSE3Supported << std::endl;
    std::cout << "SSSE4 is supported: " << g_bIsSSSE4Supported << std::endl;

}

bool IsAVXSupported()
{
    return g_bIsAVXSupported;
}

bool IsAVX2Supported()
{
    return g_bIsAVX2Supported;
}

bool IsAVX512Supported()
{
    return g_bIsAVX512Supported;
}

bool IsSSSE3Supported()
{
    return g_bIsSSSE3Supported;
}   

bool IsSSSE4Supported()
{
    return g_bIsSSSE4Supported;
}
