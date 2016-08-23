/*
 * thingml-avahi-example.c
 *
 *  Created on: Aug 23, 2016
 *      Author: vassik
 */


#include <stdio.h>
#include <stdlib.h>

#include "../thingml-avahi-utility.h"
#include "../thingml-avahi.h"

void callback_success(void * _instance, ...) {
	printf("callback success is called\n");
}

void callback_failuer(void* _instance, ...) {
	printf("callback failuer is called\n");
}

int main(int argc, char* argv[]) {

	int ch;
	ThingMLAvahiService* service_data = malloc(sizeof(ThingMLAvahiService));

	do {
		printf("waiting for input...\n");
		ch = getchar();

		switch(ch) {
			case 's' : {
				service_data->fn_srv_esteb_callback = callback_success;
				service_data->fn_srv_failure_callback = callback_failuer;

				service_data->name = "MegaPrinter";
				service_data->type = "_ipp._tcp";
				service_data->port = 651,
				service_data->txt = "test=blah";
				printf("starting...\n");
				start_dnssd_service(service_data);
			}; break;
			case 'e' : {
				printf("stopping...\n");
				stop_dnssd_service(service_data);
			}; break;
		}

	} while(ch != 'q');


	free(service_data);
	return 0;
}
