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
tcp = socket.tcp()
print("Attempting connection to host '" ..host.. "' and port " ..port.. "...")
tcp:connect(host, port)
print("Connected! Please type stuff (empty line to stop):")
tcp:settimeout(0)

local data = ""
function handle_read()
    print("recv ...")
    local s, status, partial = tcp:receive(1024)
    print("status => ", status, s, partial)
    if status == "closed" then
        data = ""
        return false
    end 
    if partial then
        data = data .. partial
    end
    local cur_len = #data
    print("cur_len=", cur_len)
    if cur_len > 4 then
        print("upack len")
        local size = pack.bunpack("i", string.sub(data,1,4))
        print("size", size)
        if size <= 0 then
            print("invalid size", size)
            data = ""
            return false
        end
        if cur_len >= size+4 then
            local pack = string.sub(data, 5, 4 + size)
            print("recv", pack)
            data = string.sub(data, 4 + size + 1)
        end
    end
    return true
end

msg = io.read()

while msg and msg ~= "" and not e do
    local msg_len = pack.bpack("i", #msg)
	tcp:send(msg_len .. msg)
    handle_read()
	msg = io.read()
end

tcp:close()
