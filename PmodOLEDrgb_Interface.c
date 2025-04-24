/*
 * PmodOLEDrgp_Interface.c
 *
 *  Created on: May 31, 2023
 *      Author: Eric
 */

#include "PmodOLEDrgb_Interface.h"

// Static Variables
static PmodOLEDrgb oledrgb;				// Represents PmodOLEDrgb device.

static char display[8][12] = {0};		// Represents display of PmodOLEDrgb.

static int rowInd = 0, colInd = 0;		// Represents display cursor's current position.
static u8 rgbUserFont[] = {
   0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00, // 0x00
   0x0E, 0x1F, 0x15, 0x1F, 0x17, 0x10, 0x1F, 0x0E, // 0x01
   0x00, 0x1F, 0x11, 0x00, 0x00, 0x11, 0x1F, 0x00, // 0x02
   0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00, // 0x03
   0x07, 0x0C, 0xFA, 0x2F, 0x2F, 0xFA, 0x0C, 0x07  // 0x04
}; // This table defines 5 user characters, although only one is used

void PmodOLEDrgb_Initialize()
{
	// Start the PmodOLEDrgb.
	OLEDrgb_begin(&oledrgb,
					XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR,
					XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR);

	// Define the user definable characters
	char ch;
	for (ch = 0; ch < 5; ch++) {
		OLEDrgb_DefUserChar(&oledrgb, ch, &rgbUserFont[ch * 8]);
	}

	// Clear the screen
	OLEDrgb_Clear(&oledrgb);

	// Set the cursor to (0,0) (i.e. initial rowInd and colInd)
	OLEDrgb_SetCursor(&oledrgb, 0, 0);

	// Set rowInd and colInd to (0,0)
	rowInd = 0;
	colInd = 0;
}

void PmodOLEDrgb_AddCharacterToScreen(u8 ch, int update_idx)
{
//	// add the new key to display
//	display[rowInd][colInd] = (char)ch;
//
//	// clear screen
//	OLEDrgb_Clear(&oledrgb);
//	OLEDrgb_SetCursor(&oledrgb, 0, 0);
//
//	// print the board
//	for(int i=0; i < 8; i++) {
//		for(int j=0; j < 12; j++) {
//			OLEDrgb_PutChar(&oledrgb, display[i][j]);
//		}
//	}
//
//	// preparing index for the next character
//	// if reached last column
//	if (update_idx == 1)
//	{
//		if (colInd >= 11) {
//
//			colInd = 0;
//			// if reached the last row
//			if(rowInd >= 7) {
//				// row index doesn't change
//				// move row 1-7 up
//				for(int i=0; i < 7; i++) {
//					for(int j=0; j < 12; j++) {
//						display[i][j] = display[i+1][j];
//					}
//				}
//
//				// clear last row
//				for(int j=0; j < 12; j++) {
//					display[7][j] = ' ';
//				}
//
//			}
//			else rowInd++;
//		}
//		else colInd++;
//	}

	xil_printf("(x = %d, y = %d)\r\n", colInd, rowInd);
	OLEDrgb_SetCursor(&oledrgb, colInd, rowInd);

	// Add key to the display
	OLEDrgb_DrawGlyph(&oledrgb, ch);

	// If update_idx == 0, don't update rowInd and colInd.
	// Otherwise, update rowInd and colInd.
	// TODO: Add scrolling.
	if (update_idx == 1)
	{
		if (colInd >= 11) {

			colInd = 0;
			// if reached the last row
			if(rowInd >= 7) {

				// Move Row 0-6 up
				OLEDrgb_Copy(&oledrgb, 0, 0, 12, 6, 0, 0);

				// Clear Row 7
				OLEDrgb_SetCursor(&oledrgb, 0, 7);
				OLEDrgb_PutString(&oledrgb, "            ");

				rowInd = 7;
			}
			else rowInd++;
		}
		else colInd++;
	}
}

void PmodOLEDrgb_AddBufferToScreen(u8 *buf)
{
//	int idx = 0;
//	while(buf[idx] != '\0')
//	{
//		PmodOLEDrgb_AddCharacterToScreen(buf[idx], 1);
//		idx++;
//	}

	// Use PutString to add buf to screen.
	OLEDrgb_PutString(&oledrgb, buf);

	// Use GetCursor to get the current cursor position, so we can update
	// our internal rowInd and colInd variables.
	int oled_cursor_x = 0;
	int oled_cursor_y = 0;
	OLEDrgb_GetCursor(&oledrgb, &oled_cursor_x, &oled_cursor_y);
	colInd = oled_cursor_x;
	rowInd = oled_cursor_y;
	xil_printf("AddBuf: (x = %d, y = %d)\r\n", colInd, rowInd);
}

void PmodOLEDrgb_ClearScreen()
{
	OLEDrgb_Clear(&oledrgb);
	OLEDrgb_SetCursor(&oledrgb, 0, 0);
	PmodOLEDrgb_ResetCursor();
//	for(int i=0; i < 8; i++)
//	{
//		for(int j=0; j < 12; j++)
//		{
//			display[i][j] = ' ';
//		}
//	}
}

void PmodOLEDrgb_ResetCursor()
{
	rowInd = 0;
	colInd = 0;
	OLEDrgb_SetCursor(&oledrgb, 0, 0);
}

void PmodOLEDrgb_MoveCursorOneColumnRight()
{
	if (colInd >= 7)
	{
		colInd = 7;
	}
	else
	{
		colInd++;
	}
}

void PmodOLEDrgb_MoveCursorOneRowDown()
{
	if (rowInd >= 11)
	{
		rowInd = 11;
	}
	else
	{
		rowInd++;
	}
}

void PmodOLEDrgb_MoveCursorTo(u8 x, u8 y)
{
	// Update colInd
	if (x >= 7)
	{
		colInd = 7;
	}
	else
	{
		colInd = x;
	}

	// Update rowInd
	if (y >= 11)
	{
		rowInd = 11;
	}
	else
	{
		rowInd = y;
	}
}
