@echo off

for %%i in (./*.thrift) do (
    echo %%i
    thrift-0.11.0 --gen cpp %%i
    thrift-0.11.0 --gen php %%i
    thrift-0.11.0 --gen lua %%i
)

pause
