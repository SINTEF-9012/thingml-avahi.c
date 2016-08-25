/*
 * thingml-avahi.c
 *
 *  Created on: Aug 22, 2016
 *      Author: vassik
 */

#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/alternative.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>
#include <avahi-common/thread-watch.h>

#include "thingml-avahi-utility.h"
#include "thingml-avahi.h"


void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata) {
	assert(userdata);

	ThingMLAvahiService* context = (ThingMLAvahiService*) userdata;

    assert(g == context->group || context->group == NULL);
    context->group = g;

    /* Called whenever the entry group state changes */

    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED : {
            /* The entry group has been established successfully */
            fprintf(stderr, "Service '%s' successfully established.\n", context->name);
            context->state = THINGML_AVAHI_SERVICE_PUBLISH;

            if(context->fn_srv_esteb_callback != NULL)
            	context->fn_srv_esteb_callback(context->thing_instance, context);

        }; break;

        case AVAHI_ENTRY_GROUP_COLLISION : {
            char *n;

            /* A service name collision with a remote service
             * happened. Let's pick a new name */
            n = avahi_alternative_service_name(context->name);
            avahi_free(context->name);
            context->name = n;

            fprintf(stderr, "Service name collision, renaming service to '%s'\n", context->name);

            /* And recreate the services */
            add_dnssd_service(context);
        }; break;

        case AVAHI_ENTRY_GROUP_FAILURE : {

            fprintf(stderr, "Entry group failure: %s\n", avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));

            if(context->fn_srv_failure_callback != NULL)
            	context->fn_srv_failure_callback(context->thing_instance, context);

        }; break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}

void client_callback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata) {
    assert(c);
    assert(userdata);

    ThingMLThreadedAhvaiClient* client_data = (ThingMLThreadedAhvaiClient*) userdata;

    client_data->client = c;

    /* Called whenever the client or server state changes */

    switch (state) {
        case AVAHI_CLIENT_S_RUNNING: {

            /* The server has startup successfully and registered its host
             * name on the network, so it's time to create our services */
        	if(client_data->fn_client_running_callback)
        		client_data->fn_client_running_callback(client_data->thing_instance);

        }; break;

        case AVAHI_CLIENT_FAILURE: {

            fprintf(stderr, "Client failure: %s\n", avahi_strerror(avahi_client_errno(c)));

            if(client_data->fn_client_failure_callback)
            	client_data->fn_client_failure_callback(client_data->thing_instance);

        }; break;

        case AVAHI_CLIENT_S_COLLISION:

            /* Let's drop our registered services. When the server is back
             * in AVAHI_SERVER_RUNNING state we will register them
             * again with the new host name. */
        	break;

        case AVAHI_CLIENT_S_REGISTERING:

            /* The server records are now being established. This
             * might be caused by a host name change. We need to wait
             * for our own records to register until the host name is
             * properly esatblished. */
            break;

        case AVAHI_CLIENT_CONNECTING:
            ;
    }
}

void start_avahi_client(ThingMLThreadedAhvaiClient* client_data) {
	int error;

    /* Allocate main loop object */
    if (!(client_data->threaded_poll = avahi_threaded_poll_new())) {
        fprintf(stderr, "Failed to create simple poll object.\n");
        return;
    }

    const AvahiPoll* avahi_poll = avahi_threaded_poll_get(client_data->threaded_poll);

    client_data->client = avahi_client_new(avahi_poll, 0, client_callback, client_data, &error);

    /* Check weather creating the client object succeeded */
    if (!client_data->client) {
        fprintf(stderr, "Failed to create client: %s\n", avahi_strerror(error));
        return;
    }

    avahi_threaded_poll_start(client_data->threaded_poll);
}

void stop_avahi_client(ThingMLThreadedAhvaiClient* client_data) {
	if(client_data->threaded_poll)
		avahi_threaded_poll_stop(client_data->threaded_poll);

    if (client_data->client)
        avahi_client_free(client_data->client);

    if (client_data->threaded_poll)
        avahi_threaded_poll_free(client_data->threaded_poll);
}

void add_dnssd_service(ThingMLAvahiService *service) {
	char r[128];
    int ret;

    char* name = avahi_strdup(service->name);

    avahi_threaded_poll_lock(service->avahi_client->threaded_poll);

    int state = avahi_client_get_state(service->avahi_client->client);

    if(state != AVAHI_CLIENT_S_RUNNING) {
    	fprintf(stderr, "add_dnssd_service() failed due to client is in wrong state: %s\n", avahi_strerror(avahi_client_errno(service->avahi_client->client)));
    	avahi_threaded_poll_unlock(service->avahi_client->threaded_poll);
    	avahi_free(name);
    	return;
    }

    service->name = name;

    if (!service->group) {
        if (!(service->group = avahi_entry_group_new(service->avahi_client->client, entry_group_callback, service)))
            fprintf(stderr, "avahi_entry_group_new() failed: %s\n", avahi_strerror(avahi_client_errno(service->avahi_client->client)));
    }

    avahi_threaded_poll_unlock(service->avahi_client->threaded_poll);

    /* Add the service */
    if ((ret = avahi_entry_group_add_service(service->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0, service->name, service->type, service->domain, service->host, service->port, service->txt, r, NULL)) < 0) {

        if (ret == AVAHI_ERR_COLLISION)
        	fprintf(stderr, "Failed to add  service AVAHI_ERR_COLLISION: %s\n", avahi_strerror(ret));

        fprintf(stderr, "Failed to add  service: %s\n", avahi_strerror(ret));
    }

    /* Tell the server to register the service */
    if ((ret = avahi_entry_group_commit(service->group)) < 0)
        fprintf(stderr, "Failed to commit entry group: %s\n", avahi_strerror(ret));
}

void remove_dnssd_service(ThingMLAvahiService *service) {
	assert(service != NULL);

	if(service->state == THINGML_AVAHI_SERVICE_PUBLISH) {
		avahi_entry_group_reset(service->group);
		avahi_free(service->name);
		service->state = THINGML_AVAHI_SERVICE_UNPUBLISH;
	}
}

ThingMLAvahiService* constructThingMLAvahiService() {
	ThingMLAvahiService* service = malloc(sizeof(ThingMLAvahiService));

	service->domain = NULL;
	service->fn_srv_esteb_callback = NULL;
	service->fn_srv_failure_callback = NULL;
	service->group = NULL;
	service->host = NULL;
	service->name = NULL;
	service->port = 0;
	service->state = THINGML_AVAHI_SERVICE_NOT_INIT;
	service->thing_instance = NULL;
	service->txt = NULL;
	service->type = NULL;
	service->avahi_client = NULL;
	return service;
}

void distructThingMLAvahiService(ThingMLAvahiService** service_data) {
	free(*service_data);
	*service_data = NULL;
}
