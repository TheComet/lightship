#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void signals_register(void);
int signals_exit_requested(void);

#ifdef __cplusplus
}
#endif
