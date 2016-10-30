#include "os_type.h"

void ir_init (void);
void ir_set  (bool on);
void ir_set_address (uint8_t address);
void ir_set_address_csum (uint8_t address, uint8_t csum);
void ir_send (uint8_t command);
void ir_send_csum (uint8_t command, uint8_t csum);
