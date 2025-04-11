// BOOL(name, register, bit)
// INT (name, register, startbit, endbit, offset, multiplier)

#undef BQ_CFG
#define BQ_CFG(extra) \
INT(batlowv, 6, 1, 1, 2800, 200, extra) \
INT(iprechg, 5, 4, 7, 64, 64, extra) \
INT(ichg, 4, 0, 6, 0, 64, extra) \
INT(iterm, 5, 0, 3, 64, 64, extra) \
INT(sys_min, 3, 1, 3, 3000, 100, extra) \
BOOL(otg_config, 3, 5, extra) \
BOOL(ico_en, 2, 4, extra) \
BOOL(hvdcp_en, 2, 3, extra) \
BOOL(maxc_en, 2, 2, extra)

#undef BQ_ADC
#define BQ_ADC(extra) \
INTA(batv, 0xE, 0, 6, 2304, 20, extra) \
INTA(sysv, 0xF, 0, 6, 2304, 20, extra) \
INTA(vbusv, 0x11, 0, 6, 2600, 100, extra) \
INTA(ichgr, 0x12, 0, 6, 0, 50, extra)
// INT(tspct:   ADC conversion of TS Voltage (TS) as percentage of REGN

#undef BQ_ACTION
#define BQ_ACTION(extra) \
BOOLF(conv_start, 2, 7, extra) \
BOOLF(wd_rst, 3, 6, extra) \
BOOLF(batfet_dis, 9, 5, extra) \
BOOLF(reg_rst, 0x14, 7, extra)

// // Reg0 (R/W):
// - EN_HIZ:  Enable HIZ Mode (disabled)
// - EN_ILIM: Enable ILIM Pin (enabled)
// - IINLIM:  Input Current Limit 0.1A-3.5A (0.5A, changed automaticallly after input source type detection is completed)
// Reg1 (R/W)
// - BHOT:      Boost Mode Hot Temperature Monitor Threshold (34.75%)
// - BCOLD:     Boost Mode Cold Temperature Monitor Threshold (77%)
// - VINDPM_OS: Input Voltage Limit Offset 0V - 3.1V (0.5V, used to calculate VINDPM threshold)
// Reg2 (R/W)
// - CONV_START:   ADC Conversion Start Control (not active)
// - CONV_RATE:    ADC Conversion Rate Selection (one shot)
// - BOOST_FREQ:   Boost Mode Frequency Selection (500kHz)
// - ICO_EN:       Input Current Optimizer (ICO) Enable (enabled)
// - HVDCP_EN:     High Voltage DCP Enable (enabled)
// - MAXC_EN:      MaxCharge Adapter Enable (enabled)
// - FORCE_DPDM:   Force D+/D- Detection (disabled)
// - AUTO_DPDM_EN: Automatic D+/D- Detection Enable when VBUS plugged in (enabled)
// Reg3 (R/W)
// - BAT_LOADEN: Battery Load (IBATLOAD) Enable (disabled)
// - WD_RST:     I2C Watchdog Timer Reset (disabled)
// - OTG_CONFIG: Boost (OTG) Mode Configuration (enabled) [FIXME!]
// - CHG_CONFIG: Charge Enable Configuration (enabled, Battery charging is enabled by setting CHG_CONFIG bit, /CE pin is low and ICHG register is not 0 mA)
// - SYS_MIN:    Minimum System Voltage Limit 3V-3.7V (3.5V) (Vsys == VDDH on nrf, VDDH-VDD>=0.3V, so if VDD=3.3V, Vsys=3.6V)
// Reg4 (R/W)
// - EN_PUMPX: Current pulse control Enable (disabled)
// - ICHG:     Fast Charge Current Limit 0-5.056A (2.048A)
// Reg5 (R/W)
// - IPRECHG: Precharge Current Limit 64mA - 1024mA (128mA)
// - ITERM:   Termination Current Limit 64mA - 1024mA (256mA)
// Reg6 (R/W)
// - VREG:    Charge Voltage Limit 3.840V - 4.608V (4.208V)
// - BATLOWV: Battery Precharge to Fast Charge Threshold 2.8V-3.0V (3.0V)
// - VRECHG:  Battery Recharge Threshold Offset 0.1V-0.2V (0.1V)
// Reg7 (R/W)
// - EN_TERM:   Charging Termination Enable (enabled)
// - STAT_DIS:  STAT Pin Disable (pin enabled)
// - WATCHDOG:  I2C Watchdog Timer Setting disabled-160s (40s)
// - EN_TIMER:  Charging Safety Timer Enable (enabled)
// - CHG_TIMER: Fast Charge Timer Setting 5hrs-20hrs (12hrs)
// Reg8 (R/W)
// - BAT_COMP: IR Compensation Resistor Setting 0-140mOhm (0)
// - VCLAMP:   IR Compensation Voltage Clamp 0-0.224V (0)
// - TREG:     Thermal Regulation Threshold 60-120C (120C)
// Reg9 (R/W)
// - FORCE_ICO:     Force Start Input Current Optimizer (ICO) (off)
// - TMR2X_EN:      Safety Timer Setting during DPM or Thermal Regulation (slowed down by 2X)
// - BATFET_DIS:    Force BATFET off to enable ship mode (disabled) [enables ship mode!]
// - BATFET_DLY:    BATFET turn off delay control, tSM_DLY=10s-15s (off, no delay) 
// - BATFET_RST_EN: BATFET full system reset enable (enabled)
// - PUMPX_UP:      Current pulse control voltage up enable (disabled)
// - PUMPX_DN:      Current pulse control voltage down enable	(disabled)
// RegA (R/W)
// - BOOSTV: Boost Mode Voltage Regulation 4.55V - 5.51V (5.126V)
// RegB (R)
// - VBUS_STAT: VBUS Status register
// - CHRG_STAT: Charging Status
// - PG_STAT:   Power Good Status
// - SDP_STAT:  USB Input Status
// - VSYS_STAT: VSYS Regulation Status
// RegC (R)
// - WATCHDOG_FAULT: Watchdog Fault Status
// - BOOST_FAULT:    Boost Mode Fault Status
// - CHRG_FAULT:     Charge Fault Status
// - BAT_FAULT:      Battery Fault Status
// - NTC_FAULT:      NTC Fault Status
// RegD (R/W)
// - FORCE_VINDPM: VINDPM Threshold Setting Method (relative)
// - VINDPM:       Absolute VINDPM Threshold 3.9V - 15.3V (4.4V)
// RegE (R)
// - THERM_STAT: Thermal Regulation Status
// - BATV:       ADC conversion of Battery Voltage (VBAT)
// RegF (R)
// - SYSV: ADC conversion of System Voltage (VSYS)
// Reg10 (R)
// - TSPCT: ADC conversion of TS Voltage (TS) as percentage of REGN
// Reg11 (R)
// - VBUS_GD: VBUS Good Status
// - VBUSV:   ADC conversion of VBUS voltage (VBUS) [good if 12V!]
// Reg12 (R)
// - ICHGR: ADC conversion of Charge Current (IBAT) when VBAT > VBATSHORT
// Reg13 (R)
// - VDPM_STAT: VINDPM Status
// - IDPM_STAT: IINDPM Status
// - IDPM_LIM:  Input Current Limit in effect while Input Current Optimizer
// Reg14(R/W R)
// - REG_RST: Register Reset (0)
// - ICO_OPTIMIZED: Input Current Optimizer (ICO) Status
// - PN:            Device Configuration
// - TS_PROFILE:    Temperature Profile
// - DEV_REV:       Device Revision