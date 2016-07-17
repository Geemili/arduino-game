
#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 1000

int main (void)
{
  // Set pin 5 of PORTB (aka pin 13) to output
  DDRB |= _BV(DDB5);

  while (1) {
    // Set pin 5 of PORTB to high (turn the LED on)
    PORTB |= _BV(PORTB5);
    _delay_ms(BLINK_DELAY_MS);

    // Turn the LED off
    PORTB &= ~_BV(PORTB5);
    _delay_ms(BLINK_DELAY_MS);
  }
}
