# DtsDecoderThis repository builds a decoder DLL for playing DTS audio on Raspberry Pi4.The decoder itself is from there: https://github.com/foo86/dcadecI’ve modified it heavily. Ported it to C++ (non-trivial amount of unnecessary complexity was caused by the lack of destructors in C), also improved a few things here and there.I’ve also created a COM wrapper, for easier integration into .NET Core.The source code I have written (everything except what's in `libdcadec` subfolder) is dual-licensed, pick LGPL or MIT.The code in `libdcadec` subfolder, and therefore this complete project, is LGPL.## Build Instructions```mkdir buildcd buildcmake ../makestrip libDtsDecoder.so```## Bonus ChapterDespite I don’t actually need Windows DLL of that, you can build one from this code. I have used it for testing before porting to ARM Linux. You’ll need visual studio 2017. Technically, the freeware community edition is fine. Open `DtsDecoder.sln` and build, I've only tested Win64 version.