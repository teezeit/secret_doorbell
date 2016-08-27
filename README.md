This is an Arduino Project from 2011

Arduino Inputs:
The arduino connects to the doorbell voltage
The arduino has a two buttons
    Button 1: program pattern
    Button 2: Show current pattern

Arduino Outputs:
A relais to shortcircuit the main door
A motor to open the door to the flat
Green diode to show correct pattern
Red diode to show incorrect pattern or to blink the current pattern or to show programming mode



HOW TO USE:

1.  i)      Push programming button
    ii)     programming mode diode blinks on
    iii)    ring a secret pattern (max 5 rings allowed)
2. Arduino saves this pattern
3. Ring the doorbell with the secret pattern
4. Arduino does a pattern matching analysis
5. Arduino blinks red if pattern is wrong
6. Arduino blinks green if pattern is correct
    then shortcircuits the main door and you can enter
    after a short delay, arduino triggers a small motor attached to the doorlock
    of the flat to open the door.
    
ENJOY!
