#ifndef TRAFFIC_GENERATOR_H_
#define TRAFFIC_GENERATOR_H_

#include "cyu3dma.h"
#include "cyu3error.h"

typedef struct
{
	CyU3PDmaChannel dma;
} traffic_generator_t;

extern CyU3PErrorCode_t tg_init (traffic_generator_t* tg, uint8_t endpoint, uint16_t size, CyU3PDmaSocketId_t socket_id);

// TODO Add timeout argument?
extern CyU3PErrorCode_t tg_send (traffic_generator_t* tg, uint32_t len);

extern void tg_deinit (traffic_generator_t* tg);

#endif /* TRAFFIC_GENERATOR_H_ */
