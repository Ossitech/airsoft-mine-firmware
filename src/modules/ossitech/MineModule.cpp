#include "MineModule.h"

MineModule *mineModule;

bool MineModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded)
{
    meshtastic_MinePacket response;

    switch(decoded->messageType)
    {
        case meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER:
        if (m_triggered) {
            response.messageType = meshtastic_MinePacket_MessageType_MINE_MSG_TRIGGER_SUCCESS;
        }
        else
        {
            response.messageType = meshtastic_MinePacket_MessageType_MINE_MSG_ALREADY_TRIGGERED;
        }

        break;
        default:
        
        break;
    }

    return true;
}

int32_t MineModule::runOnce()
{
    return 1000; // Politely ask to re run this method in 1000ms.
}