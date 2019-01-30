#pragma once

#include <Urho3D/Core/StringUtils.h>

namespace LS {

class Chat
{
public:
    Chat();

    /// Returns the size of the buffer, which is the number of messages that are "remembered".
    int GetMessageLimit() const;
    /// Resizes the buffer, which is the number of messages that are "remembered". Existing messages are cleared.
    void SetMessageLimit(int limit);

    /*!
     * Returns a list of all messages that are in the buffer, sorted such that
     * the newest message is last in the list.
     */
    virtual Urho3D::StringVector GetMessages() const = 0;
    /// Returns the most recently added message
    virtual Urho3D::String GetNewestMessage() const = 0;
    /// Adds a message to the buffer. This will erase the oldest message if the buffer is full.
    virtual void AddMessage(const Urho3D::String& message, const Urho3D::Color& color=Urho3D::Color::WHITE) = 0;
    /// Removes all messages in the buffer.
    virtual void ClearMessages() = 0;

private:
    int limit_;
};

}
