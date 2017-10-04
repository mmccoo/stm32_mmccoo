

#ifndef APP_HH
#define APP_HH

#include <stdint.h>

#ifdef __cplusplus 
extern "C" {
#endif
  void main_loop();
  void app_init();

  typedef enum {
    DSUsb,
    DSUart,
    DSCc1101
  } DataSource;
  void SendData(uint8_t *data, uint32_t length, DataSource source);

#ifdef __cplusplus 
}
#endif

#endif
