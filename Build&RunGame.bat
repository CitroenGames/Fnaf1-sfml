@echo off
echo Building Fnaf-SFML project...
echo.

REM Generate and build the project
sighmake Fnaf-SFML.buildscript
if %ERRORLEVEL% NEQ 0 (
    echo sighmake generation failed!
    pause
    exit /b 1
)

sighmake --build . --config Release --parallel 8
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
if exist "build\bin\Release\Fnaf-SFML.exe" (
    start "Fnaf-SFML" "build\bin\Release\Fnaf-SFML.exe"
) else (
    echo Could not find build\bin\Release\Fnaf-SFML.exe. Please check build output.
    pause
    exit /b 1
)

echo Game launched!
pause
