@echo off
echo Installing dependencies...
call npm install
echo.
echo Deleting database...
del /q database.sqlite 2>nul
echo.
echo Done! Run start.bat to start the server.
pause
