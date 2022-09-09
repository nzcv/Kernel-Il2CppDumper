@set ABI=arm64-v8a
@if DEFINED ANDROID_NDK_ROOT (
    @set ANDROID_NDK=%ANDROID_NDK_ROOT%
) else (
    @set ANDROID_NDK=D:\Android\android-ndk-r23
)
@if DEFINED ANDROID_SDK_ROOT (
    @set CMAKE=%ANDROID_SDK_ROOT%\cmake\3.22.1\bin\cmake.exe
) else (
    @set CMAKE=D:\Android\sdk\cmake\3.22.1\bin\cmake.exe
)
@cd %~dp0

@set BUILD_DIR=_build_%ABI%

rem @rd /s /q %BUILD_DIR%

@mkdir %BUILD_DIR%

@echo %BUILD_DIR%
@cd %BUILD_DIR%

@cd %BUILD_DIR%

@%CMAKE% -G "Ninja" ^
  -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake ^
  -DANDROID_ABI=%ABI% ^
  -DANDROID_NATIVE_API_LEVEL=24 ^
  -DCMAKE_MAKE_PROGRAM=D:\Android\sdk\cmake\3.18.1\bin\ninja.exe ^
  -DANDROID_STL=c++_static ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_C_FLAGS_RELEASE="-O3" ^
  -DCMAKE_CXX_FLAGS_RELEASE="-O3" ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=%~dp0\built\%ABI% ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=%~dp0\built\%ABI% ^
  ..

@%CMAKE% --build .

@cd ..