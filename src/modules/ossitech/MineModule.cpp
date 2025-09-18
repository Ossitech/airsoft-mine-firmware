#include "MineModule.h"
#include "MeshService.h"

MineModule *mineModule;

MineModule::MineModule()
    : ProtobufModule("mine", meshtastic_PortNum_PRIVATE_APP, &meshtastic_MinePacket_msg), concurrency::OSThread("MinePlugin")
{
    ourPortNum = meshtastic_PortNum_PRIVATE_APP;

    m_lastMotionState = false;
    m_triggered = false;

    m_Servo.setPeriodHertz(50);    // standard 50 hz servo
	m_Servo.attach(SERVO_PIN, 500, 2000);

    pinMode(MOTION_PIN, INPUT_PULLDOWN);
    pinMode(LED, OUTPUT);

    digitalWrite(LED, LOW);
}

// This handles a MinePacket protobuf that was parsed from an incoming message
// in the base class (ProtobufModule).
bool MineModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded)
{
    m_tsLastMessage = millis();

    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);

    switch(decoded->messageType)
    {
        case meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER:
            if (triggerMine()) {
                sendMineMessage(meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER_SUCCESS);
            }
            else
            {
                sendMineMessage(meshtastic_MinePacket_MessageType_MINE_MSG_ALREADY_TRIGGERED);
            }

            break;
        
        case meshtastic_MinePacket_MessageType_MINE_MSG_PING:
            sendMineMessage(meshtastic_MinePacket_MessageType_MINE_MSG_PONG);
            break;
        
        default:
            break;
    }

    return true;
}

// This method belongs to the base class OSThread
// and its execution will be scheduled using the return value
// of its previous execution as delay in milliseconds.
int32_t MineModule::runOnce()
{
    auto msSinceLastMsg = millis() - m_tsLastMessage;

    if (msSinceLastMsg > MSG_COOLDOWN_MS)
    {
        if (motionDetected())
        {
            sendMineMessage(meshtastic_MinePacket_MessageType_MINE_MSG_MOTION_DETECTED);

            return MOTION_COOLDOWN_MS; // Politely ask to re run this method in 5 seconds.
        }

        return 1000;
    }

    return MSG_COOLDOWN_MS - msSinceLastMsg;
}

bool MineModule::triggerMine()
{
    // Move servo to trigger Begadi M1 Mine.
    m_Servo.write(60);
    delay(300);
    m_Servo.write(0);

    if (m_triggered)
    {
        // Already triggered.
        return false;
    }

    m_triggered = true;
    
    return true;
}

bool MineModule::motionDetected()
{
    if (digitalRead(MOTION_PIN)) // 0 means activated -> connected to GND through sensor.
    {
        if (!m_lastMotionState)
        {
            m_lastMotionState = true;
            m_tsLastMessage = millis();

            digitalWrite(LED, HIGH);
            delay(200);
            digitalWrite(LED, LOW);
            return true;
        }
    }
    else
    {
        if (m_lastMotionState)
        {
            m_lastMotionState = false;
        }
    }

    return false;
}

void MineModule::drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    
}

bool MineModule::isRequestingFocus()
{
    return false;
}

void MineModule::sendMineMessage(meshtastic_MinePacket_MessageType msgType)
{
    meshtastic_MinePacket msg;
    msg.messageType = msgType;

    auto meshPacket = allocDataProtobuf(msg);

    service->sendToMesh(meshPacket);
}