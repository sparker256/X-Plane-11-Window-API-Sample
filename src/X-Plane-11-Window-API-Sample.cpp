// Downloaded from https://developer.x-plane.com/code-sample/x-plane-11-window-api-sample/


#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <string.h>
#include <stdio.h>
#if IBM
	#include <windows.h>
#endif
#if LIN
#include <GL/gl.h>
#endif
#if __GNUC__ && APL
#include <OpenGL/gl.h>
#endif
#if __GNUC__ && IBM
#include <GL/gl.h>
#endif

#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif

static XPLMWindowID	g_window;

void				draw(XPLMWindowID in_window_id, void * in_refcon);
int					handle_mouse(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);
void				receive_main_monitor_bounds(int inMonitorIndex, int inLeftBx, int inTopBx, int inRightBx, int inBottomBx, void * refcon);

int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }

static const char * g_pop_out_label = "Pop Out";
static const char * g_pop_in_label = "Pop In";
static float g_pop_button_lbrt[4]; // left, bottom, right, top
static float g_position_button_lbrt[4]; // left, bottom, right, top

static int	coord_in_rect(float x, float y, float * bounds_lbrt)  { return ((x >= bounds_lbrt[0]) && (x < bounds_lbrt[2]) && (y < bounds_lbrt[3]) && (y >= bounds_lbrt[1])); }


PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
	strcpy(outName, "GuiSamplePlugin");
	strcpy(outSig, "xpsdk.examples.guisampleplugin");
	strcpy(outDesc, "A test plug-in that demonstrates the X-Plane 11 GUI plugin API.");

	// We're not guaranteed that the main monitor's lower left is at (0, 0)... we'll need to query for the global desktop bounds!
	int global_desktop_bounds[4]; // left, bottom, right, top
	XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.left = global_desktop_bounds[0] + 50;
	params.bottom = global_desktop_bounds[1] + 150;
	params.right = global_desktop_bounds[0] + 350;
	params.top = global_desktop_bounds[1] + 450;
	params.visible = 1;
	params.drawWindowFunc = draw;
	params.handleMouseClickFunc = handle_mouse;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	params.decorateAsFloatingWindow = 1;
	
	g_window = XPLMCreateWindowEx(&params);
	
	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	XPLMSetWindowGravity(g_window, 0, 1, 0, 1); // As the X-Plane window resizes, keep our size constant, and our left and top edges in the same place relative to the window's left/top
	XPLMSetWindowResizingLimits(g_window, 200, 200, 500, 500); // Limit resizing our window: maintain a minimum width/height of 200 boxels and a max width/height of 500
	XPLMSetWindowTitle(g_window, "Sample Window");

	return (g_window != NULL);
}

PLUGIN_API void	XPluginStop(void)
{
	// Since we created the window, we'll be good citizens and clean it up
	XPLMDestroyWindow(g_window);
	g_window = NULL;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

void	draw(XPLMWindowID in_window_id, void * in_refcon)
{
	char scratch_buffer[150];
	float col_white[] = {1.0, 1.0, 1.0};

	XPLMSetGraphicsState(
			0 /* no fog */,
			0 /* 0 texture units */,
			0 /* no lighting */,
			0 /* no alpha testing */,
			1 /* do alpha blend */,
			1 /* do depth testing */,
			0 /* no depth writing */
	);


	// We draw our rudimentary button boxes based on the height of the button text
	int char_height;
	XPLMGetFontDimensions(xplmFont_Proportional, NULL, &char_height, NULL);

	// We'll change the text of the pop-in/pop-out button based on our current state
	int is_popped_out = XPLMWindowIsPoppedOut(in_window_id);
	const char * pop_label = is_popped_out ? g_pop_in_label : g_pop_out_label;

	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);

	// Draw our buttons
	{
		// Position the pop-in/pop-out button in the upper left of the window (sized to fit the
		g_pop_button_lbrt[0] = l + 10;
		g_pop_button_lbrt[3] = t - 15;
		g_pop_button_lbrt[2] = g_pop_button_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, pop_label, strlen(pop_label)); // *just* wide enough to fit the button text
		g_pop_button_lbrt[1] = g_pop_button_lbrt[3] - (1.25f * char_height); // a bit taller than the button text

		// Position the "move to lower left" button just to the right of the pop-in/pop-out button
		const char * position_btn_text = "Move to Lower Left";
		g_position_button_lbrt[0] = g_pop_button_lbrt[2] + 30;
		g_position_button_lbrt[1] = g_pop_button_lbrt[1];
		g_position_button_lbrt[2] = g_position_button_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, position_btn_text, strlen(position_btn_text));
		g_position_button_lbrt[3] = g_pop_button_lbrt[3];

		// Draw the boxes around our rudimentary buttons
		float green[] = {0.0, 1.0, 0.0, 1.0};
		glColor4fv(green);
		glBegin(GL_LINE_LOOP);
		{
			glVertex2i(g_pop_button_lbrt[0], g_pop_button_lbrt[3]);
			glVertex2i(g_pop_button_lbrt[2], g_pop_button_lbrt[3]);
			glVertex2i(g_pop_button_lbrt[2], g_pop_button_lbrt[1]);
			glVertex2i(g_pop_button_lbrt[0], g_pop_button_lbrt[1]);
		}
		glEnd();
		glBegin(GL_LINE_LOOP);
		{
			glVertex2i(g_position_button_lbrt[0], g_position_button_lbrt[3]);
			glVertex2i(g_position_button_lbrt[2], g_position_button_lbrt[3]);
			glVertex2i(g_position_button_lbrt[2], g_position_button_lbrt[1]);
			glVertex2i(g_position_button_lbrt[0], g_position_button_lbrt[1]);
		}
		glEnd();

		// Draw the button text (pop in/pop out)
		XPLMDrawString(col_white, g_pop_button_lbrt[0], g_pop_button_lbrt[1] + 4, (char *)pop_label, NULL, xplmFont_Proportional);

		// Draw the button text (reposition)
		XPLMDrawString(col_white, g_position_button_lbrt[0], g_position_button_lbrt[1] + 4, (char *)position_btn_text, NULL, xplmFont_Proportional);
	}

	// Draw a bunch of informative text
	{
		// Set the y position for the first bunch of text we'll draw to a little below the buttons
		int y = g_pop_button_lbrt[1] - 2 * char_height;

		// Display the total global desktop bounds
		{
			int global_desktop_lbrt[4];
			XPLMGetScreenBoundsGlobal(&global_desktop_lbrt[0], &global_desktop_lbrt[3], &global_desktop_lbrt[2], &global_desktop_lbrt[1]);
			sprintf(scratch_buffer, "Global desktop bounds: (%d, %d) to (%d, %d)", global_desktop_lbrt[0], global_desktop_lbrt[1], global_desktop_lbrt[2], global_desktop_lbrt[3]);
			XPLMDrawString(col_white, l, y, scratch_buffer, NULL, xplmFont_Proportional);
			y -= 1.5 * char_height;
		}

		// Display our bounds
		if(XPLMWindowIsPoppedOut(in_window_id)) // we are in our own first-class window, rather than "floating" within X-Plane's own window
		{
			int window_os_bounds[4];
			XPLMGetWindowGeometryOS(in_window_id, &window_os_bounds[0], &window_os_bounds[3], &window_os_bounds[2], &window_os_bounds[1]);
			sprintf(scratch_buffer, "OS Bounds: (%d, %d) to (%d, %d)", window_os_bounds[0], window_os_bounds[1], window_os_bounds[2], window_os_bounds[3]);
			XPLMDrawString(col_white, l, y, scratch_buffer, NULL, xplmFont_Proportional);
			y -= 1.5 * char_height;
		}
		else
		{
			int global_bounds[4];
			XPLMGetWindowGeometry(in_window_id, &global_bounds[0], &global_bounds[3], &global_bounds[2], &global_bounds[1]);
			sprintf(scratch_buffer, "Window bounds: %d %d %d %d", global_bounds[0], global_bounds[1], global_bounds[2], global_bounds[3]);
			XPLMDrawString(col_white, l, y, scratch_buffer, NULL, xplmFont_Proportional);
			y -= 1.5 * char_height;
		}

		// Display whether we're in front of our our layer
		{
			sprintf(scratch_buffer, "In front? %s", XPLMIsWindowInFront(in_window_id) ? "Y" : "N");
			XPLMDrawString(col_white, l, y, scratch_buffer, NULL, xplmFont_Proportional);
			y -= 1.5 * char_height;
		}

		// Display the mouse's position info text
		{
			int mouse_global_x, mouse_global_y;
			XPLMGetMouseLocationGlobal(&mouse_global_x, &mouse_global_y);
			sprintf(scratch_buffer, "Draw mouse (global): %d %d\n", mouse_global_x, mouse_global_y);
			XPLMDrawString(col_white, l, y, scratch_buffer, NULL, xplmFont_Proportional);
			y -= 1.5 * char_height;
		}
	}

}

int	handle_mouse(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus is_down, void * in_refcon)
{
	if(is_down == xplm_MouseDown)
	{
		const int is_popped_out = XPLMWindowIsPoppedOut(in_window_id);
		if (!XPLMIsWindowInFront(in_window_id))
		{
			XPLMBringWindowToFront(in_window_id);
		}
		else if(coord_in_rect(x, y, g_pop_button_lbrt)) // user clicked the pop-in/pop-out button
		{
			XPLMSetWindowPositioningMode(in_window_id, is_popped_out ? xplm_WindowPositionFree : xplm_WindowPopOut, 0);
		}
		else if(coord_in_rect(x, y, g_position_button_lbrt)) // user clicked the "move to lower left" button
		{
			// If we're popped out, and the user hits the "move to lower left" button,
			// we need to move them to the lower left of their OS's desktop space (units are pixels).
			// On the other hand, if we're a floating window inside of X-Plane, we need
			// to move to the lower left of the X-Plane global desktop (units are boxels).
			void (* get_geometry_fn)(XPLMWindowID, int *, int *, int *, int *) = is_popped_out ? &XPLMGetWindowGeometryOS : &XPLMGetWindowGeometry;
			int lbrt_current[4];
			get_geometry_fn(in_window_id, &lbrt_current[0], &lbrt_current[3], &lbrt_current[2], &lbrt_current[1]);

			int h = lbrt_current[3] - lbrt_current[1];
			int w = lbrt_current[2] - lbrt_current[0];
			void (* set_geometry_fn)(XPLMWindowID, int, int, int, int) = is_popped_out ? &XPLMSetWindowGeometryOS : &XPLMSetWindowGeometry;

			// Remember, the main monitor's origin is *not* guaranteed to be (0, 0), so we need to query for it in order to move the window to its lower left
			int bounds[4] = {0}; // left, bottom, right, top
			if(is_popped_out)
			{
				XPLMGetScreenBoundsGlobal(&bounds[0], &bounds[3], &bounds[2], &bounds[1]);
			}
			else
			{
				XPLMGetAllMonitorBoundsOS(receive_main_monitor_bounds, bounds);
			}

			set_geometry_fn(in_window_id, bounds[0], bounds[1] + h, bounds[0] + w, bounds[1]);
		}
	}
	return 1;
}

void receive_main_monitor_bounds(int inMonitorIndex, int inLeftBx, int inTopBx, int inRightBx, int inBottomBx, void * refcon)
{
	int * main_monitor_bounds = (int *)refcon;
	if(inMonitorIndex == 0) // the main monitor
	{
		main_monitor_bounds[0] = inLeftBx;
		main_monitor_bounds[1] = inBottomBx;
		main_monitor_bounds[2] = inRightBx;
		main_monitor_bounds[3] = inTopBx;
	}
}

