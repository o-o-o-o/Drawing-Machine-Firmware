#ifndef __SERVO_H
#define	__SERVO_H

#define SERVO LATBbits.LB0 //this port pin alternatively controls RELAY4
#define PEN_DOWN  1
#define PEN_UP 0

void PENdown(void);
void PENup(void);

#endif	/* SERVO_H */

