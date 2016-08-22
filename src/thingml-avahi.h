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

void start_dnssd_service(ThingMLAvahiService* service_data);

void stop_dnssd_service(ThingMLAvahiService* service_data);


#ifdef __cplusplus
}
#endif


#endif /* SRC_THINGML_AVAHI_H_ */
