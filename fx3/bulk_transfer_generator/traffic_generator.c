#include "traffic_generator.h"

#include <stddef.h>

#include "cyu3system.h"
#include "cyu3types.h"
#include "cyu3usb.h"

static uint16_t tg_get_endpoint_packet_size (CyU3PUSBSpeed_t speed)
{
	// Note that this needs to correspond with the settings in the USB descriptors
	switch (speed)
	{
	case CY_U3P_FULL_SPEED: return 64;
	case CY_U3P_HIGH_SPEED: return 512;
	case CY_U3P_SUPER_SPEED: return 1024;
	default: return 0;
	}
}

static CyU3PErrorCode_t tg_endpoint_init (
	uint8_t endpoint
)
{
	CyU3PEpConfig_t config;
	CyU3PMemSet((uint8_t*)&config, 0, sizeof(CyU3PEpConfig_t));

	const CyU3PUSBSpeed_t speed = CyU3PUsbGetSpeed();

	CyU3PDebugPrint(4, "tg_endpoint_init(): USB speed %d\r\n", speed);

	config.enable = CyTrue;
	config.epType = CY_U3P_USB_EP_BULK;
	config.streams = 0;
	config.pcktSize = tg_get_endpoint_packet_size(speed);
	config.burstLen = speed == CY_U3P_SUPER_SPEED ? 16 : 1;
	config.isoPkts = 0;

	const CyU3PErrorCode_t status_config = CyU3PSetEpConfig(endpoint, &config);
	if (status_config != CY_U3P_SUCCESS) {
		return status_config;
	}

	const CyU3PErrorCode_t status_flush = CyU3PUsbFlushEp(endpoint);
	if (status_flush != CY_U3P_SUCCESS) {
		return status_flush;
	}

	return CY_U3P_SUCCESS;
}

static CyU3PErrorCode_t tg_dma_create (
	traffic_generator_t* tg
	, uint16_t size
	, uint16_t count
	, CyU3PDmaSocketId_t socket_id
)
{
	CyU3PDmaChannelConfig_t config;
	CyU3PMemSet((uint8_t*)&config, 0, sizeof(CyU3PDmaChannelConfig_t));

	config.size = size;
	config.count = count;
	config.prodSckId = CY_U3P_CPU_SOCKET_PROD;
	config.consSckId = socket_id;
	config.prodAvailCount = 0;
	config.prodHeader = 0;
	config.prodFooter = 0;
	config.consHeader = 0;
	config.dmaMode = CY_U3P_DMA_MODE_BYTE;
	config.notification = 0;
	config.cb = NULL;

	const CyU3PErrorCode_t status_create = CyU3PDmaChannelCreate(&tg->dma, CY_U3P_DMA_TYPE_MANUAL_OUT, &config);
	if (status_create != CY_U3P_SUCCESS) {
		CyU3PDebugPrint(4, "CyU3PDmaChannelCreate() failed: %d\r\n", status_create);
		return status_create;
	}

	const CyU3PErrorCode_t status_xfer = CyU3PDmaChannelSetXfer(&tg->dma, 0);
	if (status_xfer != CY_U3P_SUCCESS) {
		CyU3PDebugPrint(4, "CyU3PDmaChannelSetXfer() failed: %d\r\n", status_xfer);
		return status_xfer;
	}

	return CY_U3P_SUCCESS;
}

static CyU3PErrorCode_t tg_send_zlp (traffic_generator_t* tg)
{
	CyU3PErrorCode_t status_buffer;
	do {
		CyU3PDmaBuffer_t buffer;
		status_buffer = CyU3PDmaChannelGetBuffer(&tg->dma, &buffer, 10);
	} while (status_buffer == CY_U3P_ERROR_TIMEOUT);
	if (status_buffer != CY_U3P_SUCCESS) {
		return status_buffer;
	}

	const CyU3PErrorCode_t status_commit = CyU3PDmaChannelCommitBuffer(&tg->dma, 0, 0);
	if (status_commit != CY_U3P_SUCCESS) {
		return status_commit;
	}

	return CY_U3P_SUCCESS;
}

extern CyU3PErrorCode_t tg_init (
	traffic_generator_t* tg
	, uint8_t endpoint
	, uint16_t size
	, CyU3PDmaSocketId_t socket_id
)
{
	const CyU3PErrorCode_t status_endpoint_init = tg_endpoint_init(endpoint);
	if (status_endpoint_init != CY_U3P_SUCCESS) {
		CyU3PDebugPrint(4, "status_endpoint_init() failed: %d\r\n", status_endpoint_init);
		return status_endpoint_init;
	}

	const uint16_t count = 4;

	const CyU3PErrorCode_t status_dma_create = tg_dma_create(tg, size, count, socket_id);
	if (status_dma_create != CY_U3P_SUCCESS) {
		CyU3PDebugPrint(4, "tg_dma_create() failed: %d\r\n", status_dma_create);
		return status_dma_create;
	}

	return CY_U3P_SUCCESS;
}

extern CyU3PErrorCode_t tg_send (traffic_generator_t* tg, uint32_t len)
{
	uint32_t sent = 0;

	do {

		CyU3PDmaBuffer_t buffer;

		const CyU3PErrorCode_t status_buffer = CyU3PDmaChannelGetBuffer(&tg->dma, &buffer, CYU3P_NO_WAIT);
		if (status_buffer != CY_U3P_SUCCESS) {
			if (status_buffer == CY_U3P_ERROR_TIMEOUT) {
				CyU3PThreadSleep(10);
				continue;
			}
			return status_buffer;
		}

		const uint32_t remaining = len - sent;
		const uint16_t count = remaining >= buffer.size ? buffer.size : remaining;
		const uint16_t buffer_status = 0;

		const CyU3PErrorCode_t status_commit = CyU3PDmaChannelCommitBuffer(&tg->dma, count, buffer_status);
		if (status_commit != CY_U3P_SUCCESS) {
			return status_commit;
		}

		sent += count;

		// From https://www.beyondlogic.org/usbnutshell/usb4.shtml
		//
		// "A bulk transfer is considered complete when it has transferred
		// the exact amount of data requested, transferred a packet less than
		// the maximum endpoint size, or transferred a zero-length packet."
		//
		// We need to send a ZLP to the host if the number of bytes in the
		// last buffer is a multiple of the packet size. If this is not done,
		// the host will (most likely) receive all the data but detect a timeout.
		const CyBool_t is_last = remaining <= buffer.size ? CyTrue : CyFalse;
		if (is_last && count % tg_get_endpoint_packet_size(CyU3PUsbGetSpeed()) == 0) {
			const CyU3PErrorCode_t status_zlp = tg_send_zlp(tg);
			if (status_zlp != CY_U3P_SUCCESS) {
				return status_zlp;
			}
		}

	} while (sent < len);

	return CY_U3P_SUCCESS;
}

extern void tg_deinit (traffic_generator_t* tg)
{
	// TODO
}
