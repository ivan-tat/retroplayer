@echo off

set execpath=src\player
set testsong=src\player\test.s3m
set test_1=plays3m
set test_2=plays3m -noems
set test_3=plays3m -env -s22050 -m -8
set test_4=plays3m -env -s22050 -8
set test_5=plays3m -env -s22050 -m
set test_6=plays3m -env
set test_7=plays3m -env -noems
set test_8=smalls3m
set test_9=s3mosci

:_start
cls
echo Choose a test to run (or press CTRL+C to exit):
echo.
echo 1. %test_1%
echo 2. %test_2%
echo 3. %test_3%
echo 4. %test_4%
echo 5. %test_5%
echo 6. %test_6%
echo 7. %test_7%
echo 8. %test_8%
echo 9. %test_9%
echo.
echo (using test song: %testsong%)
echo.
choice /C:123456789

if errorlevel 9 goto _9
if errorlevel 8 goto _8
if errorlevel 7 goto _7
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
%execpath%\%test_1% %testsong%
goto _repeat

:_2
echo.
echo Running test 2...
%execpath%\%test_2% %testsong%
goto _repeat

:_3
echo.
echo Running test 3...
%execpath%\%test_3% %testsong%
goto _repeat

:_4
echo.
echo Running test 4...
%execpath%\%test_4% %testsong%
goto _repeat

:_5
echo.
echo Running test 5...
%execpath%\%test_5% %testsong%
goto _repeat

:_6
echo.
echo Running test 6...
%execpath%\%test_6% %testsong%
goto _repeat

:_7
echo.
echo Running test 7...
%execpath%\%test_7% %testsong%
goto _repeat

:_8
echo.
echo Running test 8...
%execpath%\%test_8% %testsong%
goto _repeat

:_9
echo.
echo Running test 9...
%execpath%\%test_9% %testsong%
goto _repeat

:_end
