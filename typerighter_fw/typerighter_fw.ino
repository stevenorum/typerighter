// KEYBOARD
#include "USBHost_t36.h"
USBHost myusb;
KeyboardController keyboard1(myusb);
int user_axis[64];
uint32_t buttons_prev = 0;
USBDriver *drivers[] = {&keyboard1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"KB1"};
bool driver_active[CNT_DEVICES] = {false};
bool show_changed_only = false;

// DISPLAY
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
// any pins can be used
#define SHARP_SCK  13
#define SHARP_MOSI 11
#define SHARP_SS   10
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 144, 168);
#define BLACK 0
#define WHITE 1

// TEXT
#define TEXT_BUFFER_SIZE 8192
char textBuffer[TEXT_BUFFER_SIZE];
int textCursor = 0;

// EEPROM
// https://learn.sparkfun.com/tutorials/reading-and-writing-serial-eeproms#arduino-hardware-hookup-
//#define EEPROM_ADR 0x54
//#define MAX_I2C_WRITE 16
//byte tempStore[MAX_I2C_WRITE];

void setup()
{
//  Wire.begin();
//  Wire.setClock(400000);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("\n\nUSB Host Testing");
  Serial.println(sizeof(USBHub), DEC);
  myusb.begin();
  keyboard1.attachPress(OnPress);
  display.begin();
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setRotation(0);
  // Rotations:
  // 0: pins are bottom edge of screen
  // 1: pins are right edge of screen
  // 2: pins are top edge of screen
  // 3: pins are left edge of screen
  // Text size 3: 8 wide, 7 tall
  // Text size 2: 12 wide, 10.5 tall
  // Text size 1: 24 wide, 21 tall
  // Auto hard-wraps, will write off the bottom of the screen if you don't handle that nicely.
  display.setTextColor(BLACK);
  display.clearDisplay();
}

void loop()
{
  myusb.Task();
  // TODO: Simplify this given that there's only ever going to be one USB device, and we know it's a keyboard.
  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        Serial.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);

        // Note: with some keyboards there is an issue that they don't output in boot protocol mode
        // and may not work.  The above code can try to force the keyboard into boot mode, but there
        // are issues with doing this blindly with combo devices like wireless keyboard/mouse, which
        // may cause the mouse to not work.  Note: the above id is in the builtin list of
        // vendor IDs that are already forced
        if (drivers[i] == &keyboard1) {
          if (keyboard1.idVendor() == 0x04D9) {
            Serial.println("Gigabyte vendor: force boot protocol");
            // Gigabyte keyboard
            keyboard1.forceBootProtocol();
          }
        }
      }
    }
  }
}

void printToScreen(char msg[])
{
  display.print(msg);
  display.refresh();  
}
void printToScreen(char msg)
{
  display.print(msg);
  display.refresh();  
}

void writeBufferToScreen() {
  display.clearDisplay();
  display.setCursor(0,0);
  for (int i = 0; i < textCursor; i++) {
    display.print(textBuffer[i]);  
  }
  display.refresh();
  //display.print(msg);
}

void backspace() {
    if (0 >= textCursor) {
      // buffer empty
      // pass  
    } else {
      textBuffer[textCursor-1] = (char) 0;
      textCursor--;
    }
  }

void appendCharacter(char c) {
    if (TEXT_BUFFER_SIZE <= textCursor) {
      // buffer full
      // pass
    } else {
      textBuffer[textCursor] = c;
      textCursor++;
    }
  }

void keyPressed(int key) {
  switch (key) {
    case 127: backspace(); break;
    default: appendCharacter((char) key); break;
  }
  writeBufferToScreen();
}

void OnPress(int key)
{
  Serial.print("key '");
  switch (key) {
    case KEYD_UP       : Serial.print("UP"); break;
    case KEYD_DOWN    : Serial.print("DN"); break;
    case KEYD_LEFT     : Serial.print("LEFT"); break;
    case KEYD_RIGHT   : Serial.print("RIGHT"); break;
    case KEYD_INSERT   : Serial.print("Ins"); break;
    case KEYD_DELETE   : Serial.print("Del"); break;
    case KEYD_PAGE_UP  : Serial.print("PUP"); break;
    case KEYD_PAGE_DOWN: Serial.print("PDN"); break;
    case KEYD_HOME     : Serial.print("HOME"); break;
    case KEYD_END      : Serial.print("END"); break;
    case KEYD_F1       : Serial.print("F1"); break;
    case KEYD_F2       : Serial.print("F2"); break;
    case KEYD_F3       : Serial.print("F3"); break;
    case KEYD_F4       : Serial.print("F4"); break;
    case KEYD_F5       : Serial.print("F5"); break;
    case KEYD_F6       : Serial.print("F6"); break;
    case KEYD_F7       : Serial.print("F7"); break;
    case KEYD_F8       : Serial.print("F8"); break;
    case KEYD_F9       : Serial.print("F9"); break;
    case KEYD_F10      : Serial.print("F10"); break;
    case KEYD_F11      : Serial.print("F11"); break;
    case KEYD_F12      : Serial.print("F12"); break;
    default: Serial.print((char)key);Serial.print(" (");Serial.print((int)key);Serial.print(")"); keyPressed(key); break;
  }
  Serial.print("'  ");
  Serial.print(key);
  Serial.print(" MOD: ");
  if (keyboard1) {
    Serial.print(keyboard1.getModifiers(), HEX);
    Serial.print(" OEM: ");
    Serial.print(keyboard1.getOemKey(), HEX);
    Serial.print(" LEDS: ");
    Serial.println(keyboard1.LEDS(), HEX);
  }
}

/*void writeEEPROMPage(long eeAddress)
{

  Wire.beginTransmission(EEPROM_ADR);

  Wire.write((int)(eeAddress >> 8)); // MSB
  Wire.write((int)(eeAddress & 0xFF)); // LSB

  //Write bytes to EEPROM
  for (byte x = 0 ; x < MAX_I2C_WRITE ; x++) {
    Wire.write(tempStore[x]); //Write the data
  }
  Wire.endTransmission(); //Send stop condition
}*/
