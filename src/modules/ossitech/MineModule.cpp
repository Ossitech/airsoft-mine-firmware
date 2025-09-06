#include "MineModule.h"

bool MineModule::handleReceivedProtobuf(const meshtastic_MeshPacket &mp, meshtastic_MinePacket *decoded)
{
    return true;
}

int32_t MineModule::runOnce()
{
    return 1000;
}