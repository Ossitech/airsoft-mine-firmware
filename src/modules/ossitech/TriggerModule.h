#pragma once

#include "ProtobufModule.h"
#include "meshtastic/mine.pb.h"

#define OSSITECH_MINE_PORT 458

#define SWITCH_PIN 7
#define SWITCH_LED_PIN 6
#define BUZZER_PIN 5

#define CYCLES_BETWEEN_PINGS 10

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

#if HAS_SCREEN
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
    
    bool isRequestingFocus() override;
#endif

    protected:
    bool handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded) override;

    int32_t runOnce() override;

    void sendMineMessage(meshtastic_MinePacket_MessageType msgType);

    private:
    bool m_lastSwitchState;
    unsigned long m_tsLastEvent;
    int m_cycle;

    bool switchPressed();

    void playMelody(const Tone *melody, uint32_t length);
};

extern TriggerModule *triggerModule;