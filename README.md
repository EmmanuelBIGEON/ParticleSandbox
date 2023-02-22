# ParticleSandbox
**A particle simulator and sandbox in C++**

### Dependencies
I provide the dependencies for Visual Studio 17 2022 (x64 and Debug) but otherwise, you have to provide them and edit the environment file (project/env.bat)

- **GLFW**
- **FreeType**
- **GL3D** _(header only, no need to replace it)_
- **GLM** _(header only, no need to replace it)_

## Build

```
Default Parameters:
Architecture=x64
Generator="Visual Studio 17 2022

$ particle --build -G [generator] -A [platform/arch]  
or
$ particle -b
```