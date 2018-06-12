
local client = GameClient()
print("client", client)
print(client.test())
print("send hello")
client.SendPacket("hello form lua")
print("send hello end")
client.SyncRun()