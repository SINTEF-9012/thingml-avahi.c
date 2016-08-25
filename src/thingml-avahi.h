/*
 * thingml-avahi.h
 *
 *  Created on: Aug 22, 2016
 *      Author: vassik
 */

#ifndef SRC_THINGML_AVAHI_H_
#define SRC_THINGML_AVAHI_H_

#include "thingml-avahi-utility.h"


#ifdef __cplusplus
extern "C" {
#endif


void start_avahi_client(ThingMLThreadedAhvaiClient* client_data);

void stop_avahi_client(ThingMLThreadedAhvaiClient* client_data);

void add_dnssd_service(ThingMLAvahiService* service);

void remove_dnssd_service(ThingMLAvahiService* service);

ThingMLAvahiService* constructThingMLAvahiService();

void distructThingMLAvahiService(ThingMLAvahiService** service_data);


#ifdef __cplusplus
}
#endif


#endif /* SRC_THINGML_AVAHI_H_ */
