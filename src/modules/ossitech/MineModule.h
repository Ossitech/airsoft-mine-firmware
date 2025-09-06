#pragma once

#include "ProtobufModule.h"
#include "meshtastic/mine.pb.h"
#include <ESP32Servo.h>

#define OSSITECH_MINE_PORT 458

class MineModule : public ProtobufModule<meshtastic_MinePacket>, private concurrency::OSThread
{
    public:
    MineModule();

    protected:
    bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded) override;

    int32_t runOnce() override;

    private:
    Servo m_Servo;
};