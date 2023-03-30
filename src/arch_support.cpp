#include "arch_support.h"

#include <iostream>

void DisplayArchSupport()
{
    std::cout << "AVX support: " << g_bIsAVXSupported << std::endl;
    std::cout << "AVX2 support: " << g_bIsAVX2Supported << std::endl;
    std::cout << "AVX512 support: " << g_bIsAVX512Supported << std::endl;
    std::cout << "SSSE3 support: " << g_bIsSSSE3Supported << std::endl;
    std::cout << "SSSE4 support: " << g_bIsSSSE4Supported << std::endl;
}

bool IsAVXSupported(){return g_bIsAVXSupported;}
bool IsAVX2Supported(){return g_bIsAVX2Supported;}
bool IsAVX512Supported(){return g_bIsAVX512Supported;}
bool IsSSSE3Supported(){return g_bIsSSSE3Supported;}   
bool IsSSSE4Supported(){return g_bIsSSSE4Supported;}
