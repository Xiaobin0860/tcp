#pragma once

#include "GeneralClient.h"
#include "lua.hpp"

namespace Base
{
    class GameClient : public GeneralClient
    {
    public:
        lua_State* L = nullptr;
    protected:
        virtual void ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len) override;
        virtual void OnConnect(const ConnectionPtr conn) override;
        virtual void OnDisconnect(const ConnectionPtr conn) override;
    };
}

extern "C" int luaopen_client(lua_State* L);
