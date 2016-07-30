#ifndef LED_H_
#define LED_H_

#include "os_type.h"

void led_init (void);
void led_toggle (void);
void led_set (bool on);
bool led_is_set (void);

#endif /* LED_H_ */
