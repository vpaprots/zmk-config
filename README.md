# Hardware

- Processor module: EBYTE E73-2G4M08S1C module with nrf52840/aQFN chip
- Charger: BQ25895 I2C Controlled Single Cell 5-A Fast Charger with MaxCharge
- Connectors
   - USB1: keyboard/debug/DFU (not charging)
   - USB2: charging (up-to-12V with HVDCP)
   - ...

For details, limitations, design decisions, etc. see [Hardware README](easyeda/README.md)

# Bootloader 
brew install --cask gcc-arm-embedded
pip3 install intelhex
pip3 install --user adafruit-nrfutil
PATH=$PATH:$HOME/Library/Python/3.9/bin make BOARD=ferne all
```

```

Per-Key RGB

behaviors {
   pht: positional_hold_tap {
      compatible = "zmk,behavior-rgb-tap";
      #binding-cells = <2>;
      flavor = "hold-preferred";
      animation = &
      bindings = <&kp>, <&kp>;
      hold-trigger-key-positions = <1>;    // <---[[the W key]]
   };
};

- on/off (simplest)
- on/off dampened
- 