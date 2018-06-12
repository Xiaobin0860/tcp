#include "lua.hpp"
#include <zlib.h>
#include <limits>
#include <iostream>
#include "gtest/gtest.h"
#include "addressbook.pb.h"
#include "demo_types.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "../Base/GeneralClient.h"
#include "LuaIntf/LuaIntf.h"
//#include "LuaBridge/LuaBridge.h"

using MemBuf = apache::thrift::transport::TMemoryBuffer;
using BinProto = apache::thrift::protocol::TBinaryProtocol;


using namespace Base;
class GameClient : public GeneralClient
{
public:
    GameClient() : GeneralClient() {
        Connect("127.0.0.1", "11112");
    }
    ~GameClient()
    {
    }
    void SendPacket(const std::string& data)
    {
        GeneralClient::SendPacket(data);
    }
    void SyncRun()
    {
        GeneralClient::SyncRun();
    }
    void AsyncRun()
    {
        GeneralClient::AsyncRun();
    }
    void Stop()
    {
        GeneralClient::Stop();
    }

protected:
    virtual void ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len) override
    {
        std::string msg((char*)data, len);
        std::cout << __FUNCTION__ << " msg=" << msg << std::endl;
        ++_msg_count;
        if (_msg_count > 10)
        {
            std::string close("Close");
            SendPacket(close);
            _hive->Post([this] {
                Stop();
            });
        }
        else
        {
            std::string hello("Hello");
            SendPacket(hello);
        }
    }
    virtual void OnConnect(const ConnectionPtr conn) override
    {
        //std::string msg("Hello");
        //SendPacket(msg);
    }
    virtual void OnDisconnect(const ConnectionPtr conn) override
    {
    }

private:
    int _msg_count = 0;
};


TEST(PB, pb)
{
    //tutorial::AddressBook ab;
    //ASSERT_TRUE(ab.people_size() == 0);
    //auto p = ab.add_people();
    //ASSERT_TRUE(p->name() == "");
    //p->set_name("xiaoming");
    //ASSERT_TRUE(p->name() == "xiaoming");
    //ASSERT_TRUE(p->id() == 0);
    //p->set_id(1);
    //ASSERT_TRUE(p->id() == 1);
    //p->set_email("lilei@gmail.com");
    //ASSERT_TRUE(p->email() == "lilei@gmail.com");
    //auto phone = p->add_phones();
    //phone->set_type(tutorial::Person_PhoneType_HOME);
    //phone->set_number("18612345678");
    //phone = p->add_phones();
    //phone->set_type(tutorial::Person_PhoneType_WORK);
    //phone->set_number("13812345678");
    //auto up = p->mutable_last_updated();
    //up->set_seconds(UINT_MAX + 1ull);
    //up->set_nanos(UINT_MAX + 1ull);
    //auto p2 = ab.add_people();
    //p2->CopyFrom(*p);
    //p2->set_name("Han Meimei");
    //ASSERT_TRUE(p2->id() == 1);
    //p2->set_id(2);
    //ASSERT_TRUE(p2->id() == 2);
    //p2->set_email("hanmeimei@gmail.com");
    //ASSERT_TRUE(p2->phones_size() == 2);
    //p2->mutable_phones(0)->set_number("11111111111");
    //p2->mutable_phones(1)->set_number("22222222222");
    //p2->mutable_last_updated()->set_nanos(-10);
    //p2->add_test_double(1.0001);
    //p2->add_test_double(2.2222);
    //p2->add_test_float(1.0);
    //p2->add_test_float(22222.2);
    //p2->add_test_uint64(UINT_MAX + 1ull);
    //p2->add_test_uint64(UINT_MAX + 2ull);

    //auto s = ab.SerializeAsString();
    //tutorial::AddressBook ab2;
    //ab2.ParseFromString(s);
    //std::cout << "ab: " << ab.DebugString() << std::endl;
    //std::cout << "ab2: " << ab2.DebugString() << std::endl;
}


TEST(THRIFT, thrift)
{
    std::vector<Demo::Work> works;
    for (int i = 0; i < 4; ++i)
    {
        Demo::Work work;
        work.key = std::to_string(i);
        work.comment = "comment" + work.key;
        work.type = Demo::Type::MULTIPLY;
        work.num1 = INT_MAX;
        work.num2 = UINT_MAX + 1;
        works.push_back(work);
    }
    Demo::Worker worker;
    worker.__set_works(works);
    worker.result = 3.1415926;

    Demo::Work work;
    work.key = "key";
    work.comment = "comment" + work.key;
    work.type = Demo::Type::DIVIDE;
    work.num1 = INT_MAX;
    work.num2 = UINT_MAX + 1;
    worker.work_map[work.key] = work;
    auto mt = std::make_shared<MemBuf>(4);
    BinProto bp(mt);
    worker.write(&bp);
    std::ostringstream os;
    os << worker;
    std::cout << "worker: " << os.str() << std::endl;
    auto s = mt->getBufferAsString();
    std::cout << "worker string size: " << s.size() << std::endl;
    {
        auto mt = std::make_shared<MemBuf>((uint8_t*)s.c_str(), s.size());
        BinProto bp(mt);
        Demo::Worker worker;
        worker.read(&bp);
        std::ostringstream os;
        os << worker;
        std::cout << "worker: " << os.str() << std::endl;
    }
}

void unzip()
{
    FILE* fin = fopen("cepl_zip", "rb");
    if (!fin) return;
    fseek(fin, 0, SEEK_END);
    int zip_len = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    unsigned char* zip_buf = (unsigned char*)malloc(zip_len);
    fread(zip_buf, 1, zip_len, fin);
    fclose(fin);

    unsigned char buf[306] = { 0 };
    unsigned long len = 306;
    int zip_result = uncompress(buf, &len, zip_buf, zip_len);
    if (zip_result != Z_OK)
    {
        std::cerr << "uncompress error: " << zip_result << std::endl;
    }
    else
    {
        auto fout = fopen("cepl_unzip", "wb");
        if (fout)
        {
            fwrite(buf, 1, len, fout);
            fclose(fout);
            std::cout << "cepl_unzip len=" << len << std::endl;
        }
    }
    free(zip_buf);
}
int dec_xs()
{
    unzip();

    FILE* fin = fopen("a", "rb");
    if (!fin) return 1;
    int result = 1;
    FILE* fout = nullptr;
    do {
        fseek(fin, 6, SEEK_SET);
        int len = 0;
        int size = fread(&len, 4, 1, fin);
        if (size != 1) break;
        len -= 0x0C;
        std::cout << "content len=" << len << std::endl;
        if (len <= 0) break;
        fseek(fin, 8, SEEK_CUR);
        fout = fopen("b", "wb");
        if (!fout) break;
        for (int i = 0; i < len; ++i)
        {
            if (i <= 0xFF)
            {
                fseek(fin, 1, SEEK_CUR);
            }
            fputc(fgetc(fin), fout);
        }

        result = 0;
    } while (0);
    fclose(fin);
    if (fout) fclose(fout);
    return result;
}

//extern int luaopen_pb(lua_State *L);
//extern int luaopen_pb_io(lua_State *L);
//extern int luaopen_pb_conv(lua_State *L);
//extern int luaopen_pb_buffer(lua_State *L);
//extern int luaopen_pb_slice(lua_State *L);
//extern int luaopen_protobuf_pb(lua_State *L);
//void init_lua_pb(lua_State *L)
//{
//    //luaopen_pb(L);
//    //luaopen_pb_io(L);
//    //luaopen_pb_conv(L);
//    //luaopen_pb_buffer(L);
//    //luaopen_pb_slice(L);
//}
//extern int luaopen_upb_c(lua_State *L);
extern "C" int luaopen_protobuf_c(lua_State *L);
int main(int argc, char* argv[])
{
    //return dec_xs();

    ::testing::InitGoogleTest(&argc, argv);

    int ret = 0;// RUN_ALL_TESTS();


    tutorial::AddressBook ab;
    auto p = ab.add_person();
    p->set_name("xiaohong");
    p->set_id(1);
    p->set_email("lilei@gmail.com");
    auto phone = p->add_phone();
    phone->set_type(tutorial::Person_PhoneType_HOME);
    phone->set_number("18612345678");
    phone = p->add_phone();
    phone->set_type(tutorial::Person_PhoneType_WORK);
    phone->set_number("13812345678");
    auto p2 = ab.add_person();
    p2->CopyFrom(*p);
    p2->set_name("Han Meimei");
    p2->set_id(2);
    p2->set_email("hanmeimei@gmail.com");
    p2->mutable_phone(0)->set_number("11111111111");
    p2->mutable_phone(1)->set_number("22222222222");
    //p2->add_test_double(1.0001);
    //p2->add_test_double(2.2222);
    //p2->add_test_float(1.0);
    //p2->add_test_float(22222.2);
    //p2->add_test_uint64(UINT_MAX + 1ull);
    //p2->add_test_uint64(UINT_MAX + 2ull);

    auto s = ab.SerializeAsString();
    std::cout << "s size=" << s.size() << std::endl;
    tutorial::AddressBook ab2;
    ab2.ParseFromString(s);
    std::cout << "ab: " << ab.DebugString() << std::endl;
    std::cout << "ab2: " << ab2.DebugString() << std::endl;
    std::cout << "s size=" << s.size() << std::endl;

    auto L = lua_open();
    luaL_openlibs(L);
    //init_lua_pb(L);
    //luaopen_upb_c(L);

    //LuaIntf::LuaBinding(L)
    //    .beginClass<GameClient>("GameClient")
    //    .addConstructor(LUA_ARGS())
    //    .addFunction("SendPacket", &GameClient::SendPacket)
    //    .addFunction("SyncRun", &GameClient::SyncRun)
    //    .addFunction("AsyncRun", &GameClient::AsyncRun)
    //    .addFunction("Stop", &GameClient::Stop)
    //    .addStaticFunction("test", [] { return "yes";})
    //    .endClass();

    //luaopen_protobuf_pb(L);
    luaopen_protobuf_c(L);
    lua_pushlstring(L, s.c_str(), s.size());
    lua_setglobal(L, "g_s");
    lua_pushstring(L, "hello c++!");
    lua_setglobal(L, "g_hello");
    luaL_dofile(L, "test_pbc.lua");
    //luaL_dofile(L, "test_socket.lua");
    //luaL_dofile(L, "test2.lua");
    lua_close(L);
    //std::cout << "ok s size=" << s.size() << std::endl;
#if WIN32
    getchar();
#endif

    return ret;
}
