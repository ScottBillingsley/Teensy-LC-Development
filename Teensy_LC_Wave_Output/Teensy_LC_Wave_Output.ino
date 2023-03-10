/*
                       Teensy LC Wave Output
                      Vernon Billingsley c2023

                  Output a wave on the built in DAC using
                  a FlexTimer for timing...

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
                 E          DAC Ouput

  Development software...

  https://github.com/ScottBillingsley/Teensy-LC-Development/tree/main/Teensy_LC_FlexTimer_Test


*/


/************************* Defines ********************************/
/* So USB Serial Output */
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

/* Set the sample rate */
#define SAMPLE_RATE 44100.0


/************************** Variables *****************************/
/* The word accumulator */
volatile uint32_t mAccu;
/* The accu word */
volatile uint32_t mWord;
/* The accu increment */
volatile uint32_t mIcnt;
/* Hold the output sample */
volatile uint16_t out_sample;

/**************************  Functions ****************************/
void flextimer_begin() {
  /* Clear TPM0_SC register */
  FTM0_SC = 0;
  /* Reset the TPM0_CNT counter */
  FTM0_CNT = 0;
  /* Set the overflow value (48000000 / Prescale) / Freq */
  /* ((48000000 / Prescale) / SAMPLE_RATE) - 1*/
  FTM0_MOD = ((48000000 / 2) / SAMPLE_RATE) - 1;
  // Set TPM0_SC register (p. 572)
  // Bits | Va1ue | Description
  //  8   |    0  | DMA: Disable DMA
  //  7   |    1  | TOF: Clear Timer Overflow Flag
  //  6   |    1  | TOIE: Enable Timer Overflow Interrupt
  //  5   |    0  | CPWMS: TPM in up counting mode
  // 4-3  |   01  | CMOD: Counter incrememnts every TPM clock
  // 2-0  |  001  | PS: Prescale = 2
  FTM0_SC = 0b011001001;
  // Set the urgency of the interrupt. Lower numbers mean higher
  // urgency (they will happen first). Acceptable values are
  // 0, 64, 128, and 192. Default is 128. We set the priority
  // (2nd byte) in the register for the FTM0 interrupt (&E000_E410)
  // to 64.
  NVIC_SET_PRIORITY(IRQ_FTM0, 64);
  // Enable the interrupt vector. In this case, we want to execute
  // the ISR (named "ftm0_isr()" for Teensy) every time TPM0
  // overflows. We set bit 17 of &E000_E100.
  NVIC_ENABLE_IRQ(IRQ_FTM0);
  // Same as: NVIC_ISER0 |= (1 << 17);
}


/******************************************************************/
/*************************** Setup ********************************/
/******************************************************************/
void setup() {
  if (DEBUG) {
    Serial.begin(115200);
  }

  /* Start the timer */
  flextimer_begin();

  analogWriteRes(12);

  /************************* Setup Pins ***************************/

  /* Get the mWord for the accum */
  mWord = (float) pow(2, 32) * 110.0 / SAMPLE_RATE;

}/**************************  End Setup **************************/

/* Timer ISR */
void ftm0_isr(void) {
  // Write a 1 to the TOF bit to clear the timer overflow flag
  FTM0_SC |= (1 << 7);
  /* Increment the accum */
  mAccu += mWord;
  /* Get the top 12 bits */
  mIcnt = (mAccu >> 20);

  /*.... Do other things, like add an envelope ....*/
  
  out_sample = mIcnt;

  /* Send to the DAC */
  analogWrite(A12, out_sample);

}


/******************************************************************/
/**************************** Loop ********************************/
/******************************************************************/
void loop() {


}/*************************** End Loop *****************************/
