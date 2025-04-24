/*
 * MsgUtils.c
 *
 *  Created on: Jun 5, 2023
 *      Author: Eric
 */

 #include "MsgUtils.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 
 static int lastKeyCounter = 0;
 static u8 lastKeyEntered = 0;
 static u8 charToPrint = 0;
 u8 ChooseChar(u8 key)
 {
	 if (key == '1')
	 {
		 // Print 1 only
		 charToPrint = '1';
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '2')
	 {
		 if (lastKeyEntered != '2')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 4 == 0)
			 charToPrint = 'A';
		 else if (lastKeyCounter % 4 == 1)
			 charToPrint = 'B';
		 else if (lastKeyCounter % 4 == 2)
			 charToPrint = 'C';
		 else if (lastKeyCounter % 4 == 3)
			 charToPrint = '2';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '3')
	 {
		 if (lastKeyEntered != '3')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 4 == 0)
			 charToPrint = 'D';
		 else if (lastKeyCounter % 4 == 1)
			 charToPrint = 'E';
		 else if (lastKeyCounter % 4 == 2)
			 charToPrint = 'F';
		 else if (lastKeyCounter % 4 == 3)
			 charToPrint = '3';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '4')
	 {
		 if (lastKeyEntered != '4')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 4 == 0)
			 charToPrint = 'G';
		 else if (lastKeyCounter % 4 == 1)
			 charToPrint = 'H';
		 else if (lastKeyCounter % 4 == 2)
			 charToPrint = 'I';
		 else if (lastKeyCounter % 4 == 3)
			 charToPrint = '4';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '5')
	 {
		 if (lastKeyEntered != '5')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 4 == 0)
			 charToPrint = 'J';
		 else if (lastKeyCounter % 4 == 1)
			 charToPrint = 'K';
		 else if (lastKeyCounter % 4 == 2)
			 charToPrint = 'L';
		 else if (lastKeyCounter % 4 == 3)
			 charToPrint = '5';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '6')
	 {
		 if (lastKeyEntered != '6')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 4 == 0)
			 charToPrint = 'M';
		 else if (lastKeyCounter % 4 == 1)
			 charToPrint = 'N';
		 else if (lastKeyCounter % 4 == 2)
			 charToPrint = 'O';
		 else if (lastKeyCounter % 4 == 3)
			 charToPrint = '6';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '7')
	 {
		 if (lastKeyEntered != '7')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 5 == 0)
			 charToPrint = 'P';
		 else if (lastKeyCounter % 5 == 1)
			 charToPrint = 'Q';
		 else if (lastKeyCounter % 5 == 2)
			 charToPrint = 'R';
		 else if (lastKeyCounter % 5 == 3)
			 charToPrint = 'S';
		 else if (lastKeyCounter % 5 == 4)
			 charToPrint = '7';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '8')
	 {
		 if (lastKeyEntered != '8')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 4 == 0)
			 charToPrint = 'T';
		 else if (lastKeyCounter % 4 == 1)
			 charToPrint = 'U';
		 else if (lastKeyCounter % 4 == 2)
			 charToPrint = 'V';
		 else if (lastKeyCounter % 4 == 3)
			 charToPrint = '8';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '9')
	 {
		 if (lastKeyEntered != '9')
		 {
			 lastKeyCounter = 0;
		 }
 
		 if (lastKeyCounter % 5 == 0)
			 charToPrint = 'W';
		 else if (lastKeyCounter % 5 == 1)
			 charToPrint = 'X';
		 else if (lastKeyCounter % 5 == 2)
			 charToPrint = 'Y';
		 else if (lastKeyCounter % 5 == 3)
			 charToPrint = 'Z';
		 else if (lastKeyCounter % 5 == 4)
			 charToPrint = '9';
		 else
		 {
		 }
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
	 else if (key == '0')
	 {
		 // Print 0 only
		 charToPrint = '0';
 
		 lastKeyCounter++;
		 lastKeyEntered = key;
	 }
 
	 // if (key == 'A')
	 // {
	 // 	// Send to display and update cursor position.
	 // 	PmodOLEDrpg_AddCharacterToScreen(charToPrint, 1);
	 // }
	 // else if (key == 'B')
	 // {
	 // 	// Send to display a blank character, but do not update cursor position.
	 // 	PmodOLEDrpg_AddCharacterToScreen(' ', 0);
	 // }
	 // else
	 // {
	 // 	// Send to display, but do not update cursor position.
	 // 	PmodOLEDrpg_AddCharacterToScreen(charToPrint, 0);
	 // }
 
	 return charToPrint;
 }
 
 void AddCharToBuffer(u8 *buf, u8 ch)
 {
	 strncat(buf, &ch, 1);
 }
 
 void DeleteCharFromBuffer(u8 *buf)
 {
	 int size = strlen(buf);
	 buf[size - 1] = '\0';
 }
 
 