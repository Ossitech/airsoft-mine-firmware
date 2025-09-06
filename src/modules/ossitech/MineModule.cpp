#include "MineModule.h"
#include "MeshService.h"

MineModule *mineModule;

MineModule::MineModule()
    : ProtobufModule("mine", meshtastic_PortNum_PRIVATE_APP, &meshtastic_MinePacket_msg), concurrency::OSThread("MinePlugin")
{
    ourPortNum = meshtastic_PortNum_PRIVATE_APP;

    m_Servo.setPeriodHertz(50);    // standard 50 hz servo
	m_Servo.attach(SERVO_PIN, 500, 2000);

    pinMode(MOTION_PIN, INPUT_PULLDOWN);
}

bool MineModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded)
{
    meshtastic_MinePacket response;

    if(decoded->messageType == meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER)
    {
        if (triggerMine()) {
            response.messageType = meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER_SUCCESS;
        }
        else
        {
            response.messageType = meshtastic_MinePacket_MessageType_MINE_MSG_ALREADY_TRIGGERED;
        }
    }

    meshtastic_MeshPacket *meshPacket = allocDataProtobuf(response);

    service->sendToMesh(meshPacket);

    return true;
}

int32_t MineModule::runOnce()
{
    if (motionDetected())
    {
        meshtastic_MinePacket msg;
        msg.messageType = meshtastic_MinePacket_MessageType_MINE_MSG_MOTION_DETECTED;

        meshtastic_MeshPacket *meshPacket = allocDataProtobuf(msg);

        service->sendToMesh(meshPacket);

        return 5000; // Politely ask to re run this method in 5 seconds.
    }

    return 1000; // Politely ask to re run this method in 1 second.
}

bool MineModule::triggerMine()
{
    if (m_triggered)
    {
        // Already triggered.
        return false;
    }

    // Move servo to trigger Begadi M1 Mine.
    m_Servo.write(60);
    delay(300);
    m_Servo.write(0);

    m_triggered = true;
    
    return true;
}

bool MineModule::motionDetected()
{
    if (digitalRead(MOTION_PIN))
    {
        if (!m_lastMotionState)
        {
            m_lastMotionState = true;
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