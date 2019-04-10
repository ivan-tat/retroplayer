@echo off

set execpath=src\player
set testsong=src\player\test.s3m

:_start
cls
echo Choose a test to run (or press CTRL+C to exit):
echo.
echo 1. plays3m
echo 2. plays3m -noems
echo 3. plays3m -env
echo 4. plays3m -env -noems
echo 5. smalls3m
echo 6. s3mosci
echo.
echo (using test song: %testsong%)
echo.
choice /C:123456

if errorlevel 6 goto _6
if errorlevel 5 goto _5
if errorlevel 4 goto _4
if errorlevel 3 goto _3
if errorlevel 2 goto _2
if errorlevel 1 goto _1

:_repeat
echo.
choice /C:yn Do you want to run a new test?
if errorlevel 2 goto _end
goto _start

:_1
echo.
echo Running test 1...
%execpath%\plays3m %testsong%
goto _repeat

:_2
echo.
echo Running test 2...
%execpath%\plays3m -noems %testsong%
goto _repeat

:_3
echo.
echo Running test 3...
%execpath%\plays3m -env %testsong%
goto _repeat

:_4
echo.
echo Running test 4...
%execpath%\plays3m -env -noems %testsong%
goto _repeat

:_5
echo.
echo Running test 5...
%execpath%\smalls3m %testsong%
goto _repeat

:_6
echo.
echo Running test 6...
%execpath%\s3mosci %testsong%
goto _repeat

:_end
