/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                  Display "driver"                     *
 *                                                       *
 *  This is the display drawing functions that is used   *
 *  to display figures and text on the screen.           *
 *														 *
 * By: Marius C. K. Gulbrandsen                          *
 *     Antoni Climent Munoz                              *
 *     Andrea Mazzoli							         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Includes
//---------------------------------------------------
#include "display.h"

// Definitions
//---------------------------------------------------
#define FB_REFRESH 0x4680
#define handle_error(msg) \     
           do { perror(msg); exit(EXIT_FAILURE); } while (0)

// Variables
int fd_framebuffer; 				   // Framebuffer communication
uint16_t* map;			   // Pointer to the display memory map 
struct fb_copyarea screen; // The area to update the screen


// Function Definitions
//---------------------------------------------------
void display_init(uint16_t color)
{
	// Open framebuffer communication and error check
	fd_framebuffer = open("/dev/fb0", O_RDWR);
	if (fd_framebuffer == -1) {
    	handle_error("Error on open");
    }           

    // Save pointer to memory map and error check
	map = (uint16_t*)mmap(NULL, SCREEN_SIZE*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd_framebuffer, 0);
	if (map == MAP_FAILED) {
        handle_error("Error on mmap");
    }

    // Set background color
    display_set_background(color);
	
}

void display_refresh(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	// Refresh screen
    screen.dx = x;
    screen.dy = y; 
    screen.width = w;
    screen.height = h;

    ioctl(fd_framebuffer, FB_REFRESH, &screen);
}

void display_refresh_all()
{
	display_refresh(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void display_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{	
	// Variable calculations
	uint16_t start_pos     = SCREEN_WIDTH * y + x;
	uint16_t width_offset  = start_pos    + w;
	//uint16_t heigth_offset = start_pos    + SCREEN_WIDTH * h;  // Not used
	//uint16_t end_pos       = width_offset + SCREEN_WIDTH * h;  // Not used

	// Draw square
	for (int i = 0; i < h; i++) {

		uint16_t row_offset = i * SCREEN_WIDTH;
		for (int j = start_pos; j < width_offset; j++) 
			map[j + row_offset] = color;
	}

	// Refresh the new part of the screen
    display_refresh(x, y, w, h);
}

void display_draw_frame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t frame_thickness, uint16_t color)
{
	// Variable calculations
	uint16_t start_pos     = (SCREEN_WIDTH) * y + x;
	uint16_t width_offset  = start_pos    + w;
	
	////////////////////////////
	// Draw horizontal frames //	
	////////////////////////////

	// First horizontal line
	for (int i = 0; i < frame_thickness; ++i){	
		uint16_t row_offset = i*(SCREEN_WIDTH);

		for (int j = start_pos + row_offset; j < width_offset + row_offset; j++) 
			map[j] = color;
	}

	// Second horizontal line
	for (int i = 0; i < frame_thickness; ++i){	
		uint16_t row_offset = i*(SCREEN_WIDTH);
	
		for (int j = start_pos + row_offset; j < width_offset + row_offset; j++) 
			map[j+(SCREEN_WIDTH)*h] = color;
	}
 	

	//////////////////////////
	// Draw vertical frames //
	//////////////////////////

	// First vertical line
	for(int i = 0; i <= h; i++){ 
		uint16_t row_offset = i*(SCREEN_WIDTH);

		// Draw short horisontal lines (repeats with the outer for-loop)
		for (int j = start_pos + row_offset; j < start_pos + row_offset + frame_thickness; j++) 
			map[j] = color;
    }

    // Second vertical line
    for(int i = 0; i <= h; i++){ 
		uint16_t row_offset = i*(SCREEN_WIDTH);

		// Draw short horisontal lines (repeats with the outer for-loop)
		for (int j = start_pos + w + row_offset; j < start_pos + w + row_offset + frame_thickness; j++) 
			map[j] = color;
    }

	// Refresh the full screen
	display_refresh_all();

}


void display_border(){
	//Vertical border
	for(int i = 1; i < 26;i++){
		//display_draw_rect(0,i*8,8,8,WHITE);
		display_draw_rect(8,i*8,8,8,WHITE);
		display_draw_rect(38*8,i*8,8,8,WHITE);
	}
	//Horizontal borders
	for(int j = 1; j < 39;j++){
		display_draw_rect(j*8,0,8,8,WHITE);
		display_draw_rect(j*8,25*8,8,8,WHITE);
	}
}


void display_set_background(uint16_t color) {
	display_draw_rect(0,0, SCREEN_WIDTH, SCREEN_HEIGHT, color);
}

void display_close()
{
	munmap(map, SCREEN_SIZE);
	close(fd_framebuffer);
}