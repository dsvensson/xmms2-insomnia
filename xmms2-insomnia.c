/* Copyright (c) 2012, Daniel Svensson <dsvensson@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <CoreServices/CoreServices.h>

#include <xmmsclient/xmmsclient.h>
#include <xmmsclient/xmmsclient-cf.h>

#ifdef DEBUG
#define debug(x) printf (x)
#else
#define debug(x)
#endif

static void
update_system_activity (xmms_playback_status_t status)
{
	if (status == XMMS_PLAYBACK_STATUS_PLAY) {
		debug ("Updating system activity\n");
		UpdateSystemActivity (OverallAct);
	}
}

static int
on_playback_status_changed (xmmsv_t *value, void *udata)
{
	int32_t *status = (int32_t *) udata;

	if (!xmmsv_get_int (value, status)) {
		fprintf (stderr, "ERROR: Broken result.\n");
		return 1;
	}

	update_system_activity (*status);

	return 1;
}

static void
on_update_system_activity (CFRunLoopTimerRef timer, void *udata)
{
	xmms_playback_status_t *status = (xmms_playback_status_t *) udata;
	update_system_activity (*status);
}

static void
on_disconnect (void *udata)
{
	debug ("Shutting down main loop\n");
	CFRunLoopStop (CFRunLoopGetCurrent ());
}

int
main (int argc, char **argv) {
	xmmsc_connection_t *connection;
	xmmsc_result_t *broadcast;
	xmmsc_result_t *normal;
	xmms_playback_status_t status;

	CFRunLoopTimerContext context;
	CFRunLoopSourceRef source;
	CFRunLoopTimerRef timer;

	connection = xmmsc_init ("Mac-OS-X-insomnia-inducer");

	if (!xmmsc_connect (connection, NULL)) {
		fprintf (stderr, "Could not connect.\n");
		return EXIT_FAILURE;
	}

	status = XMMS_PLAYBACK_STATUS_STOP;

	memset (&context, 0, sizeof (CFRunLoopTimerContext));
	context.info = &status;

	timer = CFRunLoopTimerCreate (NULL, CFAbsoluteTimeGetCurrent(), 30, 0, 0,
	                              on_update_system_activity, &context);
	if (timer == NULL) {
		fprintf (stderr, "Could not create timer.\n");
		return EXIT_FAILURE;
	}

	CFRunLoopAddTimer (CFRunLoopGetCurrent (), timer, kCFRunLoopCommonModes);

	normal = xmmsc_playback_status (connection);
	xmmsc_result_notifier_set (normal, on_playback_status_changed, &status);
	xmmsc_result_unref (normal);

	broadcast = xmmsc_broadcast_playback_status (connection);
	xmmsc_result_notifier_set (broadcast, on_playback_status_changed, &status);

	xmmsc_disconnect_callback_set (connection, on_disconnect, &context);

	xmmsc_mainloop_cf_init (connection, &source);
	CFRunLoopRun ();
	xmmsc_mainloop_cf_shutdown (connection, source);

	xmmsc_result_disconnect (broadcast);
	xmmsc_result_unref (broadcast);

	xmmsc_unref (connection);

	CFRunLoopTimerInvalidate (timer);
	CFRelease (timer);

	return EXIT_SUCCESS;
}
