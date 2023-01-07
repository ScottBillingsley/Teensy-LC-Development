/*
                          Teensy LC MCP3202 Read
                         Vernon Billingsley  c2023

                     Setup and read the MCP3202 SPI DAC and
                     send the value to the USB Seiral port....

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
                            E   E30 A12 DAC Out

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
                 9                MCP3202
                 10     CS        1 CS
                 11     MOSI0     5 Din
                 12     MISO0     6 Dout
                 13     SCK       7 CLK
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
                 E


                 MCP3202
                ---------
          CS --|1   U   8|-- Vdd/Vref
         CH0 --|2       7|-- CLK
         CH1 --|3       6|-- Dout
         GND --|4       5|-- Din
                ---------

           Suggested dac input, datasheet page 14
                              2k2
           0 to 5 vdc -------/\/\/-------- CHx
                                    |
                                    _
                                    -   472n
                                    |
                                   GND


  Development software...
  https://github.com/ScottBillingsley/Teensy-LC-Development


*/
#include <SPI.h>


/************************* Defines ********************************/
#define DEBUG 1 /* Allow USB Serial */

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

/* MCP3202 Defines */
/* Single channel read on chaanel 0 */
#define SingleChannel0 0b10100000
/* Single channel read on chaanel 1 */
#define SingleChannel1 0b11100000
/* Differential channel read CH0 as +ve, CH1 as -ve */
#define DiffChannel0 0b00100000
/* Differential channel read CH0 as -ve, CH1 as +ve */
#define DiffChannel1 0b01100000

/* Define the Chip Select pin */
#define CS 10

/************************** Variables *****************************/
uint16_t running_average;


/**************************  Functions ****************************/
uint16_t read_mcp3202(uint8_t mode) {
  uint8_t HIGH_BYTE = 0;
  uint8_t LOW_BYTE = 0;
  uint16_t out_word = 0;
  /* Begin transaction */
  /* 1.5 mHz clock speed, datasheet page 3 */
  SPI.beginTransaction(SPISettings(1500000, MSBFIRST, SPI_MODE3));
  /* Take the chip select LOW to begin */
  digitalWrite(CS, LOW);
  /* Send 7 leading Zeros and a Strat bit, datasheet page 17 */
  LOW_BYTE = SPI.transfer(0x01);
  /* Read the HIGH byte from the '3202 */
  HIGH_BYTE = SPI.transfer(mode);
  /* Read the LOW byte */
  LOW_BYTE = SPI.transfer(0);
  /* Take the chip select HIGH */
  digitalWrite(CS, HIGH);
  /* End the transaction */
  SPI.endTransaction();
  /* Get the HIGH nibble and place it in the out word */
  out_word = (HIGH_BYTE & 0x0f) << 8;
  /* Place the LOW byte in the out word */
  out_word = out_word | LOW_BYTE;
  /* Return the out word */
  return out_word ;
}

/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /************************* Setup Pins ***************************/
  /* Pin D10 as OUTPUT for Chip Select */
  pinMode(CS, OUTPUT);
  /* Take CS High to begin */
  digitalWrite(CS, HIGH);

  /************************* Setup SPI ***************************/
  SPI.begin();


}/**************************  End Setup **************************/



/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {

  running_average = (running_average + read_mcp3202(SingleChannel0)) / 2;
  Serial.println(running_average);

  delay(5);



}/*************************** End Loop *****************************/
