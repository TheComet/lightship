#include <Urho3D/Core/Object.h>

/*!
 * All messages used to synchronize the map state use this ID.
 */
static const int MSG_MAPSTATE = 0x17;

enum ChatNetwork
{
    CHAT_REQUEST_CONNECTED_USERS_LIST,
    CHAT_SEND_MESSAGE
};
static const int MSG_CHAT = 0x18;

static const int MSG_USERMANAGER = 0x19;
