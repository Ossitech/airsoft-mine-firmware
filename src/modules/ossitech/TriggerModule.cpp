#include "TriggerModule.h"
#include "MeshService.h"
#include "pitches.h"

TriggerModule *triggerModule;

TriggerModule::TriggerModule()
    : ProtobufModule("mine", meshtastic_PortNum_PRIVATE_APP, &meshtastic_MinePacket_msg), concurrency::OSThread("MinePlugin")
{
    ourPortNum = meshtastic_PortNum_PRIVATE_APP;

    m_lastSwitchState = false;

    m_cycle = 0;

    pinMode(SWITCH_PIN, INPUT_PULLDOWN);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);
    pinMode(SWITCH_LED_PIN, OUTPUT);
    digitalWrite(SWITCH_LED_PIN, HIGH);
}

// This handles a MinePacket protobuf that was parsed from an incoming message
// in the base class (ProtobufModule).
bool TriggerModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded)
{
    static const Tone melodyMotion[] = {
        { NOTE_C5, 200, 100 },
        { NOTE_C5, 200, 100 },
        { NOTE_C5, 200, 100 }
    };

    static const Tone melodySuccess[] = {
        { NOTE_C5, 200, 100 },
        { NOTE_D5, 200, 100 },
        { NOTE_E5, 200, 100 }
    };

    static const Tone melodyAlreadyTriggered[] = {
        { NOTE_E5, 200, 100 },
        { NOTE_D5, 200, 100 },
        { NOTE_C5, 200, 100 }
    };

    static const Tone melodyError[] = {
        { NOTE_C4, 500, 100 },
        { NOTE_D4, 500, 100 },
        { NOTE_C4, 500, 100 }
    };

    m_tsLastEvent = millis();
    
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);

    switch (decoded->messageType)
    {
        case meshtastic_MinePacket_MessageType_MINE_MSG_MOTION_DETECTED:
        playMelody(melodyMotion, 3);
        break;

        case meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER_SUCCESS:
        playMelody(melodySuccess, 3);
        break;

        case meshtastic_MinePacket_MessageType_MINE_MSG_ALREADY_TRIGGERED:
        playMelody(melodyAlreadyTriggered, 3);
        break;

        case meshtastic_MinePacket_MessageType_MINE_MSG_PONG:
        // Turn on red switch led. LOW -> ON, HIGH -> OFF.
        digitalWrite(SWITCH_LED_PIN, LOW);
        break;

        case meshtastic_MinePacket_MessageType_MINE_MSG_ERROR:
        default:
        playMelody(melodyError, 3);
        break;
    }

    return true;
}

// This method belongs to the base class OSThread
// and its execution will be scheduled using the return value
// of its previous execution as delay in milliseconds.
int32_t TriggerModule::runOnce()
{
    if (digitalRead(SWITCH_PIN))
    {
        if (!m_lastSwitchState)
        {
            // Switch pressed
            m_lastSwitchState = true;

            sendMineMessage(meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER);

            m_tsLastEvent = millis();

            digitalWrite(LED, HIGH);
            delay(200);
            digitalWrite(LED, LOW);
        }
    }
    else
    {
        if (m_lastSwitchState)
        {
            m_lastSwitchState = false;
        }
    }

    if (m_cycle++ >= CYCLES_BETWEEN_PINGS)
    {
        m_cycle = 0;

        // send ping
        sendMineMessage(meshtastic_MinePacket_MessageType_MINE_MSG_PING);
        // Turn off red switch led until PONG is received.
        digitalWrite(SWITCH_LED_PIN, HIGH);
    }

    return 1000; // Politely ask to re run this method in 1 second.
}

void TriggerModule::playMelody(const Tone *melody, uint32_t length)
{
    for (int i = 0; i < length; i++)
    {
        const Tone &note = melody[i];
        tone(BUZZER_PIN, note.freq, note.toneDuration);
        delay(note.toneDuration);
        noTone(BUZZER_PIN);
        delay(note.pauseDuration);
    }
}

void TriggerModule::drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    display->clear();
    display->drawCircle(100, 100, 100);
    display->flush();
}

bool TriggerModule::isRequestingFocus()
{
    auto now = millis();

    return now - m_tsLastEvent < 5000;
}

void TriggerModule::sendMineMessage(meshtastic_MinePacket_MessageType msgType)
{
    meshtastic_MinePacket msg;
    msg.messageType = msgType;

    auto meshPacket = allocDataProtobuf(msg);

    service->sendToMesh(meshPacket);
}