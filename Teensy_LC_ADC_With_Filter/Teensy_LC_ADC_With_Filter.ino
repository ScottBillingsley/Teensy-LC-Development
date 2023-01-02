/*
                      Teensy LC ADC With Filter
                       Vernon Billingsley c2023

                    Read the multiple ADCs and use an integer math
                    LP filter on the results and send to the USB Serial port... 

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
    THE SOFTWARE.
                         

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
                 3        Testing pin
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
                 22         0 to 3.3 volts Input
                 23         0 to 3.3 volts Input 
                 A
                 B
                 C
                 D
                 E

*/

#include <ADC.h>
#include <ADC_util.h>

/* Create a new ADC object */
ADC *adc = new ADC();

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

/* The number of ADC channels to use */
#define ADC_MAX  2

/************************** Variables *****************************/
/* Hold the adc values */
unsigned long adc_array[ADC_MAX];
/* The order to read the ADC */
uint8_t ADC_READ[ADC_MAX] = {
  23, 22,
};
/*The ADC channel to read */
uint8_t adc_count = 0;

/*About the filter  */
/* .125 * 256 = 32 */
unsigned long alpha = 32;
/*Store the filtered sample */
unsigned long adc_filtered[ADC_MAX];
/*Store the previous filtered sample */
unsigned long f_v[ADC_MAX][3];


/**************************  Functions ****************************/
/* An integer math low pass filter for smoothing the ADC reads */
void filter(byte ary_num) {
  /*Calculate the new value */
  //f_v[ary_num][1] = (float)alpha * adc_array[ary_num] + (1 - alpha) * f_v[ary_num][0];
  f_v[ary_num][1] = (((adc_array[ary_num] << 8) * alpha) + ((256 - alpha) * ( f_v[ary_num][0] << 8))) >> 16;
  /*Store the old value */
  f_v[ary_num][0] = f_v[ary_num][1];

  /*Store the filtered value */
  adc_filtered[ary_num] = f_v[ary_num][1];
}


/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /************************* Setup Pins ***************************/
  /* Setup the ADC pins */
  for(byte x = 0; x < ADC_MAX; x++ ){
  pinMode(ADC_READ[x], INPUT_DISABLE);
  }

  /* Setup the ADC */
  adc->adc0->setAveraging(4); // set number of averages
  adc->adc0->setResolution(10); // set bits of resolution
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED);
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);

  /* Start the first conversion */
  adc->adc0->startSingleRead(ADC_READ[adc_count]);

}/**************************  End Setup **************************/



/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {

  /* Check if the ADC is finished */
  if (adc->adc0->isComplete()) {
    /* Read the sample, adc->adc0->analogRead(readPin) */
    adc_array[adc_count] = adc->adc0->readSingle();
    /* Filter the sample and store in the filtered array */
    filter(adc_count);
    /* Print the filtered sample to the Serial port */
    for(byte x = 0; x < ADC_MAX; x ++){
    Serial.print(adc_filtered[x]);
    Serial.print("\t");
    }
    Serial.println();
    /* Wait a little bit */
    delay(10);
    /* Increment the adc count */
    adc_count ++;
    if(adc_count >= ADC_MAX){
      adc_count = 0;
    }
    /* Start the next sample */
    adc->adc0->startSingleRead(ADC_READ[adc_count]);
  }

}/*************************** End Loop *****************************/
