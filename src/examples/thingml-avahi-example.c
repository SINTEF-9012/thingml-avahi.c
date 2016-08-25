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

void fn_client_running_callback(void * _instance, ...) {
	printf("fn_client_running_callback success is called\n");
}

void fn_client_failure_callback(void* _instance, ...) {
	printf("fn_client_failure_callback failuer is called\n");
}



int main(int argc, char* argv[]) {

	ThingMLAvahiService* service_data = NULL;
	ThingMLAvahiService* service_data1 = NULL;
	ThingMLThreadedAhvaiClient* avahi_client = NULL;
	int ch;

	do {
		printf("waiting for input...\n");
		ch = getchar();

		switch(ch) {
			case 's' : {
				avahi_client = malloc(sizeof(ThingMLThreadedAhvaiClient));
				avahi_client->fn_client_running_callback = fn_client_running_callback;
				avahi_client->fn_client_failure_callback = fn_client_failure_callback;
				start_avahi_client(avahi_client);
			}; break;

			case 'e' : {
				stop_avahi_client(avahi_client);
				free(avahi_client);
				if(service_data)
					free(service_data);

				if(service_data1)
					free(service_data1);
			}; break;

			case '1' : {
				service_data = constructThingMLAvahiService();
				service_data->avahi_client = avahi_client;
				service_data->fn_srv_esteb_callback = callback_success;
				service_data->fn_srv_failure_callback = callback_failuer;

				service_data->name = "MegaPrinter";
				service_data->type = "_ipp._tcp";
				service_data->port = 651,
				service_data->txt = "test=blah";
				printf("starting...\n");
				add_dnssd_service(service_data);
			}; break;

			case '2' : {
				service_data1 = constructThingMLAvahiService();
				service_data1->avahi_client = avahi_client;
				service_data1->fn_srv_esteb_callback = callback_success;
				service_data1->fn_srv_failure_callback = callback_failuer;

				service_data1->name = "MegaPrinter111";
				service_data1->type = "_ipp._tcp";
				service_data1->port = 651,
				service_data1->txt = "test=blah111";
				printf("starting...\n");
				add_dnssd_service(service_data1);
			}; break;

			case '3' : {
				printf("removing service...\n");
				remove_dnssd_service(service_data);
				distructThingMLAvahiService(&service_data);
			}; break;

			case '4' : {
				printf("removing service...\n");
				remove_dnssd_service(service_data1);
				distructThingMLAvahiService(&service_data1);
			}; break;
		}

	} while(ch != 'q');

	return 0;
}
