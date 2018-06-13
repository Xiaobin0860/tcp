-----------------------------------------------------------------------------
-- TCP sample: Little program to send text lines to a given host/port
-- LuaSocket sample files
-- Author: Diego Nehab
-----------------------------------------------------------------------------
local socket = require("socket")
local pack = require("luabpack")
host = host or "localhost"
port = port or 11112
if arg then
	host = arg[1] or host
	port = arg[2] or port
end
print("Attempting connection to host '" ..host.. "' and port " ..port.. "...")
c = assert(socket.connect(host, port))
print("Connected! Please type stuff (empty line to stop):")
msg = io.read()
while msg and msg ~= "" and not e do
    local msg_len = pack.bpack("i", #msg)
	assert(c:send(msg_len .. msg))
	msg = io.read()
end
