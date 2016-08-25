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

typedef enum {
	THINGML_AVAHI_SERVICE_UNPUBLISH,
	THINGML_AVAHI_SERVICE_PUBLISH,
	THINGML_AVAHI_SERVICE_NOT_INIT
} ThingMLServiceState;

typedef struct {
	AvahiClient* client;
	AvahiThreadedPoll* threaded_poll;

	pthingmlAvahiCallback fn_client_failure_callback;
	pthingmlAvahiCallback fn_client_running_callback;

	void* thing_instance;

} ThingMLThreadedAhvaiClient;

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

	AvahiEntryGroup* group;
	ThingMLServiceState state;
	ThingMLThreadedAhvaiClient* avahi_client;

} ThingMLAvahiService;


#ifdef __cplusplus
}
#endif

#endif /* SRC_THINGML_AVAHI_UTILITY_H_ */
