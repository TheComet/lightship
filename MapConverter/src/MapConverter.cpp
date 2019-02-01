#include "MapConverter/MapConverter.hpp"
#include "Lightship/MapState.hpp"

#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Resource/XMLFile.h>

using namespace Urho3D;

namespace LS {

// ----------------------------------------------------------------------------
MapConverter::MapConverter(Context* context) :
    Application(context),
    outLocation_(".")
{
}

// ----------------------------------------------------------------------------
void MapConverter::Setup()
{
    engineParameters_["Headless"] = true;
}

// ----------------------------------------------------------------------------
bool MapConverter::ProcessCommandLine()
{
    for (Vector<String>::ConstIterator it = GetArguments().Begin(); it != GetArguments().End(); ++it)
    {
        // Is it a switch?
        if (it->StartsWith("-"))
        {
            // Request for help?
            if (it->Compare("-h") == 0 || it->Compare("--help") == 0)
            {
                const char* cmd = "./lightship-mapconverter";
                fprintf(stderr, "Usage: %s [options]\n", cmd);
                fprintf(stderr, "Converts OMG maps to XML format. Example:\n");
                fprintf(stderr, "  %s path/to/map.omg output/location/\n", cmd);
                fprintf(stderr, "Also supports bulk conversions. Example:\n");
                fprintf(stderr, "  %s map1.omg map2.omg map3.omg output/location/\n", cmd);
                fprintf(stderr, "  -h, --help                           = Show this help\n");
                return false;
            }
            else
            {
                fprintf(stderr, "Unknown option %s\n", it->CString());
                return false;
            }

            continue;
        }

        // Assume it's a map file
        if (it->EndsWith(".omg", false))
            AddFile(*it);
        else
            SetOutputLocation(*it);
    }

    return true;
}

// ----------------------------------------------------------------------------
void MapConverter::Start()
{
    if (ProcessCommandLine())
        Convert();
    engine_->Exit();
}

// ----------------------------------------------------------------------------
void MapConverter::Stop()
{
    URHO3D_LOGINFOF("Converted files were written to %s", outLocation_.CString());
}

// ----------------------------------------------------------------------------
void MapConverter::SetOutputLocation(const Urho3D::String& outLocation)
{
    outLocation_ = outLocation;
}

// ----------------------------------------------------------------------------
void MapConverter::AddFile(const Urho3D::String& fileToConvert)
{
    filesToConvert_.Push(fileToConvert);
}

// ----------------------------------------------------------------------------
void MapConverter::ConvertSingle(const String& inFile) const
{
    URHO3D_LOGINFOF("Converting file %s...", inFile.CString());

    String outFile = inFile.Split('/').Back();
    outFile = outFile.Split('\\').Back();
    outFile = ReplaceExtension(outFile, "xml");
    outFile = Join(outLocation_, outFile);

    MapState map(context_);
    if (map.LoadOMG(inFile) == false)
        return;

    XMLFile mapXML(context_);
    XMLElement root = mapXML.CreateRoot("root");
    map.SaveXML(root);

    File mapFile(context_);
    mapFile.Open(outFile, FILE_WRITE);
    mapXML.Save(mapFile);
}

// ----------------------------------------------------------------------------
void MapConverter::Convert() const
{
    for (StringVector::ConstIterator it = filesToConvert_.Begin(); it != filesToConvert_.End(); ++it)
        ConvertSingle(*it);
}

// ----------------------------------------------------------------------------
String MapConverter::Join(const String& path1, const String& path2) const
{
    if (path1.EndsWith("/") || path1.EndsWith("\\"))
        return path1 + path2;
    return path1 + "/" + path2;
}

// ----------------------------------------------------------------------------
String MapConverter::ReplaceExtension(const String& fileName, const String& newExtension) const
{
    unsigned pos;
    if ((pos = fileName.FindLast('.')) != String::NPOS)
        return fileName.Substring(0, pos) + "." + newExtension;
    return fileName + "." + newExtension;
}

}

URHO3D_DEFINE_APPLICATION_MAIN(LS::MapConverter)
