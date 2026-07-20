import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID, ICON_POWER
from . import IP5306, ip5306_ns, CONF_IP5306_ID

DEPENDENCIES = ["ip5306"]

CONF_CHARGER_ENABLE = "charger_enable"

ChargerEnableSwitch = ip5306_ns.class_("ChargerEnableSwitch", switch.Switch)

CONFIG_SCHEMA = (
#    cv.GenerateID(CONF_IP5306_ID): cv.use_id(IP5306),
#    cv.Optional(CONF_CHARGER_ENABLE): switch.switch_schema(
#        ChargerEnableSwitch, icon=ICON_POWER, default_restore_mode="DISABLED",
#    ),
    switch.switch_schema(ChargerEnableSwitch, icon=ICON_POWER, default_restore_mode="DISABLED",)
    .extend(
        {
            cv.GenerateID(CONF_IP5306_ID): cv.use_id(IP5306),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_IP5306_ID])

    #var = cg.new_Pvariable(config[CONF_ID])
    #await switch.register_switch(var, config)
    var = await switch.new_switch(config)
    await cg.register_parented(var, parent)
    cg.add(parent.set_charger_enable_switch(var))

#    if CONF_CHARGER_ENABLE in config:
#        sw_var = await switch.new_switch(config[CONF_CHARGER_ENABLE])
#        await cg.register_parented(sw_var, parent)
#        cg.add(var.set_charger_enable_switch(sw_var))
