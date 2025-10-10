#ifndef ENCODER_H
#define	ENCODER_H

#include <xc.h>

#define ENCODER_A_PIN          RC6
#define ENCODER_A_PIN_TRIS     TRISC6

#define ENCODER_B_PIN          RC7
#define ENCODER_B_PIN_TRIS     TRISC7

#define ENCODER_PUSH_PIN       RC1
#define ENCODER_PUSH_PIN_TRIS  TRISC1

#define ENCODER_LEFT_BUTTON  128
#define ENCODER_RIGHT_BUTTON 129
#define ENCODER_PUSH_BUTTON  130

void encoder_init(void);
inline void encoder_worker(void);

#endif
