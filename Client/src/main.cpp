#include "LightshipClient/ClientApplication.h"

#include <stdio.h>

using namespace Urho3D;

void printHelp(const char* prog_name)
{
    printf("Usage: %s [options]", prog_name);
    printf("  -h, --help                           = Show this help");
}

#ifdef LIGHTSHIP_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
#else
int main(int argc, char** argv)
#endif
{
#ifndef LIGHTSHIP_PLATFORM_WINDOWS
    for(int i = 0; i != argc; ++i)
    {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }
    }
#endif

    SharedPtr<Context> context(new Context);
    SharedPtr<Application> app(new ClientApplication(context));
    return app->Run();
}
