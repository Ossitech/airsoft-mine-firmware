#pragma once

#include "ProtobufModule.h"
#include "meshtastic/mine.pb.h"

#define OSSITECH_MINE_PORT 458

#define SWITCH_PIN 47
#define BUZZER_PIN 48

struct Tone
{
    uint32_t freq;
    uint32_t toneDuration;
    uint32_t pauseDuration;
};

class TriggerModule : public ProtobufModule<meshtastic_MinePacket>, private concurrency::OSThread
{
    public:
    TriggerModule();

    protected:
    bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded) override;

    int32_t runOnce() override;

    private:
    bool m_lastSwitchState;

    bool switchPressed();

    void playMelody(const Tone *melody, uint32_t length);
};

extern TriggerModule *triggerModule;