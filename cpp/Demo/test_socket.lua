
local c = require "client"
print("c", c)

function on_connected()
    print("send hello")
    c.send("hello form lua")
    print("send hello end")
    print("send hello2")
    c.send("hello2 form lua")
    print("send hello2 end")
end

function on_disconnected()
    print("on_disconnected")
end

function on_msg(msg)
    print("on_msg", msg)
end

c.set_callbacks(on_connected, on_disconnected, on_connected)

print("connect")
c.connect("127.0.0.1", "11112")
print("sync_run ...")
c.sync_run()