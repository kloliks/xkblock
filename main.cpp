#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>


const auto modKey = 78; // ScrollLock


void leave(Display* display, int exit_code) {
	XCloseDisplay(display);
	exit(exit_code);
}


int main(int argc, char** argv) {
	Display *display;
	Window window;
	XEvent ev;
	XSetWindowAttributes attrib;

	display = XOpenDisplay(0);

	if( display == NULL ){
		fprintf(stderr, "xkblock: cannot open display\n");
		exit(1);
	}

	attrib.override_redirect = True;

//	auto s = DefaultScreen(display);
//	window = XCreateSimpleWindow(display,
//	                             RootWindow(display, s),
//	                             10,
//	                             10,
//	                             200,
//	                             200,
//	                             1,
//	                             BlackPixel(display, s),
//	                             WhitePixel(display, s));
	window = XCreateWindow(display,
	                       DefaultRootWindow(display),
	                       0,
	                       0,
	                       1,
	                       1,
	                       0,
	                       CopyFromParent,
	                       InputOnly,
	                       CopyFromParent,
	                       CWOverrideRedirect,
	                       &attrib);

	XSelectInput(display, window, KeyPressMask|KeyReleaseMask);
	XMapWindow(display, window);

	int gs = 0;
	struct timeval tv;
	for( int tvt = 0; tvt < 100; ++tvt ){
		auto ret = XGrabKeyboard(display,window,False,GrabModeAsync,GrabModeAsync, CurrentTime);
		if (ret == GrabSuccess) {
			gs = 1;
			break;
		}
		/*grab failed; wait .01s*/
		tv.tv_sec=0;
		tv.tv_usec=10000;
		select(1, NULL, NULL, NULL, &tv);
	}
	if( gs == 0 ){
		fprintf(stderr, "xkblock: cannot grab keyboard\n");
		leave(display, 2);
	}

	while( True ){
		XNextEvent(display, &ev);
		if( ev.type == KeyPress && ev.xkey.keycode == modKey ){
			break;
		}
	}

	leave(display, 0);
}
