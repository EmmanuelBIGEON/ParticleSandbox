# ParticleSandbox (under construction)
**A particle simulator and sandbox in C++**
(under construction)

![Preview ParticleSandbox](preview.png)

### Dependencies
I provide the dependencies for Visual Studio 17 2022 (x64 and Debug) but otherwise, you have to provide them and edit the environment file (project/env.bat)

- **GLFW**
- **FreeType**
- **GL3D** _(header only, no need to replace it)_
- **GLM** _(header only, no need to replace it)_

This application uses AVX2 for performance and prefer not to use multithread. 
However, if you're machine doesn't suppport AVX2, application render with multithread.

## Build

Currently only MSVC is supported because of the check for support of AVX/AVX2/AVX512/SSSE3/SSSE4 (__cpuid)

```
Default Parameters:
Architecture=x64
Generator="Visual Studio 17 2022

$ particle --build -G [generator] -A [platform/arch]  
or
$ particle -b
```