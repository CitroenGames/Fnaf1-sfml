@echo off
echo Building Fnaf-SFML project...
echo.

REM Build the project
cmake -B build
if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Starting game...
echo.

REM Run the executable
cd build\bin
if exist "Fnaf-SFML.exe" (
    start "Fnaf-SFML" "Fnaf-SFML.exe"
) else (
    echo Error: Fnaf-SFML.exe not found in build\bin directory
    echo Checking for executable in other locations...
    if exist "Release\Fnaf-SFML.exe" (
        cd Release
        start "Fnaf-SFML" "Fnaf-SFML.exe"
    ) else if exist "..\Release\Fnaf-SFML.exe" (
        cd ..\Release
        start "Fnaf-SFML" "Fnaf-SFML.exe"
    ) else (
        echo Could not find executable. Please check build output.
        pause
        exit /b 1
    )
)

echo Game launched!
pause