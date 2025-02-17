```
 usb-pd - needs more parts (reference to app note and the two chips)
 power mux - might be more full-proof, but needs more programming for auto-detection
       - use one or the other
       - more heat management
 combine usb connectors - need to know more about usb spec, probably some sort of hub chip
       - too many moving parts and unknowns, expensive to experiment
       - dsel doesnt seem well documented 
          - i.e. use a usb mux, but will the nrf also do handshake ruining the original handshake?
          - need to clamp vbus for nrf to 5v, but, so tl431.. but only after power handshake
        - would had been better if nrf handled the entire D+/- BC1.2 handshake, but no library for non-standard connectors
        - This is all just too old, and new parts havent been well integrated yet
          - nrf should know about usb-c (need newer chips?)
          - TI should integrate usb-c into battery charger
          - BC1.2+non-standard-chargers are deprecated, probably going to get harder to get such chargers 5-10 years from now
          - but USB PD needs too many discrete, untested components
          - if you still wish USB-PD, excellent guide https://www.ti.com/lit/ta/sszt229/sszt229.pdf?ts=1730645092694
kscan at 1.8v - might still be a good idea, but.. need to select diodes with low voltage drop, unsure about current skree tails
          - level shifters and bucks/ldos for 3.3/5v for everything else? spi/i2c high-speed shifters?
ldo for screen & trackball - https://www.nklabs.com/post/2020/01/14/when-is-an-ldo-more-efficient-than-a-switching-power-converter
          - nrf forums say max 2ma when radio on (50ma total) on vdd, so be safe and provide separate source
          - need low drop out for when 3.3v->3v (cant use basic parts)
          - allows turning it off for extpower
multiple cells in series - could had used higher charge voltage at lower amps (less heat management)
          - need separate, per-cell protection (one extra chip)
          - need ldo/buck for 3.3/5v (more parts, different power requirements)
          - so many untested design modifications, might make battery life worse
- fuel cell - use built-in ADC in bq25 charger
          - needs to be programmed and curve trained.. 
          - 'just pay for someone's library'
          - fairly 'cheap' component, low power consumption, no extra pins

Note: this is written, at times, as instructions. Sometimes it is. And sometimes, I give up the intention of making this generic and just getting on with it working just for me; and the instructions are for the future, angrier version of me, when I forgot what I considered dangerous and forgot.

Note: treat batteries with respect you would give a table saw; its an extremely useful tool, but also powerful enough to take your fingers off. Whether this circuit is safe (and safe not just for the day but years) is for you to decide.

```

Freerouting.jar

https://hackaday.com/2022/10/10/lithium-ion-battery-circuitry-is-simple/


Final key positions (no screen, no trackball)

https://ryanis.cool/cosmos/beta#cm:CrMBChcSCRCAPyAnQABIABIAEgASADgvQICeAQoVEgkQgEsgJ0AASAASABIAEgA4HUAACiYSCRCAVyAnQABIABIAEgASAxCwLxIDELBfOAlAkrSG6AFIgIDkAgokEgkQgGMgJ0AASAASABIAEgMQsDsSAxCwazgMQJj4FEiAgMwNCiMSCRCAbyAnQABIABIAEgASACIDIPoLOCJAkKKG8AJIgIC0FBgAQOiFoK7wVUiskejEoXcKdgocEhgIgDAQwIACGAhAgYCYAkighYzmgLYOUEM4CAobEhYIgDAQQEC7pHBI7JWM3MD/C1ALWNACUJ4CChUSERBAQICA+AFI44m854DbD1BXUH8KA1CCAhgCIgsIswEQlgEYxgogAECrheyV0JYBSNKH1JeisQcQAxiGICIGEKwBIKALKDI4A4IBAQJIAFhAYAJoAHIMDQAAIEEgCmBacIwVeOOJ/OzQcw==

