#include "helpers/logger.h"
#include "usbd_framework.h"

int main(void) {
	log_info("Program entry point.\n");

    usbd_init();

	while (1)
		;
}
