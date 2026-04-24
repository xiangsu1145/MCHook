#pragma once

class ClientInstance;

class ClientInstanceHook
{
public:
    typedef void(__fastcall* update_t)(ClientInstance* thiz, unsigned a2);
    static inline update_t originalUpdate = nullptr;
    static void __fastcall hookUpdate(ClientInstance* thiz,unsigned a2);

    static void init();
};

