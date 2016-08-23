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


void create_services(ThingMLAvahiService *service);

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
            create_services(context);
        }; break;

        case AVAHI_ENTRY_GROUP_FAILURE : {

            fprintf(stderr, "Entry group failure: %s\n", avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));

            /* Some kind of failure happened while we were registering our services */
            avahi_threaded_poll_quit(context->threaded_poll);

            if(context->fn_srv_failure_callback != NULL)
            	context->fn_srv_failure_callback(context->thing_instance, context);

        }; break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}

void create_services(ThingMLAvahiService *service) {
	char *n, r[128];
    int ret;
    assert(service->client);

    /* If this is the first time we're called, let's create a new
     * entry group if necessary */

    if (!service->group)
        if (!(service->group = avahi_entry_group_new(service->client, entry_group_callback, service))) {
            fprintf(stderr, "avahi_entry_group_new() failed: %s\n", avahi_strerror(avahi_client_errno(service->client)));
            goto fail;
        }

    /* If the group is empty (either because it was just created, or
     * because it was reset previously, add our entries.  */

    if (avahi_entry_group_is_empty(service->group)) {
        fprintf(stderr, "Adding service '%s'\n", service->name);


        /* Add the service */
        if ((ret = avahi_entry_group_add_service(service->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0, service->name, service->type, service->domain, service->host, service->port, service->txt, r, NULL)) < 0) {

            if (ret == AVAHI_ERR_COLLISION)
                goto collision;

            fprintf(stderr, "Failed to add _ipp._tcp service: %s\n", avahi_strerror(ret));
            goto fail;
        }

        /* Tell the server to register the service */
        if ((ret = avahi_entry_group_commit(service->group)) < 0) {
            fprintf(stderr, "Failed to commit entry group: %s\n", avahi_strerror(ret));
            goto fail;
        }
    }

    return;

collision:

    /* A service name collision with a local service happened. Let's
     * pick a new name */
    n = avahi_alternative_service_name(service->name);
    avahi_free(service->name);
    service->name = n;

    fprintf(stderr, "Service name collision, renaming service to '%s'\n", service->name);

    avahi_entry_group_reset(service->group);

    create_services(service);
    return;

fail:

	avahi_threaded_poll_quit(service->threaded_poll);
}

void client_callback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata) {
    assert(c);
    assert(userdata);

    ThingMLAvahiService* service = (ThingMLAvahiService*) userdata;
    service->client = c;

    /* Called whenever the client or server state changes */

    switch (state) {
        case AVAHI_CLIENT_S_RUNNING:

            /* The server has startup successfully and registered its host
             * name on the network, so it's time to create our services */
            create_services(service);
            break;

        case AVAHI_CLIENT_FAILURE:

            fprintf(stderr, "Client failure: %s\n", avahi_strerror(avahi_client_errno(c)));
            avahi_threaded_poll_quit(service->threaded_poll);

            break;

        case AVAHI_CLIENT_S_COLLISION:

            /* Let's drop our registered services. When the server is back
             * in AVAHI_SERVER_RUNNING state we will register them
             * again with the new host name. */

        case AVAHI_CLIENT_S_REGISTERING:

            /* The server records are now being established. This
             * might be caused by a host name change. We need to wait
             * for our own records to register until the host name is
             * properly esatblished. */

            if (service->group)
                avahi_entry_group_reset(service->group);

            break;

        case AVAHI_CLIENT_CONNECTING:
            ;
    }
}

void start_dnssd_service(ThingMLAvahiService* service_data) {
	int error;
	service_data->name = avahi_strdup(service_data->name);

    /* Allocate main loop object */
    if (!(service_data->threaded_poll = avahi_threaded_poll_new())) {
        fprintf(stderr, "Failed to create simple poll object.\n");
        return;
    }

    service_data->client = avahi_client_new(avahi_threaded_poll_get(service_data->threaded_poll), 0, client_callback, service_data, &error);

    /* Check wether creating the client object succeeded */
    if (!service_data->client) {
        fprintf(stderr, "Failed to create client: %s\n", avahi_strerror(error));
        return;
    }

    avahi_threaded_poll_start(service_data->threaded_poll);
}

void stop_dnssd_service(ThingMLAvahiService* service_data) {

	if(service_data->threaded_poll)
		avahi_threaded_poll_stop(service_data->threaded_poll);

    if (service_data->client)
        avahi_client_free(service_data->client);

    if (service_data->threaded_poll)
        avahi_threaded_poll_free(service_data->threaded_poll);

    avahi_free(service_data->name);
}
