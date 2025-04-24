/*
 * PmodOLEDrgb_Interface.h
 *
 *  Created on: May 31, 2023
 *      Author: Eric
 */

#ifndef SRC_PMODOLEDRGB_INTERFACE_H_
#define SRC_PMODOLEDRGB_INTERFACE_H_

#include "PmodOLEDrgb.h"
#include "xparameters.h"

// Initialized PmodOLEDrpg
void PmodOLEDrgb_Initialize();

// Adds a character to the PmodOLEDrgb
void PmodOLEDrgb_AddCharacterToScreen(u8 ch, int update_idx);

// Adds a buffer of text to the PmodOLEDrgb
void PmodOLEDrgb_AddBufferToScreen(u8 *buf);

// Clears PmodOLEDrgb display.
void PmodOLEDrgb_ClearScreen();

// Moves cursor back to (0,0)
void PmodOLEDrgb_ResetCursor();

// Moves cursor one column to the right.
void PmodOLEDrgb_MoveCursorOneColumnRight();

// Moves cursor one row down.
void PmodOLEDrgb_MoveCursorOneRowDown();

// Moves cursor to specified location; clamps if it's off screen.
void PmodOLEDrgb_MoveCursorTo(u8 x, u8 y);


#endif /* SRC_PMODOLEDRGB_INTERFACE_H_ */
