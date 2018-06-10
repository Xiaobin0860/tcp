#include "lua.hpp"
#include <zlib.h>
#include <limits>
#include <iostream>
#include "gtest/gtest.h"
#include "addressbook.pb.h"


TEST(PB, pb)
{
    tutorial::AddressBook ab;
    ASSERT_TRUE(ab.people_size() == 0);
    auto p = ab.add_people();
    ASSERT_TRUE(p->name() == "");
    p->set_name("xiaoming");
    ASSERT_TRUE(p->name() == "xiaoming");
    ASSERT_TRUE(p->id() == 0);
    p->set_id(1);
    ASSERT_TRUE(p->id() == 1);
    p->set_email("lilei@gmail.com");
    ASSERT_TRUE(p->email() == "lilei@gmail.com");
    auto phone = p->add_phones();
    phone->set_type(tutorial::Person_PhoneType_HOME);
    phone->set_number("18612345678");
    phone = p->add_phones();
    phone->set_type(tutorial::Person_PhoneType_WORK);
    phone->set_number("13812345678");
    auto up = p->mutable_last_updated();
    up->set_seconds(UINT_MAX + 1ull);
    up->set_nanos(UINT_MAX + 1ull);
    auto p2 = ab.add_people();
    p2->CopyFrom(*p);
    p2->set_name("Han Meimei");
    ASSERT_TRUE(p2->id() == 1);
    p2->set_id(2);
    ASSERT_TRUE(p2->id() == 2);
    p2->set_email("hanmeimei@gmail.com");
    ASSERT_TRUE(p2->phones_size() == 2);
    p2->mutable_phones(0)->set_number("11111111111");
    p2->mutable_phones(1)->set_number("22222222222");
    p2->mutable_last_updated()->set_nanos(-10);
    p2->add_test_double(1.0001);
    p2->add_test_double(2.2222);
    p2->add_test_float(1.0);
    p2->add_test_float(22222.2);
    p2->add_test_uint64(UINT_MAX + 1ull);
    p2->add_test_uint64(UINT_MAX + 2ull);

    auto s = ab.SerializeAsString();
    tutorial::AddressBook ab2;
    ab2.ParseFromString(s);
    std::cout << "ab: " << ab.DebugString() << std::endl;
    std::cout << "ab2: " << ab2.DebugString() << std::endl;
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
    
    unsigned char buf[306] = {0};
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
        for(int i=0;i<len;++i)
        {
            if(i<=0xFF)
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

int main(int argc, char* argv[])
{
    return dec_xs();
    
    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();


    tutorial::AddressBook ab;
    auto p = ab.add_people();
    p->set_name("xiaohong");
    p->set_id(1);
    p->set_email("lilei@gmail.com");
    auto phone = p->add_phones();
    phone->set_type(tutorial::Person_PhoneType_HOME);
    phone->set_number("18612345678");
    phone = p->add_phones();
    phone->set_type(tutorial::Person_PhoneType_WORK);
    phone->set_number("13812345678");
    auto up = p->mutable_last_updated();
    up->set_seconds(UINT_MAX + 1ull);
    up->set_nanos(UINT_MAX + 1ull);
    auto p2 = ab.add_people();
    p2->CopyFrom(*p);
    p2->set_name("Han Meimei");
    p2->set_id(2);
    p2->set_email("hanmeimei@gmail.com");
    p2->mutable_phones(0)->set_number("11111111111");
    p2->mutable_phones(1)->set_number("22222222222");
    p2->mutable_last_updated()->set_nanos(-10);
    p2->add_test_double(1.0001);
    p2->add_test_double(2.2222);
    p2->add_test_float(1.0);
    p2->add_test_float(22222.2);
    p2->add_test_uint64(UINT_MAX + 1ull);
    p2->add_test_uint64(UINT_MAX + 2ull);

    auto s = ab.SerializeAsString();

    auto L = lua_open();
    luaL_openlibs(L);
    luaopen_pb(L);
    lua_pushlstring(L, s.c_str(), s.size());
    lua_setglobal(L, "g_s");
    lua_pushstring(L, "hello c++!");
    lua_setglobal(L, "g_hello");
    luaL_dofile(L, "test.lua");
    lua_close(L);

#if WIN32
    getchar();
#endif

    return ret;
}
