@echo off

for %%i in (proto/*.proto) do (
    echo %%i
    protoc -I=proto --cpp_out=cpp proto/%%i
    ::protoc -I=proto --php_out=php proto/%%i
    protoc -I=proto --python_out=python proto/%%i
    protoc -I=proto --plugin=protoc-gen-lua=protoc-gen-lua.bat --lua_out=lua proto/%%i
)

pause
