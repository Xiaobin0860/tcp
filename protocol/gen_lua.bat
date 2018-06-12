@echo off

for %%i in (proto/*.proto) do (
    echo %%i
    protoc -I=proto -olua/%%~ni.pb proto/%%~ni.proto
)

pause
