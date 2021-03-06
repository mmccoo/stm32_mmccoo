

#include "app.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#include "usbd_cdc_if.h"
#ifdef __cplusplus
 }
#endif
   
#include "ELECHOUSE_CC1101.h"

#include "string"
#include "cstring"

byte RX_buffer[11]={0};
uint8_t lighton = 0;
uint8_t state = 0;
int32_t count = 0;

// this queue is different from other queues in that it will give you
// as much queued data as it can without wrapping the internal circular
// buffer. If it gives you the data til the end of the buffer, the next
// call will give the stuff at the beginning.
struct Queue {
  uint8_t  tosend[128];
  uint32_t tosend_begin;
  uint32_t tosend_end;
  uint8_t  num_rounds;
  Queue() {
    tosend_begin = 0;
    tosend_end   = 0;
    num_rounds   = 0;
  };

  void enqueue(uint8_t *data, uint32_t length) {
    for(uint32_t i=0; i<length; i++) {
      tosend[tosend_end] = data[i];
      tosend_end++;
      if (tosend_end>=sizeof(tosend)) {
        tosend_end = 0;
      }
      tosend[tosend_end] = 0;
    }    
  }

  // returns begin/end pair.
  std::pair<uint8_t*, uint8_t*> dequeue() {
    // enqueue may move uart_tosend_end forward. Since it's called
    // from an interrupt, it needs to be treated a volatile.
    // uart_tosend_begin is also changed here.
    uint32_t end = tosend_end;
    if (tosend_begin == end) {
      return std::make_pair(tosend+tosend_begin, tosend+end);
    }
    if (tosend_begin < end) {
      // begin is before end. haven't wrapped.
      // need to capture the current value of tosend_begin before changing it.
      auto retval = std::make_pair(tosend+tosend_begin, tosend+end);
      tosend_begin = end;
      return retval;
    }

    // wrapping.
    auto retval = std::make_pair(tosend+tosend_begin, tosend+sizeof(tosend));
    tosend_begin = 0;
    return retval;
  }
};

Queue uart_tx_queue;
Queue uart_rx_queue;
Queue cc1101_tx_queue;
Queue cc1101_rx_queue;
Queue usb_tx_queue;


void UART_send_from_buffer(Queue &queue)
{

  HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
  if (state != HAL_UART_STATE_READY  &&
      state != HAL_UART_STATE_BUSY_RX) { return; }

  auto deq = queue.dequeue();
  if (deq.second == deq.first) { return; }

  HAL_UART_Transmit_DMA(&huart1, deq.first, deq.second-deq.first);

}

void CC1101_send_from_buffer(Queue &queue)
{
  auto deq = queue.dequeue();
  if (deq.second == deq.first) { return; }
  
  ELECHOUSE_cc1101.SendData(deq.first, deq.second-deq.first);
  ELECHOUSE_cc1101.SetReceive();
}

void Usb_send_from_buffer(Queue &queue)
{
  if (CDC_GetTxState()) { return; }

  auto deq = queue.dequeue();
  if (deq.second == deq.first) { return; }

  CDC_Transmit_FS(deq.first, deq.second-deq.first);
}

// from the terminal that I'm using, the data comes in two chunks
// the first is the data to be send.
// the second is a return and newline
void SendData(uint8_t *data, uint32_t length, DataSource source)
{
  if (source != DSUart) {
    uart_tx_queue.enqueue(data, length);
    // don't need to add the newline stuff if the stuff we're echoing is
    // already newlined.
    //uint8_t extra[] = "\n\r";
    // need the -1 because sizeof also needs the extra null termination.
    //uart_tx_queue.enqueue(extra, sizeof(extra)-1);
    
    UART_send_from_buffer(uart_tx_queue);
  }

  if (source != DSUsb) {
    usb_tx_queue.enqueue(data, length);
    Usb_send_from_buffer(usb_tx_queue);
  }

  if (source != DSCc1101) {
    cc1101_tx_queue.enqueue(data, length);
    CC1101_send_from_buffer(cc1101_tx_queue);
  }
  
}



extern "C"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint8_t newstate =
    ((CC1101_GDO0_GPIO_Port->IDR & CC1101_GDO0_Pin) ? 0x1 : 0) +
    ((CC1101_GDO2_GPIO_Port->IDR & CC1101_GDO2_Pin) ? 0x2 : 0);

  // 0x1 rise leading 0x2 rise is +1
  // 0x1 fall leading 0x2 fall is +1
  // The sequence should be 00,01,11,10,00 or 00,10,11,01,00
  // or                      0,1,3,2,0        0,2,3,1,0

  const int8_t state_table[4][4] = {
    // to state
    // 0,   1,  2,  3
    {  0,  +1, -1,  0}, // from state 0
    { -1,   0,  0, +1}, // from state 1
    { +1,   0,  0, -1}, // from state 2
    {  0,  -1, +1,  0}  // from state 3
  };
  count += state_table[state][newstate];
  state = newstate;
  
  if (lighton) {
    MODULE_LED_GPIO_Port->ODR |= MODULE_LED_Pin;
  } else {
    MODULE_LED_GPIO_Port->ODR &= ~(MODULE_LED_Pin);
  }
  lighton = !lighton;

}

void UartTransmit(const char* msg)
{
  HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
  while (state != HAL_UART_STATE_READY &&
         state != HAL_UART_STATE_BUSY_RX) ;
      
  __attribute__((unused)) HAL_StatusTypeDef tstate;
  tstate = HAL_UART_Transmit(&huart1, (uint8_t*) msg, std::strlen(msg), HAL_MAX_DELAY);
}

// to talk to UART minicom --baudrate 115200 --device /dev/ttyUSB0 
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_StateTypeDef state = HAL_UART_GetState(huart);
  if (state == HAL_UART_STATE_READY) {

  } else {

  }

  UART_send_from_buffer(uart_tx_queue);
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
{
  uart_rx_queue.num_rounds++;

  // don't need to do anything. DMA is circular

# if 0
  // this is what we use if not dma
  __attribute__((unused)) HAL_StatusTypeDef state;
  
  // -3 because of \n \r and 0x0 to end the string
  state = HAL_UART_Receive_IT(&huart1, received, sizeof(received)-3);
#endif
  
}



void app_init()
{
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.SetReceive();
  
  HAL_UART_Receive_DMA(&huart1, uart_rx_queue.tosend, sizeof(uart_rx_queue.tosend));

  UartTransmit("initialized\n\r");
}

void main_loop()
{

  while (1) {
    if ((HAL_GetTick() % 100) == 0) {

      // see page 287 of RM0008
      // CNDTR tells how much is left in the current DMA loop.
      // The circular DMA operation is initialized with a length/size of
      // sizeof(queue). since CNDTR is the number remaining, size-CNDTR is
      // how much has been transferred.
      uart_rx_queue.tosend_end = sizeof(uart_rx_queue.tosend) - huart1.hdmarx->Instance->CNDTR;
  
      auto deq = uart_rx_queue.dequeue();
      if (deq.second != deq.first) {
        SendData(deq.first, deq.second-deq.first, DSUart);
      }
      
      //if(CDC_GetTxState()) { return; }
      //CDC_Transmit_FS(deq.first, deq.second-deq.first);
  
    }

    if (0) {
      const char msg[] = "hello uart\n\r";
      UartTransmit(msg);
      uint8_t msgusb[] = "hello usb\n\r";
      CDC_Transmit_FS(msgusb, sizeof(msgusb));
    }

    if(ELECHOUSE_cc1101.CheckReceiveFlag()) {
      byte size=ELECHOUSE_cc1101.ReceiveData(RX_buffer);
      ELECHOUSE_cc1101.SetReceive();
      SendData(RX_buffer, size, DSCc1101);
    }
    
  }

}
