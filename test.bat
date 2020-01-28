@echo off

set PlayTestPath=src\player
set PlayTestSong=src\player\test.s3m
set MiscTestPath=src\tests
set test_1=plays3m
set test_2=plays3m -noems
set test_3=plays3m -env -s22050 -m -8
set test_4=plays3m -env -s22050 -8
set test_5=plays3m -env -s22050 -m
set test_6=plays3m -env
set test_7=plays3m -env -noems
set test_8=smalls3m
set test_9=playosci
set test_10=test
set test_11=testcrt
set test_12=testexec
set test_13=testheap

:_start
mode.com co80,50
echo Choose a test to run (0 to stop test):
echo.
echo 0. Exit.
echo 1. %test_1%
echo 2. %test_2%
echo 3. %test_3%
echo 4. %test_4%
echo 5. %test_5%
echo 6. %test_6%
echo 7. %test_7%
echo 8. %test_8%
echo 9. %test_9%
echo A. %test_10%
echo B. %test_11%
echo C. %test_12%
echo D. %test_13%
echo.
echo (using test song: %PlayTestSong%)
echo.
choice /C:0123456789ABCD

if errorlevel 14 goto _13
if errorlevel 13 goto _12
if errorlevel 12 goto _11
if errorlevel 11 goto _10
if errorlevel 10 goto _9
if errorlevel 9 goto _8
if errorlevel 8 goto _7
if errorlevel 7 goto _6
if errorlevel 6 goto _5
if errorlevel 5 goto _4
if errorlevel 4 goto _3
if errorlevel 3 goto _2
if errorlevel 2 goto _1
if errorlevel 1 goto _0

:_repeat
echo.
choice /C:NY Do you want to run a new test?
if errorlevel 2 goto _start
echo.
goto _end

:_0
echo.
echo Exiting...
goto _end

:_1
echo.
echo Running test 1...
%PlayTestPath%\%test_1% %PlayTestSong%
goto _repeat

:_2
echo.
echo Running test 2...
%PlayTestPath%\%test_2% %PlayTestSong%
goto _repeat

:_3
echo.
echo Running test 3...
%PlayTestPath%\%test_3% %PlayTestSong%
goto _repeat

:_4
echo.
echo Running test 4...
%PlayTestPath%\%test_4% %PlayTestSong%
goto _repeat

:_5
echo.
echo Running test 5...
%PlayTestPath%\%test_5% %PlayTestSong%
goto _repeat

:_6
echo.
echo Running test 6...
%PlayTestPath%\%test_6% %PlayTestSong%
goto _repeat

:_7
echo.
echo Running test 7...
%PlayTestPath%\%test_7% %PlayTestSong%
goto _repeat

:_8
echo.
echo Running test 8...
%PlayTestPath%\%test_8% %PlayTestSong%
goto _repeat

:_9
echo.
echo Running test 9...
%PlayTestPath%\%test_9% %PlayTestSong%
goto _repeat

:_10
echo.
echo Running test 10...
%MiscTestPath%\%test_10%
goto _repeat

:_11
echo.
echo Running test 11...
%MiscTestPath%\%test_11%
goto _repeat

:_12
echo.
echo Running test 12...
%MiscTestPath%\%test_12%
goto _repeat

:_13
echo.
echo Running test 13...
%MiscTestPath%\%test_13%
goto _repeat

:_end
