
#include <ini.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event_listener.h"
#include "shortcut_handler.h"

#define BUTTON(btn) BUTTON_##btn
#define _BUTTON(btn) {#btn, sizeof #btn, BUTTON(btn), 0}

static struct shortcut *shortcuts;

/* Buttons available for shortcuts */
struct button buttons[] = {
	_BUTTON(UP),
	_BUTTON(DOWN),
	_BUTTON(LEFT),
	_BUTTON(RIGHT),
	_BUTTON(A),
	_BUTTON(B),
	_BUTTON(X),
	_BUTTON(Y),
	_BUTTON(L1),
	_BUTTON(R1),
	_BUTTON(SELECT),
	_BUTTON(START),
	_BUTTON(HOLD),
	_BUTTON(VOLUP),
	_BUTTON(VOLDOWN),
	_BUTTON(L2),
	_BUTTON(R2),
	-BUTTON(MENU),
	
};

unsigned int nb_buttons = sizeof(buttons) / sizeof(buttons[0]);

static struct {
	enum event_type type;
	const char *str;
} mapping[] = {
	  { reboot,		"REBOOT", },
	  { poweroff,	"POWEROFF", },
	  { suspend,	"SUSPEND", },
	  { hold,		"HOLD", },
	  { volup,		"VOLUME_UP", },
	  { voldown,	"VOLUME_DOWN", },
	  { brightup,	"BRIGHTNESS_UP", },
	  { brightdown,	"BRIGHTNESS_DOWN", },
	  { sharpup, "SHARPNESS_UP",},
	  { sharpdown, "SHARPNESS_DOWN",},
	  { mouse,		"MOUSE_EMULATION", },
	  { tvout,		"TV_OUT", },
	  { screenshot,	"SCREENSHOT", },
	  { kill,		"KILL", },
	  { ratiomode,          "RATIOMODE", },
	  { dpad,          "DPAD", },
	  { dpadmouse,          "DPADMOUSE", },
};

static void shortcut_free(struct shortcut *scuts)
{
	struct shortcut *prev;
	while(scuts) {
		prev = scuts->prev;
		free(scuts);
		scuts = prev;
	}
}


int read_conf_file(const char *filename)
{
	int nb = 0;
	struct shortcut *old = NULL;
	struct INI *ini = ini_open(filename);
	if (!ini)
		return -1;

	while (1) {
		const char *name;
		size_t len;
		int res = ini_next_section(ini, &name, &len);
		if (res == 0)
			fprintf(stderr, "Unable to find [Shortcuts] section\n");
		if (res <= 0)
			return -1;
		if (!strncmp(name, "Shortcuts", len))
			break;
	}

	while (1) {
		struct shortcut *new;
		const char *key, *value, *action = NULL;
		size_t klen, vlen;
		int res, i, nb_keys;

		res = ini_read_pair(ini, &key, &klen, &value, &vlen);
		if (res < 0) {
			fprintf(stderr, "Error while reading key/value pair\n");
			goto error;
		}
		if (!res)
			break;

		for (i = 0; i < sizeof(mapping) / sizeof(mapping[0]); i++)
			if (!strncmp(key, mapping[i].str, klen)) {
				action = mapping[i].str;
				break;
			}

		if (!action) {
			fprintf(stderr, "Unknown shortcut action in config file\n");
			continue;
		}

		new = malloc(sizeof(*new));
		if (!new) {
			fprintf(stderr, "Unable to allocate memory\n");
			goto error;
		}

		new->action = mapping[i].type;

		nb_keys = 0;
		while (1) {
			int j, found = 0;
			for (j = 0; j < nb_buttons; j++) {
				size_t len = buttons[j].name_len - 1;
				if (!strncmp(buttons[j].name, value, len)) {
					if (vlen > len && value[len] != ',' && value[len] != '\n')
						break;
					new->keys[nb_keys++] = &buttons[j];
					value += len;
					vlen -= len;
					found = 1;
					break;
				}
			}

			if (!found) {
				fprintf(stderr, "Unknown shortcut button in config file\n");
				break;
			}

			if (vlen == 0)
				break;

			/* Skip the comma */
			vlen--;
			value++;
		}

		if (nb_keys == 0) {
			fprintf(stderr, "Shortcut skipped for action %s"
						" (no button mapped)\n", action);
		}

		new->nb_keys = nb_keys;
		new->prev = old;
		old = new;
		nb++;
	}

	shortcuts = old;
	ini_close(ini);
	return nb;

error:
	shortcut_free(shortcuts);
	ini_close(ini);
	return -1;
}


const struct shortcut * getShortcuts()
{
	return (const struct shortcut*)shortcuts;
}


void deinit()
{
	shortcut_free(shortcuts);
}
