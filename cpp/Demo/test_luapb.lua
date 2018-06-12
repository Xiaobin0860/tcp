local pb = require("pb")
print("pb", pb)

print(g_hello)
print("g_s size: ", #g_s)

pb.Import("addressbook.proto")
local ab = pb.New("tutorial.AddressBook")
pb.ParseFromString(ab, g_s)
print(pb.DebugString(ab))

print("k,v ...")
--for k,v in pairs(ab) do
--    print(k, v)
--end

print("person size=", ab.person:Size())
local p = ab.person:Get(1)
print("p1", p)
print(pb.DebugString(p))
print("p1 name", p.name)
print("p1 id", p.id)
print("p1 email", p.email)
print("p1 phone size", p.phone:Size())
