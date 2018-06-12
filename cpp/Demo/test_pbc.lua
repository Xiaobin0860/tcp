
print("==========================")
local protobuf = require "protobuf"
print("protobuf", protobuf)
addr = io.open("addressbook.pb","rb")
print("read")
buffer = addr:read "*a"
print("close")
addr:close()
print("register")
protobuf.register(buffer)
print("decode")
t2 = protobuf.decode("google.protobuf.FileDescriptorSet", buffer)

local person = {
	name = "Alice",
	id = 123,
	phone = {
		{ number = "123456789" , type = "MOBILE" },
		{ number = "87654321" , type = "HOME" },
	}
}

local buffer = protobuf.encode("tutorial.Person", person)

local t = protobuf.decode("tutorial.Person", buffer)

for k,v in pairs(t) do
	if type(k) == "string" then
		print(k,v)
	end
end

print(t.phone[2].type)

for k,v in pairs(t.phone[1]) do
	print(k,v)
end

print("==> finish test1")


proto = t2.file[1]

print(proto.name)
print(proto.package)

message = proto.message_type

for _,v in ipairs(message) do
	print(v.name)
	for _,v in ipairs(v.field) do
		print("\t".. v.name .. " ["..v.number.."] " .. v.label)
	end
end

person = {
	name = "Alice",
	id = 12345,
	phone = {
		{ number = "1301234567" },
		{ number = "87654321", type = "WORK" },
	}
}

code = protobuf.encode("tutorial.Person", person)

decode = protobuf.decode("tutorial.Person" , code)

print(decode.name)
print(decode.id)
for _,v in ipairs(decode.phone) do
	print("\t"..v.number, v.type)
end

phonebuf = protobuf.pack("tutorial.Person.PhoneNumber number","87654321")
buffer = protobuf.pack("tutorial.Person name id phone", "Alice", 123, { phonebuf })
print(protobuf.unpack("tutorial.Person name id phone", buffer))
print("==> finish test2")

ab = {
    {
        name = "xiaohong",
        id = 1,
        email = "lilei@gmail.com",
        phone = {
            { number = "18612345678", type = "HOME" },
            { number = "13812345678", type = "WORK" },
        }
    },
    --{
    --    name = "Han Meimei",
    --    id = 2,
    --    email = "hanmeimei@gmail.com",
    --    phone = {
    --        { number = "11111111111" },
    --        { number = "22222222222" },
    --    }
    --}
}
code = protobuf.encode("tutorial.AddressBook", ab)
print("ab code size=", #code)

print(g_hello)
print("g_s size: ", #g_s)
--local ab = protobuf.decode("tutorial::AddressBook", g_s)
--for k,v in pairs(t) do
--	if type(k) == "string" then
--		print(k,v)
--	end
--end
-- local person= person_pb.Person()
-- person.id = 1000
-- person.name = "Alice"
-- person.email = "Alice@example.com"

-- local home = person.Extensions[person_pb.Phone.phones]:add()
-- home.num = "2147483647"
-- home.type = person_pb.Phone.HOME

-- local data = person:SerializeToString()

-- local msg = person_pb.Person()

-- msg:ParseFromString(data)

-- local ab = addressbook_pb.AddressBook()
-- local p = ab[addressbook_pb.Person.people]:add()
-- p.name = "李"
-- p.id = 1
print("Hello World")
-- print("ab", ab)
-- local ab2 = addressbook_pb.AddressBook()
-- print(ab2, type(ab2), ab2.ParseFromString)
-- ab2:ParseFromString(g_s)
-- print("ab2", ab2)
print("ok")
