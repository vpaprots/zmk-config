// BOOL(name, register, bit)
// INT (name, register, startbit, endbit, offset, multiplier, unit)

// reg00 
BOOL(high_impedance_mode, EN_HIZ, 0, 7);
BOOL(ilim_pin, EN_ILIM, 0, 6);
INT (current_limit, IINLIM, 0, 0, 5, linear_val(100, 50, mA));
// 100mA + 50A*current_limit

// reg01
INT (boost_hot, 1, 6, 7, boost_hot_val);
BOOL(boost_cold, 1, 5);
INT (input_voltage_limit_offset, 1, 0, 4, 0, 100);

// 	/* REG00 */
[F_EN_HIZ]		= REG_FIELD(0x00, 7, 7),
[F_EN_ILIM]		= REG_FIELD(0x00, 6, 6),
[F_IINLIM]		= REG_FIELD(0x00, 0, 5),
/* REG01 */
[F_BHOT]		= REG_FIELD(0x01, 6, 7),
[F_BCOLD]		= REG_FIELD(0x01, 5, 5),
[F_VINDPM_OFS]		= REG_FIELD(0x01, 0, 4),
/* REG02 */
[F_CONV_START]		= REG_FIELD(0x02, 7, 7),
[F_CONV_RATE]		= REG_FIELD(0x02, 6, 6),
[F_BOOSTF]		= REG_FIELD(0x02, 5, 5),
[F_ICO_EN]		= REG_FIELD(0x02, 4, 4),
[F_HVDCP_EN]		= REG_FIELD(0x02, 3, 3),  // reserved on BQ25896
[F_MAXC_EN]		= REG_FIELD(0x02, 2, 2),  // reserved on BQ25896
[F_FORCE_DPM]		= REG_FIELD(0x02, 1, 1),
[F_AUTO_DPDM_EN]	= REG_FIELD(0x02, 0, 0),
/* REG03 */
[F_BAT_LOAD_EN]		= REG_FIELD(0x03, 7, 7),
[F_WD_RST]		= REG_FIELD(0x03, 6, 6),
[F_OTG_CFG]		= REG_FIELD(0x03, 5, 5),
[F_CHG_CFG]		= REG_FIELD(0x03, 4, 4),
[F_SYSVMIN]		= REG_FIELD(0x03, 1, 3),
[F_MIN_VBAT_SEL]	= REG_FIELD(0x03, 0, 0), // BQ25896 only
/* REG04 */
[F_PUMPX_EN]		= REG_FIELD(0x04, 7, 7),
[F_ICHG]		= REG_FIELD(0x04, 0, 6),
/* REG05 */
[F_IPRECHG]		= REG_FIELD(0x05, 4, 7),
[F_ITERM]		= REG_FIELD(0x05, 0, 3),
/* REG06 */
[F_VREG]		= REG_FIELD(0x06, 2, 7),
[F_BATLOWV]		= REG_FIELD(0x06, 1, 1),
[F_VRECHG]		= REG_FIELD(0x06, 0, 0),
/* REG07 */
[F_TERM_EN]		= REG_FIELD(0x07, 7, 7),
[F_STAT_DIS]		= REG_FIELD(0x07, 6, 6),
[F_WD]			= REG_FIELD(0x07, 4, 5),
[F_TMR_EN]		= REG_FIELD(0x07, 3, 3),
[F_CHG_TMR]		= REG_FIELD(0x07, 1, 2),
[F_JEITA_ISET]		= REG_FIELD(0x07, 0, 0), // reserved on BQ25895
/* REG08 */
[F_BATCMP]		= REG_FIELD(0x08, 5, 7),
[F_VCLAMP]		= REG_FIELD(0x08, 2, 4),
[F_TREG]		= REG_FIELD(0x08, 0, 1),
/* REG09 */
[F_FORCE_ICO]		= REG_FIELD(0x09, 7, 7),
[F_TMR2X_EN]		= REG_FIELD(0x09, 6, 6),
[F_BATFET_DIS]		= REG_FIELD(0x09, 5, 5),
[F_JEITA_VSET]		= REG_FIELD(0x09, 4, 4), // reserved on BQ25895
[F_BATFET_DLY]		= REG_FIELD(0x09, 3, 3),
[F_BATFET_RST_EN]	= REG_FIELD(0x09, 2, 2),
[F_PUMPX_UP]		= REG_FIELD(0x09, 1, 1),
[F_PUMPX_DN]		= REG_FIELD(0x09, 0, 0),
/* REG0A */
[F_BOOSTV]		= REG_FIELD(0x0A, 4, 7),
[F_BOOSTI]		= REG_FIELD(0x0A, 0, 2), // reserved on BQ25895
[F_PFM_OTG_DIS]		= REG_FIELD(0x0A, 3, 3), // BQ25896 only
/* REG0B */
[F_VBUS_STAT]		= REG_FIELD(0x0B, 5, 7),
[F_CHG_STAT]		= REG_FIELD(0x0B, 3, 4),
[F_PG_STAT]		= REG_FIELD(0x0B, 2, 2),
[F_SDP_STAT]		= REG_FIELD(0x0B, 1, 1), // reserved on BQ25896
[F_VSYS_STAT]		= REG_FIELD(0x0B, 0, 0),
/* REG0C */
[F_WD_FAULT]		= REG_FIELD(0x0C, 7, 7),
[F_BOOST_FAULT]		= REG_FIELD(0x0C, 6, 6),
[F_CHG_FAULT]		= REG_FIELD(0x0C, 4, 5),
[F_BAT_FAULT]		= REG_FIELD(0x0C, 3, 3),
[F_NTC_FAULT]		= REG_FIELD(0x0C, 0, 2),
/* REG0D */
[F_FORCE_VINDPM]	= REG_FIELD(0x0D, 7, 7),
[F_VINDPM]		= REG_FIELD(0x0D, 0, 6),
/* REG0E */
[F_THERM_STAT]		= REG_FIELD(0x0E, 7, 7),
[F_BATV]		= REG_FIELD(0x0E, 0, 6),
/* REG0F */
[F_SYSV]		= REG_FIELD(0x0F, 0, 6),
/* REG10 */
[F_TSPCT]		= REG_FIELD(0x10, 0, 6),
/* REG11 */
[F_VBUS_GD]		= REG_FIELD(0x11, 7, 7),
[F_VBUSV]		= REG_FIELD(0x11, 0, 6),
/* REG12 */
[F_ICHGR]		= REG_FIELD(0x12, 0, 6),
/* REG13 */
[F_VDPM_STAT]		= REG_FIELD(0x13, 7, 7),
[F_IDPM_STAT]		= REG_FIELD(0x13, 6, 6),
[F_IDPM_LIM]		= REG_FIELD(0x13, 0, 5),
/* REG14 */
[F_REG_RST]		= REG_FIELD(0x14, 7, 7),
[F_ICO_OPTIMIZED]	= REG_FIELD(0x14, 6, 6),
[F_PN]			= REG_FIELD(0x14, 3, 5),
[F_TS_PROFILE]		= REG_FIELD(0x14, 2, 2),
[F_DEV_REV]		= REG_FIELD(0x14, 0, 1)