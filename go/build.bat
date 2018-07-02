@echo off
gofmt -w ./cmd
cd @bin
go build ../tests/client
go build ../tests/server
go build ../cmd/gate
cd ../