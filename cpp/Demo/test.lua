package.path = package.path .. ';./pb/?.lua'
-- package.cpath = package.cpath .. ';../protobuf/?.so'
require 'pb.protobuf'

require 'addressbook_pb'

print(g_hello)
print("g_s size: ", #g_s)
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
local ab2 = addressbook_pb.AddressBook()
print(ab2, type(ab2), ab2.ParseFromString)
ab2:ParseFromString(g_s)
print("ab2", ab2)
