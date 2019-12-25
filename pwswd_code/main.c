
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "shortcut_handler.h"
#include "event_listener.h"
#include "backend/backends.h"

#ifndef PROGNAME
#define PROGNAME "pwswd"
#endif

#ifndef EVENT_FILENAME
#define EVENT_FILENAME  "/dev/input/event0"
#endif

#ifndef JEVENT_FILENAME
#define JEVENT_FILENAME  "/dev/input/event3"
#endif

#ifndef UINPUT_FILENAME
#define UINPUT_FILENAME "/dev/uinput"
#endif

#ifndef DEFAULT_MIXER
#error Makefile should define DEFAULT_MIXER
#endif

static const char * const usage = "Usage:\n\t " PROGNAME
	"[-f config file] [-e event interface] [-u uinput interface]"
#ifdef BACKEND_VOLUME
	" [-m mixer control]"
	" [-d DAC control]"
#endif
	"\n\n";


int main(int argc, char **argv)
{
	const char *filename = NULL,
#ifdef BACKEND_VOLUME
		  *mixer = NULL, *dac = NULL,
#endif
		  *event = NULL, *uinput = NULL, *jevent = NULL;
	int exitcode = EXIT_SUCCESS;
	size_t i;

	for (i = 1; i < argc; i++) {
		if (argc > i + 1) {
			if (!strcmp(argv[i], "-f"))
				filename = argv[i+1];
			else if (!strcmp(argv[i], "-e"))
				event = argv[i+1];
			else if (!strcmp(argv[i], "-u"))
				uinput = argv[i+1];
#ifdef BACKEND_VOLUME
			else if (!strcmp(argv[i], "-m"))
				mixer = argv[i+1];
			else if (!strcmp(argv[i], "-d"))
				dac = argv[i+1];
#endif
			else {
				printf(usage);
				return EXIT_FAILURE;
			}

			i++;
		} else {
			printf(usage);
			return EXIT_FAILURE;
		}
	}
	
	if (!filename) {
		struct stat st;
		filename = "/usr/local/etc/" PROGNAME ".conf";

		if (stat(filename, &st) == -1) {
			filename = "/etc/" PROGNAME ".conf";

			if (stat(filename, &st) == -1) {
				printf("pwswd: Unable to find a configuration file.\n");
				return EXIT_FAILURE;
			}
		}

		if (!S_ISREG(st.st_mode)) {
			printf("pwswd: The configuration file is not a regular file.\n");
			return EXIT_FAILURE;
		}
	}

	if (!event)
		event = EVENT_FILENAME;

	if (!jevent)
		jevent = JEVENT_FILENAME;

	if (!uinput)
		uinput = UINPUT_FILENAME;

	if (read_conf_file(filename) < 0) {
		fprintf(stderr, "Unable to parse configuration file: aborting.\n");
		return EXIT_FAILURE;
	}

#ifdef BACKEND_VOLUME
	if (!mixer)
		mixer = DEFAULT_MIXER;

	if (vol_init(mixer, dac))
		fprintf(stderr, "Unable to init volume backend\n");
#endif

	if (do_listen(event, jevent, uinput))
		exitcode = EXIT_FAILURE;
	deinit();
	return exitcode;
}
