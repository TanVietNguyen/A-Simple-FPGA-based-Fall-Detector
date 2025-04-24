// Pmod Interfaces/Drivers
#include "PmodKYPD.h"
#include "PmodACL2.h"
#include "xgpio.h"
#include "PmodBLE_Interface.h"
#include "PmodOLEDrgb.h"
// Other
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xil_printf.h"
// *************** GLOBAL VARIABLES *************** //
PmodKYPD myDevice; // PmodKYPD
#define DEFAULT_KEYTABLE "0FED789C456B123A" // PmodKYPD
u32 GPIO_Address = XPAR_PMODACL2_0_AXI_LITE_GPIO_BASEADDR;
u32 SPI_Address = XPAR_PMODACL2_0_AXI_LITE_SPI_BASEADDR;
PmodACL2 pmodACL2_device;
ACL2_SamplePacket sample;
float Xdata_g, Ydata_g, Zdata_g;

u8 ThisDeviceAddress[13] = {0}; // PmodBLE
u8 DeviceAddressToConnect[13] = {0}; // PmodBLE
u8 MsgToSendBuf[96] = {0}; // PmodBLE/PmodOLEDrgb
u8 MsgRecvBuf[96] = {0}; // PmodBLE/PmodOLEDrgb
PmodOLEDrgb oledrgb; // PmodOLEDrgb
u8 rgbUserFont[] = {
   0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00, // 0x00
   0x0E, 0x1F, 0x15, 0x1F, 0x17, 0x10, 0x1F, 0x0E, // 0x01
   0x00, 0x1F, 0x11, 0x00, 0x00, 0x11, 0x1F, 0x00, // 0x02
   0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00, // 0x03
   0x07, 0x0C, 0xFA, 0x2F, 0x2F, 0xFA, 0x0C, 0x07  // 0x04
}; // This table defines 5 user characters, although only one is used
char ch;
int x_current_cursor, y_current_cursor;

// *************** Function Definitions ********************* //
void DemoInitialize();
void DemoRun();
void DemoCleanup();
void EnableCaches();
void DisableCaches();

int main(void)
{
xil_printf("Test 1 \r\n");
DemoInitialize();
DemoRun();
DemoCleanup();
return 0;
}

void DemoInitialize()
{
EnableCaches();

// 1. Setup all the Pmods connected to the board.
// a) PmodKYPD
// b) PmodBLE
// c) PmodAMP2
// d) PmodOLEDrgb
// e) PmodACL2

// for PmodKYPD
KYPD_begin(&myDevice, XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR);
KYPD_loadKeyTable(&myDevice, (u8 *)DEFAULT_KEYTABLE);

// for PmodBLE
PmodBLE_Initialize();
xil_printf("Initialization done\r\n");
//For PmodOLEDrgb
    OLEDrgb_begin(&oledrgb, XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR,
         XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR);
    for (ch = 0; ch < 5; ch++) {
      OLEDrgb_DefUserChar(&oledrgb, ch, &rgbUserFont[ch * 8]);
    }


// 2. From the PmodBLE, get the device address. From there:
// for PmodACL2
ACL2_begin(&pmodACL2_device, GPIO_Address, SPI_Address);
ACL2_reset(&pmodACL2_device);
usleep(1000);
    ACL2_configure(&pmodACL2_device);
    // Set this range of 2 to detect slower motions
    ACL2_setRange(&pmodACL2_device, 2);

// 2. From the PmodBT2, get the device address. From there:
// a) Store the device address in an internal buffer
//   in case we need it later.
// b) Display the device address to the PmodOLEDrgb display.
PmodBLE_GetDeviceAddress(ThisDeviceAddress);

xil_printf("Address Recieved\r\n");
OLEDrgb_Clear(&oledrgb);
OLEDrgb_SetCursor(&oledrgb, 0, 0);
OLEDrgb_PutString(&oledrgb, "This Device\nAddress:   ");
}

#define STATE_NOT_CONNECTED_INIT 0
#define STATE_NOT_CONNECTED_INPUT 1
#define STATE_CONNECTING 2
#define STATE_CONNECTED_INIT 3
#define FALL_MONITORING_MODE 4
#define HELP_SEND_MODE 5
#define MESSAGE_EOL "\x03" // End of Text Character
u8 cur_state = 0;
void DemoRun()
{

// for PmodKYPD
u16 keystate;
XStatus status, last_status = KYPD_NO_KEY;
u8 key, last_key = 'x';
Xil_Out32(myDevice.GPIO_addr, 0xF);

while (1)
{
// Get input from keypad
keystate = KYPD_getKeyStates(&myDevice);
status = KYPD_getKeyPressed(&myDevice, keystate, &key);

// Print key detect if a new key is pressed or if status has changed
if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key)) {
xil_printf("Key Pressed: %c\r\n", (char) key);
//   last_key = key;
}

// State Machine
if (cur_state == STATE_NOT_CONNECTED_INIT)
{
// Clear MsgToSend, MsgRecv, and DeviceAddressToConnect buffers.
memset(MsgToSendBuf, '\0', sizeof(MsgToSendBuf));
memset(MsgRecvBuf, '\0', sizeof(MsgRecvBuf));
memset(DeviceAddressToConnect, '\0', sizeof(DeviceAddressToConnect));



// Display this device's MAC address to the screen.
xil_printf(ThisDeviceAddress);
OLEDrgb_GetCursor(&oledrgb, &x_current_cursor, &y_current_cursor);
OLEDrgb_SetCursor(&oledrgb, 0, y_current_cursor + 2);
OLEDrgb_PutString(&oledrgb, ThisDeviceAddress);
xil_printf("\r\n");

// Set state to STATE_NOT_CONNECTED_INPUT
cur_state = STATE_NOT_CONNECTED_INPUT;
}
else if (cur_state == STATE_NOT_CONNECTED_INPUT)
{
// While buffer is not filled, add key pressed to DeviceAddressToConnect
if (strlen(DeviceAddressToConnect) < 12)
{
if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key))
{
// Print character to screen.
xil_printf(key);
// Device is still connected. Sending coordinate of the device
// TO-DO: for now, just print the coordinates to sdtout.
sample = ACL2_getSample(&pmodACL2_device);
Xdata_g = ACL2_DataToG(&pmodACL2_device, sample.XData);
Ydata_g = ACL2_DataToG(&pmodACL2_device, sample.YData);
Zdata_g = ACL2_DataToG(&pmodACL2_device, sample.ZData);
xil_printf("X: %d, Y: %d, Z: %d\n", Xdata_g, Ydata_g, Zdata_g);
xil_printf("\r\n");
sleep(1);

// Add key to DeviceAddressToConnect
strncat(DeviceAddressToConnect, &key, 1);
sleep(1);
}
}
else
{
// Set cur_state to STATE_CONNECTING
cur_state = STATE_CONNECTING;

// Display Status Message
xil_printf("\r\n");
xil_printf("Connecting..\r\n");
OLEDrgb_Clear(&oledrgb);
OLEDrgb_SetCursor(&oledrgb, 0, 0);
OLEDrgb_PutString(&oledrgb, "Connecting..");
}

if (PmodBLE_IsConnected() == 1)
{
// Another PmodBLE device connected with this one.
// Skip to STATE_CONNECTED_INIT
cur_state = STATE_CONNECTED_INIT;
}
}
else if (cur_state == STATE_CONNECTING)
{
// Send connect command to PmodBLE and loop until status is connected.
PmodBLE_ConnectTo(DeviceAddressToConnect);

// Loop until connected.
while (PmodBLE_IsConnected() == 0)
{
}

// At this point, device is connected.
// Clear receive buffers and move to STATE_CONNECTED_INIT
PmodBLE_Flush();
cur_state = STATE_CONNECTED_INIT;
}
else if (cur_state == STATE_CONNECTED_INIT)
{
// Clear the display
OLEDrgb_Clear(&oledrgb);
OLEDrgb_SetCursor(&oledrgb, 0, 0);
OLEDrgb_PutString(&oledrgb, "Connected!");
xil_printf("\r\nConnected!\r\n");

// Clear the display after 1 second
sleep(1);
OLEDrgb_Clear(&oledrgb);
// Clear receive buffers.
PmodBLE_Flush();

// Set to next state
xil_printf("Fall Monitoring Mode \r\n");
OLEDrgb_Clear(&oledrgb);
OLEDrgb_SetCursor(&oledrgb, 0, 0);
OLEDrgb_PutString(&oledrgb, "Fall       \nMonitoring \nMode       ");
cur_state = FALL_MONITORING_MODE;
}
else if (cur_state == FALL_MONITORING_MODE)
{

if (PmodBLE_IsConnected() == 0)
{
// Set cur_state to STATE_NOT_CONNECTED_INIT
cur_state = STATE_NOT_CONNECTED_INIT;
}
else
{
// Poll PmodBLE for message.
u8 temp[96] = {0};
int n = PmodBLE_ReceiveMessage(temp, 95);
strcat(MsgRecvBuf, temp);
if (strstr(MsgRecvBuf, MESSAGE_EOL) != NULL)
{
// Clear the screen.
xil_printf("Message Recieved is: \r\n");
OLEDrgb_Clear(&oledrgb);

// Add MsgRecvBuf to screen.
xil_printf(MsgRecvBuf);
OLEDrgb_PutString(&oledrgb, MsgRecvBuf);
xil_printf("\r\n");

if (MsgRecvBuf[0] == '1') {
xil_printf("HELP is requested! \r\n");
OLEDrgb_Clear(&oledrgb);
OLEDrgb_SetCursor(&oledrgb, 0, 0);
OLEDrgb_PutString(&oledrgb, "HELP is    \nrequested! ");
OLEDrgb_SetCursor(&oledrgb, 0, 1);
OLEDrgb_PutString(&oledrgb, "Press 1 if \nyou are    \nable to    \nassist now.");
// OLEDrgb_PutString(&oledrgb, "HELP is");
// OLEDrgb_SetCursor(&oledrgb, 0, 1);
// OLEDrgb_PutString(&oledrgb, "requested");
// xil_printf("Press 1 if you are able to assist now. \r\n");
// OLEDrgb_SetCursor(&oledrgb, 0, 2);
// OLEDrgb_PutString(&oledrgb, "Press 1 if");
// OLEDrgb_SetCursor(&oledrgb, 0, 3);
// OLEDrgb_PutString(&oledrgb, "you are able");
// OLEDrgb_SetCursor(&oledrgb, 0, 4);
// OLEDrgb_PutString(&oledrgb, "to assist now");

cur_state = HELP_SEND_MODE;
}

// Clear MsgRecvBuf
memset(MsgRecvBuf, '\0', sizeof(MsgRecvBuf));

// Clear Receive Buffers.
PmodBLE_Flush();
}


}
}
else if (cur_state == HELP_SEND_MODE)
{
if (PmodBLE_IsConnected() == 0)
{
// Set cur_state to STATE_NOT_CONNECTED_INIT
cur_state = STATE_NOT_CONNECTED_INIT;
}
else
{
// Device is still connected. Process key input.
if (status == KYPD_SINGLE_KEY && (status != last_status || key != last_key))
{
u8 ch = 0;
ch = ChooseChar(key);

if (key == '1')
{
// Add ch to screen w/ updating cursor.
xil_printf(ch);

// Add ch to the MsgToSend buffer.
strncat(MsgToSendBuf, &ch, 1);
strncat(MsgToSendBuf, MESSAGE_EOL, 1);

// Send MsgToSend buffer using PmodBLE_SendMessage()
PmodBLE_SendMessage(MsgToSendBuf);

// Clear the MsgToSend buffer.
memset(MsgToSendBuf, '\0', sizeof(MsgToSendBuf));

// Clear screen.
xil_printf("\r\n");
OLEDrgb_Clear(&oledrgb);
}
else if (key == 'E')
{
// Clear the MsgToSend buffer.
memset(MsgToSendBuf, '\0', sizeof(MsgToSendBuf));

// Clear screen.
xil_printf("\r\n");
OLEDrgb_Clear(&oledrgb);

// Set cur_state to STATE_RECV_MODE
cur_state = FALL_MONITORING_MODE;

// Print "Receive Mode" message, wait for 0.5 seconds, and then clear screen.
xil_printf("Back to Fall Monitoring...\r\n");
OLEDrgb_SetCursor(&oledrgb, 0, 0);
OLEDrgb_PutString(&oledrgb, "Back to    \nFall       \nMonitoring.\n..         \n");
usleep(500000);
}
else
{
// Add ch to screen w/o updating cursor.
xil_printf(ch);
}
sleep(1);

//last_key = key;
}
}
}
else
{}

// Update last_status and last_key for PmodKYPD
last_key = key;
last_status = status;
}
}

void DemoCleanup()
{
ACL2_end(&pmodACL2_device);
DisableCaches();
}

void EnableCaches()
{

#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_DCACHE
Xil_DCacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_ICACHE
Xil_ICacheDisable();
#endif
#endif
}