#include <stdio.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>

#include "event_listener.h"
#include "shortcut_handler.h"
#include "backend/backends.h"

#ifdef DEBUG
#define DEBUGMSG(msg...) printf(msg)
#else
#define DEBUGMSG(msg...)
#endif

/* Time in seconds before poweroff when holding the power switch.
 * Set to 0 to disable this feature. */
#ifndef POWEROFF_TIMEOUT
#define POWEROFF_TIMEOUT 3
#endif

#define DEAD_ZONE		450
#define SLOW_MOUSE_ZONE		600
#define AXIS_ZERO_0		1730
#define AXIS_ZERO_1		1730

#if (POWEROFF_TIMEOUT > 0)
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#endif

static struct uinput_user_dev uud = {
	.name = "OpenDingux mouse daemon",
	.id = { BUS_USB, 1,1,1 },
};

enum _mode {
	NORMAL, MOUSE, HOLD, DPAD, DPADMOUSE
};

static enum _mode mode = NORMAL;

static int event0 = -1, jevent0 = -1, uinput = -1;
static bool grabbed, power_button_pressed, is_mouse = false, is_dpad = false;
static int epollfd = -1;

static void switchmode(enum _mode new)
{

	//fprintf(stderr, "Switch Mode OLD: %d NEW: %d\n",(int)mode,(int)new);
	if (new == mode) return;
	
	switch(mode) {
		case NORMAL:
			switch(new) {
				case DPAD:
				case MOUSE:
				case DPADMOUSE:
					// enable read from joystick
					{
						struct epoll_event ev;
						ev.events = EPOLLIN;
						ev.data.fd = jevent0;
						epoll_ctl(epollfd, EPOLL_CTL_ADD, jevent0, &ev);
					}
					if (ioctl(jevent0, EVIOCGRAB, true) == -1)
						perror(__func__);
					grabbed = true;
					break;
				case HOLD:
					grabbed = true;
					blank(1);
					break;
				default:
					break;
			}
			mode = new;
			break;
		case DPAD:
		case MOUSE:
		case DPADMOUSE:
			switch(new) {
				case NORMAL:
					// disable read from joystick
					epoll_ctl(epollfd, EPOLL_CTL_DEL, jevent0, NULL);
					if (ioctl(jevent0, EVIOCGRAB, false) == -1)
						perror(__func__);
					grabbed = false;
					break;
				case HOLD:
					grabbed = true;
					blank(1);
					break;
				default:
					break;
			}
			mode = new;
			break;
		case HOLD:
			switch(new) {
				case NORMAL:
					grabbed = false;
					break;
				default:
					break;
			}
			mode = new;
			blank(0);
			break;
		default:
			break;
	}
	is_dpad = mode == DPAD || mode == DPADMOUSE;
	is_mouse = mode == MOUSE || mode == DPADMOUSE;
}


static void execute(enum event_type event, int value)
{
	char *str = NULL;
	switch(event) {
#ifdef BACKEND_REBOOT
		case reboot:
			if (value != 1) return;
			str = "reboot";
			do_reboot();
			break;
#endif
#ifdef BACKEND_POWEROFF
		case poweroff:
			if (value == 2) return;
			str = "poweroff";
			do_poweroff();
			break;
#endif
#ifdef BACKEND_SUSPEND
		case suspend:
			if (value != 1) return;
			str = "suspend";
			do_suspend();
			break;
#endif
		case hold:
			if (value != 1) return;
			str = "hold";
			if (mode == HOLD)
				switchmode(NORMAL);
			else
				switchmode(HOLD);
			break;
#ifdef BACKEND_VOLUME
		case volup:
			str = "volup";
			vol_up(value);
			break;
		case voldown:
			str = "voldown";
			vol_down(value);
			break;
#endif
#ifdef BACKEND_BRIGHTNESS
		case brightup:
			str = "brightup";
			blank(0);
			bright_up(value);
			break;
		case brightdown:
			str = "brightdown";
			if (get_brightness() <= 1) {
				blank(1);
			} else {
				bright_down(value);
			}
			break;
#endif
#ifdef BACKEND_SHARPNESS
		case sharpup:
			str = "sharpup";
			sharp_up(value);
			break;
		case sharpdown:
			str = "sharpdown";
			sharp_down(value);
			break;
#endif

		case mouse:
			if (value != 1) return;
			str = "mouse";
			if (mode == MOUSE) 
				switchmode(NORMAL);
			else if (mode == DPAD)
				switchmode(DPADMOUSE);
			else if (mode == DPADMOUSE)
				switchmode(DPAD);
			else
				switchmode(MOUSE);
			break;
		case dpad:
			if (value != 1) return;
			str = "dpad";
			if (mode == DPAD)
				switchmode(NORMAL);
			else if (mode == MOUSE)
				switchmode(DPADMOUSE);
			else if (mode == DPADMOUSE)
				switchmode(MOUSE);
			else 
				switchmode(DPAD);
			break;
		case dpadmouse:
			if (value != 1) return;
			str = "dpadmouse";
			if (mode == DPADMOUSE)
				switchmode(NORMAL);
			else 
				switchmode(DPADMOUSE);
			break;
#ifdef BACKEND_TVOUT
		case tvout:
			if (value != 1) return;
			str = "tvout";
			tv_out();
			break;
#endif
#ifdef BACKEND_SCREENSHOT
		case screenshot:
			if (value != 1) return;
			str = "screenshot";
			do_screenshot();
			break;
#endif
#ifdef BACKEND_KILL
		case kill:
			if (value != 1) return;
			str = "kill";
			do_kill();
			break;
#endif
#ifdef BACKEND_RATIOMODE
		case ratiomode:
			if (value != 1) return;
			str = "ratiomode";
			do_change_ratiomode();
			break;
#endif
		default:
			return;
	}
	DEBUGMSG("Execute: %s.\n", str);
}


static int open_fds(const char *event0fn, const char *jeventfn, const char *uinputfn)
{
	event0 = open(event0fn, O_RDONLY | O_NONBLOCK);
	if (event0 < 0) {
		perror("opening event0");
		return -1;
	}

	jevent0 = open(jeventfn, O_RDONLY | O_NONBLOCK);
	if (jevent0 < 0) {
		perror("opening jevent");
		return -1;
	}

	uinput = open(uinputfn, O_RDWR);
	if (uinput < 0) {
		perror("opening uinput");
		return -1;
	}

	int fd = uinput;
	write(fd, &uud, sizeof(uud));


	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BTN_LEFT) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT) == -1) goto filter_fail;

	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_X) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_Y) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_B) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_A) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_L1) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_R1) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_L2) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_R2) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_START) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_SELECT) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_MENU) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_POWER) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_VOLUP) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, BUTTON_VOLDOWN) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_UP) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_DOWN) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_LEFT) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT) == -1) goto filter_fail;


	if (ioctl(fd, UI_SET_EVBIT, EV_REL) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_RELBIT, REL_X) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_RELBIT, REL_Y) == -1) goto filter_fail;
	if (ioctl(fd, UI_SET_RELBIT, REL_WHEEL) == -1) goto filter_fail;

	if (ioctl(fd, UI_SET_KEYBIT, BTN_MOUSE) == -1) goto filter_fail;

	if (ioctl(fd, UI_DEV_CREATE)) {
		perror("creating device");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = event0;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, event0, &ev);

	return 0;

filter_fail:
	perror("setting event filter bits");
	return -1;
}


static int inject(unsigned short type, unsigned short code, int value)
{
	struct input_event inject_event;

	DEBUGMSG("Injecting event.\n");
	inject_event.value = value;
	inject_event.type = type;
	inject_event.code = code;
	inject_event.time.tv_sec = time(0);
	inject_event.time.tv_usec = 0;
	return write(uinput, &inject_event, sizeof(struct input_event));
}


#if (POWEROFF_TIMEOUT > 0)
struct poweroff_thd_args {
	pthread_mutex_t mutex, mutex2;
	unsigned long timeout;
	unsigned char canceled;
};

static void * poweroff_thd(void *p)
{
	struct poweroff_thd_args *args = p;
	unsigned long *args_timeout = &args->timeout;
	unsigned char *canceled = &args->canceled;

	while (1) {
		unsigned long time_us;
		long delay;

		pthread_mutex_unlock(&args->mutex2);
		pthread_mutex_lock(&args->mutex);

		do {
			struct timeval tv;

			gettimeofday(&tv, NULL);
			time_us = tv.tv_sec * 1000000 + tv.tv_usec;
			delay = *args_timeout - time_us;

			DEBUGMSG("poweroff thread: sleeping for %i micro-seconds\n", delay);
			if (delay <= 0)
				break;

			usleep(delay);

		} while (*args_timeout > time_us + delay);

		if (!*canceled)
			execute(poweroff, 0);

		DEBUGMSG("Poweroff canceled\n");
	}

	return NULL;
}
#endif


bool power_button_is_pressed(void)
{
	return power_button_pressed;
}

int do_listen(const char *event, const char *jevent, const char *uinput)
{
#define MAX_EVENTS 2
	struct epoll_event ev, events[MAX_EVENTS];
	epollfd = epoll_create1(0);
	if (epollfd == -1) {
		perror("epoll_create1");
		return -1;
	}

	if (open_fds(event, jevent, uinput)) {
		close(epollfd);
		epollfd = -1;
		return -1;
	}

	const struct shortcut *tmp;
	const struct shortcut *shortcuts = getShortcuts();

#if (POWEROFF_TIMEOUT > 0)
	bool with_poweroff_timeout = true;
	struct poweroff_thd_args *args;

	/* If a poweroff combo exist, don't activate the shutdown
	 * on timeout feature */
	for (tmp = shortcuts; tmp; tmp = tmp->prev)
		if (tmp->action == poweroff) {
			with_poweroff_timeout = false;
			break;
		}

	if (with_poweroff_timeout) {
		args = malloc(sizeof(*args));

		if (!args) {
			fprintf(stderr, "Unable to allocate memory\n");
			with_poweroff_timeout = false;
		} else {
			pthread_t thd;

			pthread_mutex_init(&args->mutex, NULL);
			pthread_mutex_init(&args->mutex2, NULL);
			pthread_mutex_lock(&args->mutex);

			pthread_create(&thd, NULL, poweroff_thd, args);
		}
	}
#endif

	bool combo_used = false;
	bool last_dpad[4] = {false,false,false,false};
	bool current_dpad[4] = {false,false,false,false};
	short dpad_buttons[4] = {BUTTON_LEFT,BUTTON_RIGHT,BUTTON_UP,BUTTON_DOWN};
	short mouse_x,mouse_y;

	while(1) {
		int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		struct input_event my_event, my_jevent;
		int eread = 0, jread = 0, n;
		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == event0)
				eread = read(event0, &my_event, sizeof(struct input_event));
			else
				jread = read(jevent0, &my_jevent, sizeof(struct input_event));
		}

		if ( is_dpad && jread && ! power_button_pressed) {
			if (jread && !power_button_pressed && my_jevent.type == EV_ABS) {
				//fprintf(stderr, "code: %d value: %d\n",my_jevent.code,my_jevent.value);
				switch(my_jevent.code) {
					case 0:
						current_dpad[0] = my_jevent.value < AXIS_ZERO_0 - DEAD_ZONE;
						current_dpad[1] = my_jevent.value > AXIS_ZERO_0 + DEAD_ZONE;
						
						break;
					case 1:
						current_dpad[2] = my_jevent.value < AXIS_ZERO_1 - DEAD_ZONE;
						current_dpad[3] = my_jevent.value > AXIS_ZERO_1 + DEAD_ZONE;
						break;
					default:
						break;
				}
				int i;
				for (i = 0; i < 4; i ++) {
					if(!last_dpad[i] && current_dpad[i]){
						inject(EV_KEY, dpad_buttons[i] , 1);
						inject(EV_SYN, SYN_REPORT, 0);
					} else if(last_dpad[i] && !current_dpad[i]) {
						inject(EV_KEY, dpad_buttons[i] , 0);
						inject(EV_SYN, SYN_REPORT, 0);
					}

				}
				memcpy(last_dpad, current_dpad, sizeof(last_dpad));
			}
			if(mode == DPAD && eread) {
				inject(EV_KEY, my_event.code, my_event.value);
				inject(EV_SYN, SYN_REPORT, 0);
			}
		}

		if (eread) {
			// If the power button is pressed, block inputs (if it wasn't already blocked)
			if (my_event.code == EVENT_SWITCH_POWER) {

				// We don't want key repeats on the power button.
				if (my_event.value == 2)
					continue;

				DEBUGMSG("(un)grabbing.\n");
				power_button_pressed = !!my_event.value;

				if (!power_button_pressed) {
					// Send release events to all active combos.
					for (tmp = shortcuts; tmp; tmp = tmp->prev) {
						bool was_combo = true;
						unsigned int i;
						for (i = 0; i < tmp->nb_keys; i++) {
							struct button *button = tmp->keys[i];
							was_combo &= !!button->state;
							button->state = 0;
						}
						if (was_combo)
							execute(tmp->action, 0);
					}

					if (!combo_used) {
						DEBUGMSG("No combo used, injecting BUTTON_POWER\n");
						inject(EV_KEY, BUTTON_POWER, 1);
						inject(EV_KEY, BUTTON_POWER, 0);
						inject(EV_SYN, SYN_REPORT, 0);
					}

					combo_used = false;
				}

#if (POWEROFF_TIMEOUT > 0)
				if (with_poweroff_timeout) {
					if (power_button_pressed) {
						struct timeval tv;

						gettimeofday(&tv, NULL);
						args->timeout = (tv.tv_sec + POWEROFF_TIMEOUT)
									* 1000000 + tv.tv_usec;
						args->canceled = 0;
						if (!pthread_mutex_trylock(&args->mutex2))
							pthread_mutex_unlock(&args->mutex);
					} else {
						DEBUGMSG("Power button released: canceling poweroff\n");
						args->canceled = 1;
					}
				}
#endif

				if (!grabbed) {
					if (ioctl(event0, EVIOCGRAB, power_button_pressed)
							== -1)
						perror(__func__);
				}
				continue;
			}
			else if (!power_button_pressed)
			{
				
				if (my_event.code == EVENT_SWITCH_VOLUP) {
					execute(volup, my_event.value);
					
				}
				if (my_event.code == EVENT_SWITCH_VOLDOWN) {
					execute(voldown, my_event.value);
				}
				
			}


			// If the power button is currently pressed, we enable shortcuts.
			if (power_button_pressed) {
				for (tmp = shortcuts; tmp; tmp = tmp->prev) {
					bool was_combo = true, is_combo = true, match = false;
					unsigned int i;
					for (i = 0; i < tmp->nb_keys; i++) {
						struct button *button = tmp->keys[i];
						was_combo &= !!button->state;
						if (my_event.code == button->id) {
							match = 1;
							button->state = (my_event.value != 0);
						}
						is_combo &= button->state;
					}

					if (match && (was_combo || is_combo)) {
#if (POWEROFF_TIMEOUT > 0)
						if (with_poweroff_timeout) {
							DEBUGMSG("Combo: canceling poweroff\n");
							args->canceled = 1;
						}
#endif
						combo_used = true;
						execute(tmp->action, my_event.value);
					}
				}
				continue;
			}
		}

		// In case we are in the "mouse" mode, handle mouse emulation.
		if (is_mouse || is_dpad) {

			// We don't want to move the mouse if the power button is pressed.
			if (power_button_pressed)
				continue;

			// An event occured
			if (eread) {
				unsigned int i;

				// Toggle the "value" flag of the button object
				for (i = 0; i < nb_buttons; i++) {
					if (buttons[i].id == my_event.code)
						buttons[i].state = my_event.value;
				}

				switch(my_event.code) {
					case BUTTON_L2:
					case BUTTON_R2:
						if (is_mouse) {
							if (my_event.value == 2) /* Disable repeat on mouse buttons */
								continue;

							inject(EV_KEY, my_event.code == BUTTON_L2 ? BTN_LEFT : BTN_RIGHT, my_event.value);
							inject(EV_SYN, SYN_REPORT, 0);
							continue;
						}
						// If the event is not mouse-related, we reinject it.
						inject(EV_KEY, my_event.code, my_event.value);
						inject(EV_SYN, SYN_REPORT, 0);
						continue;

					case BUTTON_UP:
					case BUTTON_DOWN:
					case BUTTON_LEFT:
					case BUTTON_RIGHT:
					case BUTTON_X:
					case BUTTON_Y:
					case BUTTON_A:
					case BUTTON_B:
					case BUTTON_L1:
					case BUTTON_R1:
					case BUTTON_START:
					case BUTTON_SELECT:
					case BUTTON_MENU:

						// If the event is not mouse-related, we reinject it.
						inject(EV_KEY, my_event.code, my_event.value);
						inject(EV_SYN, SYN_REPORT, 0);
						continue;

					default:
						continue;
				}
			}

			// 
			if(is_mouse && jread && my_jevent.value != 0) {
				// For each direction of the D-pad, we check the state of the corresponding button.
				// If it is pressed, we inject an event with the corresponding mouse movement.
				
				switch(my_jevent.code) {
					case 0: {
						if(my_jevent.value < AXIS_ZERO_0 - DEAD_ZONE) {
							if (my_jevent.value < AXIS_ZERO_0 - DEAD_ZONE - SLOW_MOUSE_ZONE)
								mouse_x = 0-5; 
							else
								mouse_x = 0-1; 
						} else if (my_jevent.value > AXIS_ZERO_0 + DEAD_ZONE) {
							if (my_jevent.value > AXIS_ZERO_0 + DEAD_ZONE + SLOW_MOUSE_ZONE)
								mouse_x = 5; 
							else
								mouse_x = 1; 
						} else {
							mouse_x = 0;
						}
						break;
					}
					case 1: {
						if(my_jevent.value < AXIS_ZERO_1 - DEAD_ZONE) {
							if (my_jevent.value < AXIS_ZERO_1 - DEAD_ZONE - SLOW_MOUSE_ZONE)
								mouse_y = 0-5; 
							else
								mouse_y = 0-1; 
						} else if (my_jevent.value > AXIS_ZERO_1 + DEAD_ZONE) {
							if (my_jevent.value > AXIS_ZERO_1 + DEAD_ZONE + SLOW_MOUSE_ZONE)
								mouse_y = 5; 
							else
								mouse_y = 1; 
						} else {
							mouse_y = 0;
						}
						break;
					default:
						break;
					}

				}

			}
			if (mouse_y != 0)
				inject(EV_REL, REL_Y, mouse_y);
			if (mouse_x != 0)
				inject(EV_REL, REL_X, mouse_x);
			inject(EV_SYN, SYN_REPORT, 0); 
		}
	}

	return -1;
}
