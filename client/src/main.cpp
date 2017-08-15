#include "lightship-client/ClientApplication.h"
#include <Urho3D/Engine/Application.h>

#include <stdio.h>

using namespace Urho3D;

void printHelp(const char* prog_name)
{
    printf("Usage: %s [options]", prog_name);
    printf("  -h, --help                           = Show this help");
}

int main(int argc, char** argv)
{
    for(int i = 0; i != argc; ++i)
    {
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }
    }

    SharedPtr<Context> context(new Context);
    SharedPtr<Application> app(new ClientApplication(context));
    return app->Run();
}
