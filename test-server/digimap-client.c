/*
 * digimap-client.c
 *
 *  Created on: 08.06.2015
 *      Author: botho
 *
 * Copyright (C) 2015 Botho Hohbaum, Media Impression Unit 08 <bh@miu08.de>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#ifdef _WIN32
#define random rand
#else
#include <unistd.h>
#endif
#include "lws_config.h"
#include "../lib/libwebsockets.h"

static unsigned int opts;
static int deny_deflate;
static int deny_mux;
static struct libwebsocket *wsi_event;
static volatile int force_exit = 0;
static int longlived = 0;
static char *input;
static int msg_sent = 0;
static unsigned char *buf;

enum demo_protocols {
	PROTOCOL_EVENT_NOTIFY,

	/* always last */
	DEMO_PROTOCOL_COUNT
};

typedef struct per_session_data__event_notify {
	struct libwebsocket *wsi;
	char msg[1024 * 64];
} t_per_session_data__event_notify;



static int
callback_event_dispatch(struct libwebsocket_context *context,
						struct libwebsocket *wsi,
						enum libwebsocket_callback_reasons reason,
						void *user, void *in, size_t len) {
	struct per_session_data__event_notify *pss = (struct per_session_data__event_notify *)user;
	int l = 0;
	int n;

	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED:
			break;
		case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
			break;
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			break;
		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
			break;
		case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
			break;
		case LWS_CALLBACK_GET_THREAD_ID:
			return pthread_self();
		case LWS_CALLBACK_ADD_POLL_FD:
			break;
		case LWS_CALLBACK_DEL_POLL_FD:
			break;
		case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
			break;
		case LWS_CALLBACK_LOCK_POLL:
			break;
		case LWS_CALLBACK_UNLOCK_POLL:
			break;
		case LWS_CALLBACK_PROTOCOL_DESTROY:
			break;
		case LWS_CALLBACK_WSI_DESTROY:
			break;
		case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
			break;
		case LWS_CALLBACK_PROTOCOL_INIT:
			break;
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			libwebsocket_callback_on_writable(context, wsi);
			break;
		case LWS_CALLBACK_CLOSED:
			wsi_event = NULL;
			break;
		case LWS_CALLBACK_CLIENT_RECEIVE:
			libwebsocket_read(context, wsi, buf, sizeof(buf));
			break;
		case LWS_CALLBACK_CLIENT_WRITEABLE:
			strcpy(pss->msg, input);
			n = libwebsocket_write(wsi, (unsigned char *)pss->msg, strlen(pss->msg), opts | LWS_WRITE_TEXT);
			if (n < 0) {
				return -1;
			}
			if (n < l) {
				lwsl_err("Partial write LWS_CALLBACK_CLIENT_WRITEABLE\n");
				return -1;
			}
			msg_sent = 1;
			libwebsocket_callback_on_writable(context, wsi);
			break;
		default:
			break;
	}

	return 0;
}


/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
	{
		"event-dispatch-protocol",
		callback_event_dispatch,
		sizeof(t_per_session_data__event_notify),
		128,
	},
	{ NULL, NULL, 0, 0 } /* end */
};

void sighandler(int sig) {
	force_exit = 1;
}

static struct option options[] = {
	{ "help",	no_argument,		NULL, 'h' },
	{ "debug",      required_argument,      NULL, 'd' },
	{ "port",	required_argument,	NULL, 'p' },
	{ "ssl",	no_argument,		NULL, 's' },
	{ "version",	required_argument,	NULL, 'v' },
	{ "undeflated",	no_argument,		NULL, 'u' },
	{ "nomux",	no_argument,		NULL, 'n' },
	{ "longlived",	no_argument,		NULL, 'l' },
	{ NULL, 0, 0, 0 }
};


int main(int argc, char **argv) {
	int n = 0;
	int m = 0;
	int ret = 0;
	int port = 7681;
	int use_ssl = 0;
	struct libwebsocket_context *context;
	const char *address;
	int ietf_version = -1; /* latest */
	struct lws_context_creation_info info;

	input = malloc(1024 * 64);
	memset(input, 0, sizeof(*input));
	memset(&info, 0, sizeof(info));
	buf = malloc(1024 * 64);
	lwsl_notice("DigiMap WebSocket Client\n"
			"(C) Copyright 2015 MediaImpression Unit 08 <bh@miu08.de>\n");
	if (argc < 2) {
		goto usage;
	}
	while (n >= 0) {
		n = getopt_long(argc, argv, "nuv:hsp:d:l", options, NULL);
		if (n < 0)
			continue;
		switch (n) {
		case 'd':
			lws_set_log_level(atoi(optarg), NULL);
			break;
		case 's':
			use_ssl = 2; /* 2 = allow selfsigned */
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'l':
			longlived = 1;
			break;
		case 'v':
			ietf_version = atoi(optarg);
			break;
		case 'u':
			deny_deflate = 1;
			break;
		case 'n':
			deny_mux = 1;
			break;
		case 'h':
			goto usage;
		}
	}
	if (optind >= argc) {
		goto usage;
	}
	signal(SIGINT, sighandler);
	address = argv[optind];
	/*
	 * create the websockets context.  This tracks open connections and
	 * knows how to route any traffic and which protocol version to use,
	 * and if each connection is client or server side.
	 *
	 * For this client-only demo, we tell it to not listen on any port.
	 */
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
#ifndef LWS_NO_EXTENSIONS
	info.extensions = libwebsocket_get_internal_extensions();
#endif
	info.gid = -1;
	info.uid = -1;
	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		lwsl_err("Creating libwebsocket context failed\n");
		return 1;
	}
	do {
		n = fread((char *)(input + m), 1024 * 64, 1, stdin);
		m += n;
	} while (n > 0);
	lwsl_notice(input);
	lwsl_notice("Waiting for connect...\n");
	n = libwebsocket_service(context, 100);
	if (n < 0) goto bail;
	if (wsi_event) goto bail;
	/* create a client websocket */
	wsi_event = libwebsocket_client_connect(context,
											address, port, use_ssl,  "/",
											argv[optind], argv[optind],
											protocols[PROTOCOL_EVENT_NOTIFY].name, ietf_version);
	if (wsi_event == NULL) {
		fprintf(stderr, "libwebsocket connect failed\n");
		ret = 1;
		goto bail;
	}
	libwebsocket_callback_on_writable(context, wsi_event);
	while (!msg_sent) {
		libwebsocket_service(context, 1000000);
	}
bail:
	lwsl_notice("Exiting\n");
	libwebsocket_context_destroy(context);
	free(buf);
	return ret;
usage:
	fprintf(stderr, "Usage: libwebsockets-test-client "
				"<server address> [--port=<p>] "
				"[--ssl] [-k] [-v <ver>] "
				"[-d <log bitfield>] [-l]\n");
	return 1;
}
