#include <Urho3D/Core/Object.h>

/*!
 * All messages used to synchronize the map state use this ID.
 */

static const int MSG_CHAT_MESSAGE = 0x17;
static const int MSG_CHAT_REQUEST_HISTORY = 0x18;

static const int MSG_USER_ADDED = 0x19;
static const int MSG_USER_REMOVED = 0x20;
static const int MSG_USER_REQUEST_LIST = 0x21;

static const int MSG_MAP_STATE = 0x22;
