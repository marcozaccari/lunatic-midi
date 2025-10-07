#ifndef BUTTONS_H
#define	BUTTONS_H

#include <xc.h>

void buttons_init(void);
inline void buttons_worker(void); // must be called at every main cycle

#endif
