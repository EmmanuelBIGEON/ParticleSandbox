#pragma once

// Check for support of MSEEE and AVX
void DisplayArchSupport();

bool IsAVXSupported();
bool IsAVX2Supported();
bool IsAVX512Supported();
bool IsSSSE3Supported();    
bool IsSSSE4Supported();

extern bool g_bIsAVXSupported;
extern bool g_bIsAVX2Supported;
extern bool g_bIsAVX512Supported;
extern bool g_bIsSSSE3Supported;
extern bool g_bIsSSSE4Supported;
