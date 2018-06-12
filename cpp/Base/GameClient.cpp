#include "GameClient.h"
#include <iostream>

using namespace Base;

void GameClient::ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len)
{
    std::string msg((char*)data, len);
    std::cout << __FUNCTION__ << " msg=" << msg << std::endl;
}
void GameClient::OnConnect(const ConnectionPtr conn)
{
    std::cout << __FUNCTION__ << " id=" << conn->GetId() << std::endl;
}
void GameClient::OnDisconnect(const ConnectionPtr conn)
{
    std::cout << __FUNCTION__ << " id=" << conn->GetId() << std::endl;
}

static GameClient* s_client = nullptr;
static int _new_client(lua_State* L)
{
    if (!s_client)
    {
        s_client = new GameClient;
        s_client->L = L;
    }
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
    return 1;
}
static int _delete_client(lua_State* L)
{
    if (!s_client)
    {
        std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
        delete s_client;
        s_client = nullptr;
    }
    return 1;
}
static int _send_data(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    size_t len;
    const char *str = static_cast<const char *>(luaL_checklstring(L, 1, &len));
    std::string data(str, len);
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << ", data=" << data << std::endl;
    s_client->SendPacket(data);
    return 1;
}
static int _connect(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    std::string host = luaL_checkstring(L, 1);
    std::string port = luaL_checkstring(L, 2);
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << ", server=" << host << ":" << port << std::endl;
    s_client->Connect(host, port);
    return 1;
}
static int _disconnect(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
    s_client->Disconnect();
    return 1;
}
static int _stop(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
    s_client->Stop();
    return 1;
}
static int _async_run(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
    s_client->AsyncRun();
    return 1;
}
static int _sync_run(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
    s_client->SyncRun();
    return 1;
}
static int _set_callbacks(lua_State* L)
{
    if (!s_client)
    {
        return 0;
    }
    std::cout << __FUNCTION__ << " s_client=0x" << (void*)s_client << std::endl;
    return 1;
}
int luaopen_client(lua_State* L)
{
    luaL_Reg reg[] = {
        {"new" , _new_client },
        {"delete" , _delete_client },
        {"send", _send_data },
        {"connect", _connect },
        {"disconnect", _disconnect },
        {"stop", _stop },
        {"async_run", _async_run },
        {"sync_run", _sync_run },
        {"set_callbacks", _set_callbacks },
        {NULL,NULL},
    };
    _new_client(L);
    luaL_register(L,"client",reg);
    return 1;
}
