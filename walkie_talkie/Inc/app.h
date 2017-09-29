

#ifndef APP_HH
#define APP_HH

#include <stdint.h>

#ifdef __cplusplus 
extern "C" {
#endif
  void main_loop();
  void app_init();
  void SendUART(uint8_t *data, uint32_t length);

#ifdef __cplusplus 
}
#endif

#endif
