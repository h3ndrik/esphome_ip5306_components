import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, i2c, sensor, text_sensor
from esphome.const import CONF_ID, CONF_BATTERY_LEVEL, DEVICE_CLASS_VOLTAGE, ICON_PERCENT, UNIT_PERCENT

DEPENDENCIES = ["i2c"]

MULTI_CONF = True

AUTO_LOAD = [ "binary_sensor", "sensor", "text_sensor" ]

ip5306_ns = cg.esphome_ns.namespace('ip5306')
IP5306 = ip5306_ns.class_('IP5306', cg.PollingComponent, i2c.I2CDevice)

CONF_CHARGER_CONNECTED = "charger_connected"
CONF_CHARGE_FULL = "charge_full"
CONF_OUTPUT_LOAD = "output_load"

CONF_IP5306_ID = "ip5306_id"

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(IP5306),
        cv.Optional(CONF_BATTERY_LEVEL): sensor.sensor_schema(
            IP5306,
            unit_of_measurement=UNIT_PERCENT,
            icon=ICON_PERCENT,
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_CHARGER_CONNECTED): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_CHARGE_FULL): binary_sensor.binary_sensor_schema(),
        cv.Optional(CONF_OUTPUT_LOAD): text_sensor.text_sensor_schema(),
    }
).extend(cv.polling_component_schema("60s")).extend(i2c.i2c_device_schema(0x75))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_BATTERY_LEVEL in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_LEVEL])
        cg.add(var.set_battery_level(sens))

    if CONF_CHARGER_CONNECTED in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CHARGER_CONNECTED])
        cg.add(var.set_charger_connected(sens))

    if CONF_CHARGE_FULL in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CHARGE_FULL])
        cg.add(var.set_charge_full(sens))

    if CONF_OUTPUT_LOAD in config:
        sens = await text_sensor.new_text_sensor(config[CONF_OUTPUT_LOAD])
        cg.add(var.set_output_load_sensor(sens))
