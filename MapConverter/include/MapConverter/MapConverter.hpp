#pragma once

#include <Urho3D/Engine/Application.h>

namespace LS {

class MapConverter : public Urho3D::Application
{
public:
    MapConverter(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

    void SetOutputLocation(const Urho3D::String& outLocation);
    void AddFile(const Urho3D::String& fileToConvert);

private:
    void ConvertSingle(const Urho3D::String& inFile) const;
    void Convert() const;
    Urho3D::String Join(const Urho3D::String& path1, const Urho3D::String& path2) const;
    Urho3D::String ReplaceExtension(const Urho3D::String& fileName, const Urho3D::String& newExtension) const;

private:
    Urho3D::StringVector filesToConvert_;
    Urho3D::String outLocation_;
};

}
