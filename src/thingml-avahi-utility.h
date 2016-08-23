/*
 * thingml-avahi-utility.h
 *
 *  Created on: Aug 22, 2016
 *      Author: vassik
 */

#ifndef SRC_THINGML_AVAHI_UTILITY_H_
#define SRC_THINGML_AVAHI_UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <avahi-common/thread-watch.h>
#include <avahi-client/publish.h>


typedef void (*pthingmlAvahiCallback)(void* _instance, ...);

typedef struct {
	char* name;
	const char* type;
	const char* domain;
	const char* host;
	uint16_t port;
	const char* txt;

	pthingmlAvahiCallback fn_srv_esteb_callback;
	pthingmlAvahiCallback fn_srv_failure_callback;

	void* thing_instance;

	AvahiClient* client;
	AvahiEntryGroup* group;
	AvahiThreadedPoll* threaded_poll;

} ThingMLAvahiService;

#ifdef __cplusplus
}
#endif

#endif /* SRC_THINGML_AVAHI_UTILITY_H_ */
