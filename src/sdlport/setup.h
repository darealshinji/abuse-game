#ifndef _SETUP_H_
#define _SETUP_H_

struct flags_struct
{
	short fullscreen;
	short doublebuf;
	short mono;
	short nosound;
	short grabmouse;
	short nosdlparachute;
	short xres;
	short yres;
	short overlay;
	short gl;
	int antialias;
};

struct keys_struct
{
	int left;
	int right;
	int up;
	int down;
	int b1;
	int b2;
	int b3;
	int b4;
};

#endif // _SETUP_H_
