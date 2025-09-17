#pragma once

#include "ProtobufModule.h"
#include "meshtastic/mine.pb.h"
#include <ESP32Servo.h>

#define OSSITECH_MINE_PORT 458

#define SERVO_PIN 7
#define MOTION_PIN 6

class MineModule : public ProtobufModule<meshtastic_MinePacket>, private concurrency::OSThread
{
    public:
    MineModule();

#if HAS_SCREEN
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
    
    bool isRequestingFocus() override;
#endif

    protected:
    bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded) override;

    int32_t runOnce() override;

    void sendMineMessage(meshtastic_MinePacket_MessageType msgType);

    private:
    Servo m_Servo;
    bool m_triggered;
    bool m_lastMotionState;
    unsigned long m_tsLastEvent;

    bool triggerMine();

    bool motionDetected();
};

extern MineModule *mineModule;