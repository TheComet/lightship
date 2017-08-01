#include "mapconverter/MapConverter.h"

#include <stdio.h>

using namespace Urho3D;

void printHelp(const char* prog_name)
{
    printf("Usage: %s [options]\n", prog_name);
    printf("Converts OMG maps to XML format. Example:\n");
    printf("  %s path/to/map.omg output/location/\n", prog_name);
    printf("Also supports bulk conversions. Example:\n");
    printf("  %s map1.omg map2.omg map3.omg output/location/\n", prog_name);
    printf("  -h, --help                           = Show this help\n");
}

int main(int argc, char** argv)
{
    SharedPtr<Context> context(new Context);
    SharedPtr<MapConverter> app(new MapConverter(context));

    for(int i = 1; i != argc; ++i)
    {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }

        String name(argv[i]);
        if (name.EndsWith(".omg", false) == true)
            app->AddFile(argv[i]);
        else
            app->SetOutputLocation(argv[i]);
    }

    return app->Run();
}
