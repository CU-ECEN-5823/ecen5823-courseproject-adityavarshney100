/*
 * display.c
 *
 *  Created on: Jan 1, 2019
 *      Author: Dan Walkes
 *      Edited: Dave Sluiter, Jan 5, 2021, mods to get default file to compile without warnings
 *              and moved the list of includes to .h file.
 *
 *  Students: There are 3 edits required in this file, 2 edits to function displayInit()
 *            1 edit to function displayUpdate().
 *            See comments in these functions.
 *
 * @brief Contains functions used to control the LCD display on the Silicon Labs
 * blue gecko development board for ECEN 5823
 */

//#define INCLUDE_LOG_DEBUG 1

#include "display.h"




#if ECEN5823_INCLUDE_DISPLAY_SUPPORT

/**
 * The number of characters per row
 */
#define DISPLAY_ROW_LEN   			 32
/**
 * The number of rows
 */
#define DISPLAY_ROW_NUMBER_OF_ROWS	 8

/**
 * A structure containing information about the data we want to display on a given
 * LCD display
 */
struct display_data {
	/**
	 * tracks the state of the extcomin pin for toggling purposes
	 */
	bool last_extcomin_state_high;
	/**
	 * GLIB_Context required for use with GLIB_ functions
	 */
	GLIB_Context_t context;
	/**
	 * The char content of each row, null terminated
	 */
	char row_data[DISPLAY_ROW_NUMBER_OF_ROWS][DISPLAY_ROW_LEN+1];
};

/**
 * We only support a single global display data structure and a
 * single display with this design
 */
static struct display_data global_display_data;

static struct display_data *displayGetData() {
	return &global_display_data;
}

/**
 * Necessary to prevent a compiler warning on GCC7, despite the fact strnlen is defined in
 * string.h.  I'm not sure why this is necessary
 */
extern size_t strnlen(const char *, size_t);

/**
 * Write the display data in the buffer represented by @param display to the device
 */
static void displayUpdateWriteBuffer(struct display_data *display)
{
	enum display_row row = DISPLAY_ROW_NAME;
	GLIB_Context_t *context = &display->context;
	EMSTATUS result = GLIB_clear(context);
	if( result != GLIB_OK ) {
		LOG_ERROR("GLIB_Clear failed with result %d",(int)result);
	} else {
		/**
		 * See example in graphics.c graphPrintCenter()
		 */
		for( row = DISPLAY_ROW_NAME; row < DISPLAY_ROW_MAX; row ++) {
			uint8_t row_len = strnlen(display->row_data[row],DISPLAY_ROW_LEN);
			uint8_t row_width = row_len * context->font.fontWidth;
			if( row_width > context->pDisplayGeometry->xSize ) {
				LOG_ERROR("Content of display row %d (%s) with length %d font width %d is too wide for display geometry size %d",
						row,&display->row_data[row][0],row_len,context->font.fontWidth,context->pDisplayGeometry->xSize);
			} else {
				uint8_t posX = (context->pDisplayGeometry->xSize - row_width) >> 1;
				uint8_t posY = ((context->font.lineSpacing + context->font.fontHeight) * row)
							   + context->font.lineSpacing;
				result = GLIB_drawString(context, &display->row_data[row][0], row_len, posX, posY, 0);
				if( result != GLIB_OK ) {
					if( result == GLIB_ERROR_NOTHING_TO_DRAW ) {
						/**
						 * This error happens if the content of the draw string did not change
						 */
						LOG_DEBUG("GLIB_drawString returned GLIB_ERROR_NOTHING_TO_DRAW for string %s len %d",&display->row_data[row][0],row_len);
						result = GLIB_OK;
					} else {
						LOG_ERROR("GLIB_drawString failed with result %d for content %s length %d at X=%d Y=%d",
								(int)result,&display->row_data[row][0],row_len,posX,posY);
					}
				}
			}
		}
	}
	result = DMD_updateDisplay();
	if( result != DMD_OK ) {
		LOG_ERROR("DMD_updateDisplay failed with result %d",(int)result);
	}
}


/**
 * Based on example from graphInit() graphics.c
 */
static void displayGlibInit(GLIB_Context_t *context)
{
	EMSTATUS status;

	/* Initialize the display module. */
	status = DISPLAY_Init();
	if (DISPLAY_EMSTATUS_OK != status) {
		LOG_ERROR("Failed to init display, error was %d",(int)status);
	} else {
		/* Initialize the DMD module for the DISPLAY device driver. */
		status = DMD_init(0);
		if (DISPLAY_EMSTATUS_OK != status) {
			LOG_ERROR("Failed to init DMD, error was %d",(int)status);
		} else {
			status = GLIB_contextInit(context);
			if (DISPLAY_EMSTATUS_OK != status) {
				LOG_ERROR("Failed to init GLIB context, error was %d",(int)status);
			} else {
				context->backgroundColor = White;
				context->foregroundColor = Black;

				/* Use Narrow font */
				status = GLIB_setFont(context, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
				if( GLIB_OK != status ) {
					LOG_ERROR("Failed to set font to GLIB_FontNarrow6x8 in GLIB_setFont, error was %d",(int)status);
				}
			}
		}
	}
}



// ****************************************************************
// The following routines are the public functions
// ****************************************************************



void displayPrintf(enum display_row row, const char *format, ... )
{
	struct display_data *display = displayGetData();
	if( row >= DISPLAY_ROW_MAX ) {
		LOG_WARN("Row %d exceeded max row, ignoring write request",row);
	} else {
		va_list args;
		va_start (args, format);
		int chars_written = vsnprintf(&display->row_data[row][0],DISPLAY_ROW_LEN,format,args);
		va_end(args);
		if( chars_written < 0 ) {
			LOG_WARN("Error encoding format string %s",format);
		}
		if( chars_written >= DISPLAY_ROW_LEN ) {
			LOG_WARN("Exceeded row buffer length for row %d with format string %s",row,format);
			chars_written = DISPLAY_ROW_LEN -1;
		}
		/**
		 * Ensure null terminator
		 */
		display->row_data[row][chars_written] = 0;
		LOG_DEBUG("Updating display row %d with content \"%s\"",row,&display->row_data[row][0]);
	}

	displayUpdateWriteBuffer(display);

} // displayPrintf()




/**
 * Initialize the display.  Must call
 * @param header represents the content
 */
void displayInit()
{
	struct display_data *display = displayGetData();
	enum display_row row;

#if GPIO_DISPLAY_SUPPORT_IMPLEMENTED

    // Students: Create the function gpioI2CSensorEnSetOn() to set SENSOR_ENABLE=1, see main board schematic.
    //           Add that function to gpio.c.
    //           Then uncomment the following line
	//gpioI2CSensorEnSetOn(); // we need SENSOR_ENABLE=1 which is tied to DISP_ENABLE for the LCD, on all the time now
	
//#else
//#warning "gpioEnableDisplay is not implemented, please implement in order to use the display"
#endif

	memset(display,0,sizeof(struct display_data));
	display->last_extcomin_state_high = false;

	displayGlibInit(&display->context);

	// clear each row of the display
	for( row = DISPLAY_ROW_NAME; row < DISPLAY_ROW_MAX; row++ ) {
		displayPrintf(row,"%s"," ");
	}

#if SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT

#if TIMER_SUPPORTS_1HZ_TIMER_EVENT

	  // The BT stack implements timers that we can setup and then have the stack pass back
	  // events when the timer expires.
	  // This assignment has us using the Sharp LCD which needs to be serviced approx
	  // every 1 second, in order to toggle the input "EXTCOMIN" input to the LCD display.
	  // The documentation is a bit sketchy, but apparently charge can build up within
	  // the LCD and it needs to be bled off. So toggling the EXTCOMIN input is the method by
	  // which this takes place.
	  // We will get a gecko_evt_hardware_soft_timer_id event back from gecko_wait_event()
	  // as a result of calling gecko_cmd_hardware_set_soft_timer()

      // Students: Figure out what parameters to pass in to set up a 1 second repeating soft timer
      //           and uncomment the following lines

	  /*
	  struct gecko_msg_hardware_set_soft_timer_rsp_t     *timer_response;

	  timer_response = gecko_cmd_hardware_set_soft_timer();

	  if (timer_response->result != 0) {
	  	  LOG_ERROR("BT soft timer failed to initiate, error code=%d", timer_response->result);
	  }
      */

//#else
//#warning "Timer does not support scheduling 1Hz event.  Please implement for full display support"
#endif

//#else
//#warning "Display Update event is not implemented in scheduler.  Please implement for display support"
#endif



} // displayInit()




/**
 * Call this function from your event handler on 1 second intervals
 * to prevent charge buildup within the Liquid Crystal Cells.
 * See details in https://www.silabs.com/documents/public/application-notes/AN0048.pdf
 */
void displayUpdate()
{
	struct display_data *display = displayGetData();

	// toggle the var that remembers the state of EXTCOMIN pin
	display->last_extcomin_state_high = !display->last_extcomin_state_high;

#if GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED

    // Students: Create the function gpioSetDisplayExtcomin() that will toggle EXTCOMIN
    //           to the LCD. Add that function to gpio.c.
	//           Then uncomment the following line.
	//gpioSetDisplayExtcomin(display->last_extcomin_state_high);
	
//#else
//#warning "gpioSetDisplayExtcomin is not implemented.  Please implement for display support"
#endif


} // displayUpdate()

#endif // ECEN5823_INCLUDE_DISPLAY_SUPPORT
