@echo off
cd /d "%~dp0"
echo [Current Directory]
echo "%CD%"
pause
echo 1. ".\*.sdf" file deleting...
del ".\*.sdf"
echo 2. ".\bin" folder deleting...
rmdir /s /q ".\bin"
echo 3. ".\ipch" folder deleting...
rmdir /s /q ".\ipch"
echo 4. ".\output" folder deleting...
rmdir /s /q ".\output"
pause
