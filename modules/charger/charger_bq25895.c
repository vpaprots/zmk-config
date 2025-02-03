#define DT_DRV_COMPAT ti_bq25895

#include <zephyr/device.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(bq25895, CONFIG_CHARGER_LOG_LEVEL);

struct bq25895_config {
	struct i2c_dt_spec i2c;
	// uint32_t initial_current_microamp;
	// uint32_t max_voltage_microvolt;
	// uint32_t recharge_voltage_microvolt;
	// uint32_t precharge_threshold_voltage_microvolt;


}

#define BOOL(name, register, bit) int name;
#define INT (name, register, startbit, endbit, offset, multiplier, unit) int name
struct bq25895_status {
	#include "bq25895_regs.h"
}
#undef BOOL
#undef INT

struct int bq25895_get_status(const struct device *dev, bq25895_status *status) {
	const struct bq25895_config *cfg = dev->config;
	
	char regs[0x15];
	for (int r = 0; r<0x15; r++) {
		int ret = i2c_reg_read_byte_dt(&cfg->i2c, r, &regs[r]);
		if (ret < 0) {
			return ret;
		}
	}

	#define BOOL(name, register, bit) \
	 status->name = regs[register] == BIT(bit);
	#define INT (name, register, startbit, endbit, offset, multiplier, unit) \
	 status->name = offset + multiplier * ((regs[register] & GENMASK(startbit, endbit))<<startbit);
	#include "bq25895_regs.h"
	#undef BOOL
	#undef INT

	return 0;
}

/*
 * For ICHG <= 35mA = ICHGCODE + 5mA
 * For ICHG > 35mA = 40 + ((ICHGCODE-31)*10)mA.
 * Maximum programmable current = 1000mA
 *
 * Return: value between 0 and 127, negative on error.
 */
static int bq25895_ma_to_ichg(uint32_t current_ma, uint8_t *ichg)
{
	if (!IN_RANGE(current_ma, BQ25895_CURRENT_MIN_MA, BQ25895_CURRENT_MAX_MA)) {
		LOG_WRN("charging current out of range: %dmA, "
			"clamping to the nearest limit", current_ma);
	}
	current_ma = CLAMP(current_ma, BQ25895_CURRENT_MIN_MA, BQ25895_CURRENT_MAX_MA);

	if (current_ma <= 35) {
		*ichg = current_ma - 5;
		return 0;
	}

	*ichg = (current_ma - 40) / 10 + 31;

	return 0;
}

static uint32_t bq25895_ichg_to_ma(uint8_t ichg)
{
	ichg &= BQ25895_ICHG_MSK;

	if (ichg <= 30) {
		return (ichg + 5);
	}

	return (ichg - 31) * 10 + 40;
}

static int bq25895_mv_to_vbatreg(const struct bq25895_config *cfg, uint32_t voltage_mv,
				 uint8_t *vbat)
{
	if (!IN_RANGE(voltage_mv, BQ25895_VOLTAGE_MIN_MV, cfg->max_voltage_microvolt)) {
		LOG_WRN("charging voltage out of range: %dmV, "
			"clamping to the nearest limit",
			voltage_mv);
	}
	voltage_mv = CLAMP(voltage_mv, BQ25895_VOLTAGE_MIN_MV, cfg->max_voltage_microvolt);

	*vbat = (voltage_mv - BQ25895_VOLTAGE_MIN_MV) / BQ25895_FACTOR_VBAT_TO_MV;

	return 0;
}

static uint32_t bq25895_vbatreg_to_mv(uint8_t vbat)
{
	vbat &= BQ25895_VBAT_MSK;

	return (vbat * BQ25895_FACTOR_VBAT_TO_MV) + BQ25895_VOLTAGE_MIN_MV;
}

static int bq25183_charge_enable(const struct device *dev, const bool enable)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t value = enable ? 0 : BQ25895_ICHG_CHG_DIS;
	int ret;

	ret = i2c_reg_update_byte_dt(&cfg->i2c, BQ25895_ICHG_CTRL,
				     BQ25895_ICHG_CHG_DIS, value);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

static int bq25895_set_charge_current(const struct device *dev,
				      uint32_t const_charge_current_ua)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	ret = bq25895_ma_to_ichg(const_charge_current_ua / 1000, &val);
	if (ret < 0) {
		return ret;
	}

	ret = i2c_reg_update_byte_dt(&cfg->i2c, BQ25895_ICHG_CTRL,
				     BQ25895_ICHG_MSK, val);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

static int bq25895_get_charge_current(const struct device *dev,
				      uint32_t *const_charge_current_ua)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	ret = i2c_reg_read_byte_dt(&cfg->i2c, BQ25895_ICHG_CTRL, &val);
	if (ret < 0) {
		return ret;
	}

	*const_charge_current_ua = bq25895_ichg_to_ma(val) * 1000;

	return 0;
}

static int bq25895_set_charge_voltage(const struct device *dev, uint32_t const_charge_voltage_uv)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	ret = bq25895_mv_to_vbatreg(cfg, const_charge_voltage_uv / 1000, &val);
	if (ret < 0) {
		return ret;
	}

	ret = i2c_reg_update_byte_dt(&cfg->i2c, BQ25895_VBAT_CTRL, BQ25895_VBAT_MSK, val);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

static int bq25895_get_charge_voltage(const struct device *dev, uint32_t *const_charge_voltage_uv)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	ret = i2c_reg_read_byte_dt(&cfg->i2c, BQ25895_VBAT_CTRL, &val);
	if (ret < 0) {
		return ret;
	}

	*const_charge_voltage_uv = bq25895_vbatreg_to_mv(val) * 1000;

	return 0;
}

static int bq25895_get_online(const struct device *dev,
			      enum charger_online *online)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	ret = i2c_reg_read_byte_dt(&cfg->i2c, BQ25895_STAT0, &val);
	if (ret < 0) {
		return ret;
	}

	if ((val & BQ25895_STAT0_VIN_PGOOD_STAT) != 0x00) {
		*online = CHARGER_ONLINE_FIXED;
	} else {
		*online = CHARGER_ONLINE_OFFLINE;
	}

	return 0;
}

static int bq25895_get_status(const struct device *dev,
			      enum charger_status *status)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t stat0;
	uint8_t ichg_ctrl;
	int ret;

	ret = i2c_reg_read_byte_dt(&cfg->i2c, BQ25895_STAT0, &stat0);
	if (ret < 0) {
		return ret;
	}

	if ((stat0 & BQ25895_STAT0_VIN_PGOOD_STAT) == 0x00) {
		*status = CHARGER_STATUS_DISCHARGING;
		return 0;
	}

	ret = i2c_reg_read_byte_dt(&cfg->i2c, BQ25895_ICHG_CTRL, &ichg_ctrl);
	if (ret < 0) {
		return ret;
	}

	if ((ichg_ctrl & BQ25895_ICHG_CHG_DIS) != 0x00) {
		*status = CHARGER_STATUS_NOT_CHARGING;
		return 0;
	}

	switch (FIELD_GET(BQ25895_STAT0_CHG_STAT_MASK, stat0)) {
	case BQ25895_STAT0_CHG_STAT_NOT_CHARGING:
		*status = CHARGER_STATUS_NOT_CHARGING;
		break;
	case BQ25895_STAT0_CHG_STAT_CONSTANT_CURRENT:
	case BQ25895_STAT0_CHG_STAT_CONSTANT_VOLTAGE:
		*status = CHARGER_STATUS_CHARGING;
		break;
	case BQ25895_STAT0_CHG_STAT_DONE:
		*status = CHARGER_STATUS_FULL;
		break;
	}

	return 0;
}

static int bq25895_get_prop(const struct device *dev, charger_prop_t prop,
			    union charger_propval *val)
{
	switch (prop) {
	case CHARGER_PROP_ONLINE:
		return bq25895_get_online(dev, &val->online);
	case CHARGER_PROP_STATUS:
		return bq25895_get_status(dev, &val->status);
	// case CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA:
	// 	return bq25895_get_charge_current(dev, &val->const_charge_current_ua);
	// case CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV:
	// 	return bq25895_get_charge_voltage(dev, &val->const_charge_voltage_uv);
	default:
		return -ENOTSUP;
	}
}

static int bq25895_set_prop(const struct device *dev, charger_prop_t prop,
			    const union charger_propval *val)
{
	switch (prop) {
	// case CHARGER_PROP_CONSTANT_CHARGE_CURRENT_UA:
	// 	return bq25895_set_charge_current(dev, val->const_charge_current_ua);
	// case CHARGER_PROP_CONSTANT_CHARGE_VOLTAGE_UV:
	// 	return bq25895_set_charge_voltage(dev, val->const_charge_voltage_uv);
	default:
		return -ENOTSUP;
	}
}

static const struct charger_driver_api bq25895_api = {
	.get_property = bq25895_get_prop,
	.set_property = bq25895_set_prop,
	// .charge_enable = bq25183_charge_enable,
};

static int bq25895_init(const struct device *dev)
{
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	ret = i2c_reg_read_byte_dt(&cfg->i2c, BQ25895_MASK_ID, &val);
	if (ret < 0) {
		return ret;
	}

	val &= BQ25895_DEVICE_ID_MSK;
	if (val != BQ25895_DEVICE_ID) {
		LOG_ERR("Invalid device id: %02x", val);
		return -EINVAL;
	}

	/* Disable the watchdog */
	ret = i2c_reg_update_byte_dt(&cfg->i2c, BQ25895_IC_CTRL,
				     BQ25895_WATCHDOG_SEL_1_MSK,
				     BQ25895_WATCHDOG_DISABLE);
	if (ret < 0) {
		return ret;
	}

	ret = bq25895_set_charge_voltage(dev, cfg->max_voltage_microvolt);
	if (ret < 0) {
		LOG_ERR("Could not set the target voltage. (rc: %d)", ret);
		return ret;
	}

	if (cfg->recharge_voltage_microvolt > 0) {
		if ((cfg->max_voltage_microvolt - cfg->recharge_voltage_microvolt) > 100000) {
			val = BQ25895_IC_CTRL_VRCH_200;
		} else {
			val = BQ25895_IC_CTRL_VRCH_100;
		}

		ret = i2c_reg_update_byte_dt(&cfg->i2c, BQ25895_IC_CTRL, BQ25895_IC_CTRL_VRCH_MSK,
					     val);
		if (ret < 0) {
			return ret;
		}
	}

	/* Precharge threshold voltage */
	if (cfg->precharge_threshold_voltage_microvolt <= 2800000) {
		val = BQ25895_VLOWV_SEL_2_8;
	} else {
		val = BQ25895_VLOWV_SEL_3_0;
	}

	ret = i2c_reg_update_byte_dt(&cfg->i2c, BQ25895_IC_CTRL, BQ25895_VLOWV_SEL_MSK, val);
	if (ret < 0) {
		return ret;
	}

	if (cfg->initial_current_microamp > 0) {
		ret = bq25895_set_charge_current(dev, cfg->initial_current_microamp);
		if (ret < 0) {
			return ret;
		}
	}

	return 0;
}

#define CHARGER_BQ25895_INIT(inst)                                                                 \
	static const struct bq25895_config bq25895_config_##inst = {                               \
		.i2c = I2C_DT_SPEC_INST_GET(inst),                                                 \
		.initial_current_microamp =                                                        \
			DT_INST_PROP(inst, constant_charge_current_max_microamp),                  \
		.max_voltage_microvolt =                                                           \
			DT_INST_PROP(inst, constant_charge_voltage_max_microvolt),                 \
		.recharge_voltage_microvolt =                                                      \
			DT_INST_PROP_OR(inst, re_charge_voltage_microvolt, 0),                     \
		.precharge_threshold_voltage_microvolt =                                           \
			DT_INST_PROP(inst, precharge_voltage_threshold_microvolt),                 \
	};                                                                                         \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(inst, bq25895_init, NULL, NULL, &bq25895_config_##inst, POST_KERNEL, \
			      CONFIG_CHARGER_INIT_PRIORITY, &bq25895_api);

DT_INST_FOREACH_STATUS_OKAY(CHARGER_BQ25895_INIT)
