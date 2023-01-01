/*

                        Teensy LC DAC0
                    Vernon Billinglsey c2023

                Setup the DAC on the Teensy LC in the none 
                buffered mode.
                              
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

    The above copyright notice and this permission
    notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE

                                Teensy LC
                            ---------------
                   GND  --|                 |-- Vin 3.6 to 5.5 volts
         MOSI1 RX1  B16 --| D0              |-- GND
         MISO1 TX1  B17 --| D1              |-- 3.3v 250 mA
                    D0  --| D2          D23 |-- C2  A9
               PWM  A1  --| D3          D22 |-- C1  A8
               PWM  A2  --| D4          D21 |-- D6  A7  RX5
                    D7  --| D5          D20 |-- D5  A6  TX5
               CS1  D4  --| D6          D19 |-- B2  A5      SCL0
               RX3  D2  --| D7          D18 |-- B3  A4      SDA0
               TX3  D3  --| D8          D17 |-- B1  A3  TX4 SDA1
               RX2  C3  --| D9          D16 |-- B0  A2  RX4 SCL1
           CS0 RX2  C4  --| D10         D15 |-- C0  A1  RX3 S/PDIF IN
              MOSI0 C6  --| D11         D14 |-- D1  A0  TX3 S/PDIF OUT
              MISO0 C7  --| D12         D13 |-- C5  LED SCK
                             A  B  C  D  E
                            ---------------
                            A   Vin Voltage
                            B   3.3 Volts
                            C   GND
                            D   Program
                            E   DAC Out

                 Pins:
                 Teensy   Function
                 0
                 1
                 2
                 3
                 4
                 5
                 6
                 7
                 8
                 9
                 10
                 11
                 12
                 13
                 14
                 15
                 15
                 17
                 18
                 19
                 20
                 21
                 22
                 23
                 A
                 B
                 C
                 D
                 E        DAC Output

*/
#include <elapsedMillis.h>

/************************* Defines ********************************/
#define DEBUG 0

#if DEBUG == 1
#define dprint(expression) Serial.print("# "); Serial.print( #expression ); Serial.print( ": " ); Serial.println( expression )
#define dshow(expression) Serial.println( expression )
#else
#define dprint(expression)
#define dshow(expression)
#endif

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define LED 3

/************************** Variables *****************************/
uint16_t count;


/**************************  Functions ****************************/
void dac_begin() {
  /* Asign a clock to the DAC */
  SIM_SCGC6 |= (1 << 31);
  /* Clear the dac register */
  DAC0_C0 = 0;
  /* Reference voltage to 3.3 vdd DACREF_2 */
  DAC0_C0 |= (1 << 6);
  /* Enable the DAC DACEN */
  DAC0_C0 |= (1 << 7);
}

inline void dac_write(uint16_t val) {
  DAC0_DAT0H = val >> 8;
  DAC0_DAT0L = val & 0x00ff;
}

/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  dac_begin();

  /************************* Setup Pins ***************************/


}/**************************  End Setup **************************/

elapsedMicros wait;

/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {
  /* Increment the count */
  count += 16;
  /* Send the count to the DAC */
  dac_write(count);
  /* Delay a little bit */
  while ( wait < 10) {}
  wait = 0;


}/*************************** End Loop *****************************/
