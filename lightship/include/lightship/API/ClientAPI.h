#pragma once

#include "lightship/config.h"
#include <Urho3D/Core/StringUtils.h>

class MenuAPI;

class ClientAPI
{
public:
    virtual bool ConnectToServer(const Urho3D::String& address, unsigned int port) = 0;
    virtual void AbortConnectingToServer() = 0;
    virtual void DisconnectFromServer() = 0;
    virtual void Quit() = 0;

    virtual Urho3D::String GetUsername() const = 0;
    virtual void SetUsername(const Urho3D::String& username) = 0;
    virtual unsigned int GetGlobalID() const = 0;
    virtual void SetGlobalID(unsigned int ID) = 0;
    virtual char GetLocalID() const = 0;
    virtual void SetLocalID(char ID) = 0;

    virtual void CreateGame(const Urho3D::String& gameName) = 0;
    virtual void JoinGame(unsigned int gameID) = 0;
    virtual void LeaveGame() = 0;

    virtual void SendGlobalChatMessage(const Urho3D::String& message) = 0;
    virtual void SendLobbyChatMessage(const Urho3D::String& message) = 0;
    virtual void SendInGameChatMessage(const Urho3D::String& message) = 0;
};
