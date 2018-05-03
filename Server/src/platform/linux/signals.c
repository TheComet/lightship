#include "LightshipServer/signals.h"
#include <signal.h>

static volatile int g_exit_requested = 0;

// ----------------------------------------------------------------------------
static void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        g_exit_requested = 1;
    }
}

// ----------------------------------------------------------------------------
void signals_register(void)
{
    signal(SIGINT, sig_handler);
}

// ----------------------------------------------------------------------------
int signals_exit_requested(void)
{
    return g_exit_requested;
}
