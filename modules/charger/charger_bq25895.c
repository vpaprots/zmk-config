#define DT_DRV_COMPAT zmk_bq25895

#include <zephyr/device.h>
#include <zephyr/drivers/charger.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/sensor.h>

LOG_MODULE_REGISTER(bq25895, CONFIG_BQ25895_LOG_LEVEL);

#define BOOLF(name, register, bit, extra)
#define BOOL(name, register, bit, extra) int name;
#define INT(name, register, startbit, endbit, offset, scale, extra) int name;
#define INTA(name, register, startbit, endbit, offset, scale, extra) int name;
#include "bq25895_regs.h"
struct bq25895_config {
	struct i2c_dt_spec i2c;
    struct gpio_dt_spec irq_gpio;
	// all values shifted, as set in I2C registers
	BQ_CFG(0)
};

struct bq25895_data {
    BQ_ADC(0)
};

#undef BOOL
#undef INT
#undef BOOLF
#undef INTA

#ifdef CONFIG_BQ25895_LOG_LEVEL_DBG
#define INT(name, register, startbit, endbit, offset, scale, extra) \
dev_val = ((regs[register] & GENMASK(endbit, startbit))>> startbit) * scale + offset; \
cfg_val = (cfg->name >> startbit) * scale + offset; \
LOG_DBG("%s is set to %d, expect %d", #name, dev_val, cfg_val); 
#define BOOL(name, register, bit, extra) INT(name, register, bit, bit, 0, 1, extra)
#define INTA(name, register, startbit, endbit, offset, scale, extra) \
dev_val = ((regs[register-0xe] & GENMASK(endbit, startbit))>> startbit) * scale + offset; \
LOG_DBG("%s is measured at %d", #name, dev_val);
#include "bq25895_regs.h"
static void dump_settings(const struct device *dev, uint8_t* regs) {
	const struct bq25895_config *cfg = dev->config;
	int cfg_val, dev_val;
	BQ_CFG(0)
}

static void dump_adc(const struct device *dev, uint8_t* regs) {
	int dev_val;
	BQ_ADC(0)
}
#undef BOOL
#undef INT
#undef BOOLF
#undef INTA
#endif //CONFIG_BQ25895_LOG_LEVEL_DBG

#define BOOLF(name, register, bit, extra) \
static int set_##name(const struct bq25895_config *cfg) { \
	return i2c_reg_update_byte_dt(&cfg->i2c, register, GENMASK(bit,bit), GENMASK(bit,bit)); \
}
#define INT(name, register, startbit, endbit, offset, scale, extra) \
	if (cfg->name != (GENMASK(endbit, startbit) & regs[register])) { \
		LOG_DBG("Updating %s to %d", #name, cfg->name); \
		ret = i2c_reg_update_byte_dt(&cfg->i2c, register, GENMASK(endbit, startbit), cfg->name); \
		if (ret < 0) { return ret; } \
	}
#define BOOL(name, register, bit, extra) INT(name, register, bit, bit, 0, 1, extra)
#define INTA(name, register, startbit, endbit, offset, scale, extra) \
	drv_data->name = ((GENMASK(endbit, startbit) & regs[register - 0xE]) >> startbit) * scale + offset;

#include "bq25895_regs.h"

BQ_ACTION(0)

static int bq25895_init(const struct device *dev) {
	const struct bq25895_config *cfg = dev->config;
	uint8_t val;
	int ret;

	// check readiness of irq gpio pin
    if (!device_is_ready(cfg->irq_gpio.port)) {
        LOG_ERR("IRQ GPIO device not ready!");
        return -ENODEV;
    }

	if (!device_is_ready(cfg->i2c.bus)) {
        LOG_WRN("I2C BUS not ready!");
        return -ENODEV;
    }

	// init the irq pin
    ret = gpio_pin_configure_dt(&cfg->irq_gpio, GPIO_INPUT);
    if (ret) {
        LOG_ERR("Cannot configure IRQ GPIO");
        return ret;
    }

    // setup and add the irq callback associated
    // gpio_init_callback(&data->irq_gpio_cb, pmw3610_gpio_callback, BIT(cfg->irq_gpio.pin));

    // err = gpio_add_callback(config->irq_gpio.port, &data->irq_gpio_cb);
    // if (err) {
    //     LOG_ERR("Cannot add IRQ GPIO callback");
    // }

	uint8_t regs[0x15];
	ret = i2c_burst_read_dt(&cfg->i2c, 0, (uint8_t *)&regs, sizeof(regs)/sizeof(uint8_t));
	if (ret < 0) {
		LOG_ERR("Error reading registers: %02x", ret);
		return ret;
	}

	#ifdef CONFIG_BQ25895_LOG_LEVEL_DBG
	for (int i=0; i<sizeof(regs)/sizeof(uint8_t); i++) {
		//LOG_DBG("Reg BQ25895 %d set to %d", i, regs[i]);
	}
	dump_settings(dev, regs);
	#endif //CONFIG_BQ25895_LOG_LEVEL_DBG

	val = (regs[0x14]&GENMASK(5,3))>>3;
	if (val != 0b000111) {
		LOG_ERR("Invalid device id: %02x", val);
		return -EINVAL;
	}

	// Put bq25895 into host mode
	ret = set_wd_rst(cfg);
	if (ret < 0) {
		return ret;
	}

	// Correct any mismatched config values
	BQ_CFG(0);

	#ifdef CONFIG_BQ25895_LOG_LEVEL_DBG
	ret = i2c_burst_read_dt(&cfg->i2c, 0, (uint8_t *)&regs, sizeof(regs)/sizeof(uint8_t));
	if (ret < 0) {
		return ret;
	}
	dump_settings(dev, regs);
	#endif //CONFIG_BQ25895_LOG_LEVEL_DBG
	return 0;
}

static int bq25895_sample_fetch(const struct device *dev, enum sensor_channel chan) {
	bool valid_chan = chan == SENSOR_CHAN_GAUGE_STATE_OF_CHARGE || chan == SENSOR_CHAN_GAUGE_VOLTAGE || chan == SENSOR_CHAN_ALL;
	if (!valid_chan) {
        LOG_DBG("unsupported channel %d", chan);
        return -ENOTSUP;
    }

	const struct bq25895_config *cfg = dev->config;
    int ret;
	ret = set_conv_start(cfg);
	k_sleep(K_SECONDS(1));
	
	uint8_t regs[0x15]; ret = i2c_burst_read_dt(&cfg->i2c, 0, (uint8_t *)&regs, sizeof(regs)/sizeof(uint8_t));
	//uint8_t regs[5];
	//ret = i2c_burst_read_dt(&cfg->i2c, 0x0E, (uint8_t *)&regs, sizeof(regs)/sizeof(uint8_t));
	if (ret < 0) {
		LOG_ERR("Error reading registers: %02x", ret);
		return ret;
	}

	#ifdef CONFIG_BQ25895_LOG_LEVEL_DBG
	for (int i=0; i<sizeof(regs)/sizeof(uint8_t); i++) {
		//LOG_DBG("ADC Reg %d set to %d", i+0xE, regs[i]);
		LOG_DBG("Reg %d set to %d", i, regs[i]);
	}
	dump_settings(dev, regs);
	dump_adc(dev, regs);
	#endif //CONFIG_BQ25895_LOG_LEVEL_DBG

	// set drv_data
	struct bq25895_data *const drv_data = dev->data;
	BQ_ADC(0);
	return 0;
}
#undef BOOL
#undef INT
#undef BOOLF
#undef INTA

static int bq25895_channel_get(const struct device *dev, enum sensor_channel chan,
                                struct sensor_value *val) {
    int err = 0;
	
	struct bq25895_data *const drv_data = dev->data;
    int bat_voltage = drv_data->batv;
	LOG_DBG("ADC bat_voltage is measured at %d mV", bat_voltage);

    switch (chan) {
    case SENSOR_CHAN_GAUGE_VOLTAGE:
		LOG_DBG("ADC+ bat_voltage is measured at %d mV", bat_voltage);
        // 1250 / 16 = 78.125
        uint64_t tmp = bat_voltage * 1250 / 16;
        val->val1 = tmp / 1000000;
        val->val2 = tmp % 1000000;
        break;

    case SENSOR_CHAN_GAUGE_STATE_OF_CHARGE:
		LOG_DBG("ADC- bat_voltage is measured at %d mV", bat_voltage);
        val->val1 = (bat_voltage >> 8);
        val->val2 = (bat_voltage & 0xFF) * 1000000 / 256;
        break;

    default:
        err = -ENOTSUP;
        break;
    }

    return err;
}

static const struct sensor_driver_api bq25895_sensor_api = {
	.sample_fetch = bq25895_sample_fetch,
	.channel_get = bq25895_channel_get
};

#if 0 // Charger API isnt used in ZMK; also no clear multi-API device support in zephyr 
static int bq25895_get_prop(const struct device *dev, charger_prop_t prop,
			    union charger_propval *val)
{
	switch (prop) {
	// case CHARGER_PROP_ONLINE:
	// 	return bq25895_get_online(dev, &val->online);
	// case CHARGER_PROP_STATUS:
	// 	return bq25895_get_status(dev, &val->status);
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

static const struct charger_driver_api bq25895_charger_api = {
	.get_property = bq25895_get_prop,
	.set_property = bq25895_set_prop,
	// .charge_enable = bq25183_charge_enable,
};
#endif

#define INT(name, register, startbit, endbit, offset, scale, inst) \
. name = GENMASK(endbit, startbit) & ((DT_INST_PROP(inst, name)-offset)/scale)<<startbit,
#define BOOL(name, register, bit, inst) INT(name, register, bit, bit, 0, 1, inst)
#define INTA(name, register, startbit, endbit, offset, scale, inst) \
. name = 0,
#include "bq25895_regs.h"
// #undef BOOL
// #undef INT

#define CHARGER_BQ25895_INIT(inst)                                      \
	static const struct bq25895_config bq25895_config_##inst = {        \
		.i2c = I2C_DT_SPEC_INST_GET(inst),                              \
		.irq_gpio = GPIO_DT_SPEC_INST_GET(inst, irq_gpios),             \
		BQ_CFG(inst)                                                    \
	};                                                                  \
	static struct bq25895_data bq25895_data_##inst = {                  \
		BQ_ADC(inst)                                                    \
	};                                                                  \
                                                                        \
	DEVICE_DT_INST_DEFINE(inst, bq25895_init, NULL, &bq25895_data_##inst,\
	    &bq25895_config_##inst, POST_KERNEL, CONFIG_CHARGER_INIT_PRIORITY, &bq25895_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(CHARGER_BQ25895_INIT)


// static const struct sensor_driver_api bq25895_api_table = {.sample_fetch = bq25895_sample_fetch,
//                                                             .channel_get = bq25895_channel_get};
//     DEVICE_DT_INST_DEFINE(inst, bq25895_init, NULL, &bq25895_##inst##_drvdata,                   
//                           &bq25895_##inst##_config, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,     
//                           &bq25895_api_table);

// DT_INST_FOREACH_STATUS_OKAY(MAX17048_INIT)