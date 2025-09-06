#pragma once

#include "ProtobufModule.h"

#define OSSITECH_MINE_PORT 458

template <class T> class MineModule : protected ProtobufModule
{
    public:
    MineModule() : ProtobufModule("MineModule", OSSITECH_MINE_PORT, nullptr)

    protected:
    bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, T *decoded) override
    {

    }
};