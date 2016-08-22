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
	char* name = NULL;
	const char* type = NULL;
	const char* domain = NULL;
	const char* host = NULL;
	uint16_t port = 0;
	const char* txt = NULL;

	pthingmlAvahiCallback fn_srv_esteb_callback = NULL;
	pthingmlAvahiCallback fn_srv_failure_callback = NULL;

	void* thing_instance = NULL;

	AvahiClient* client = NULL;
	AvahiEntryGroup* group = NULL;
	AvahiThreadedPoll* threaded_poll = NULL;

} ThingMLAvahiService;

#ifdef __cplusplus
}
#endif

#endif /* SRC_THINGML_AVAHI_UTILITY_H_ */
