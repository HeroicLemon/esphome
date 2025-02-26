# Here is the project that started me down the TSL2591 device trail in the first
# place: https://hackaday.io/project/176690-the-water-watcher

from esphome import pins
import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ENABLE_PIN,
    CONF_FREQUENCY,
    CONF_ID,
    CONF_SENSITIVITY,
    ICON_LIGHTBULB,
    STATE_CLASS_MEASUREMENT,
    UNIT_HERTZ,
)

CONF_OUTPUT_FREQUENCY_SCALING = "output_frequency_scaling"
CONF_OUTPUT_ENABLED = "output_enabled"
CONF_OUTPUT_PIN = "output_pin"
CONF_S0_PIN = "s0_pin"
CONF_S1_PIN = "s1_pin"
CONF_S2_PIN = "s2_pin"
CONF_S3_PIN = "s3_pin"

AUTO_LOAD = ["pulse_counter"]

tsl2230r_ns = cg.esphome_ns.namespace("tsl230r")
TSL230RComponent = tsl2230r_ns.class_(
    "TSL230RComponent", sensor.Sensor, cg.PollingComponent
)
TSL230RSensitivity = tsl2230r_ns.enum("TSL230RSensitivity")
TSL230ROutputFrequencyScaling = tsl2230r_ns.enum("TSL230ROutputFrequencyScaling")

SENSITIVITY = {
    "OFF": TSL230RSensitivity.TSL230R_SENSITIVITY_OFF,
    "x1": TSL230RSensitivity.TSL230R_SENSITIVITY_X1,
    "x10": TSL230RSensitivity.TSL230R_SENSITIVITY_X10,
    "x100": TSL230RSensitivity.TSL230R_SENSITIVITY_X100,
}


OUTPUT_FREQUENCY_SCALING = {
    1: TSL230ROutputFrequencyScaling.TSL230R_OUTPUT_FREQUENCY_SCALING_1,
    2: TSL230ROutputFrequencyScaling.TSL230R_OUTPUT_FREQUENCY_SCALING_2,
    10: TSL230ROutputFrequencyScaling.TSL230R_OUTPUT_FREQUENCY_SCALING_10,
    100: TSL230ROutputFrequencyScaling.TSL230R_OUTPUT_FREQUENCY_SCALING_100,
}


def validate_output_frequency_scaling(value):
    return cv.enum(OUTPUT_FREQUENCY_SCALING, int=True)(value)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(TSL230RComponent),
        cv.Required(CONF_OUTPUT_PIN): pins.gpio_input_pin_schema,
        cv.Optional(CONF_FREQUENCY): sensor.sensor_schema(
            unit_of_measurement=UNIT_HERTZ,
            icon=ICON_LIGHTBULB,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(
            CONF_OUTPUT_FREQUENCY_SCALING, default="1"
        ): validate_output_frequency_scaling,
        cv.Optional(CONF_SENSITIVITY, default="x1"): cv.enum(SENSITIVITY),
        cv.Optional(CONF_OUTPUT_ENABLED, default=True): cv.boolean,
        cv.Optional(CONF_ENABLE_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_S0_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_S1_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_S2_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_S3_PIN): pins.gpio_output_pin_schema,
    }
).extend(cv.polling_component_schema("5s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_OUTPUT_FREQUENCY_SCALING in config:
        conf = config[CONF_OUTPUT_FREQUENCY_SCALING]
        cg.add(var.set_output_frequency_scaling(conf))

    if CONF_SENSITIVITY in config:
        conf = config[CONF_SENSITIVITY]
        cg.add(var.set_sensitivity(conf))

    if CONF_OUTPUT_ENABLED in config:
        conf = config[CONF_OUTPUT_ENABLED]
        cg.add(var.set_output_enabled(conf))

    if CONF_ENABLE_PIN in config:
        conf = await cg.gpio_pin_expression(config[CONF_ENABLE_PIN])
        cg.add(var.set_output_enable_pin(conf))

    if CONF_S0_PIN in config:
        conf = await cg.gpio_pin_expression(config[CONF_S0_PIN])
        cg.add(var.set_s0_pin(conf))

    if CONF_S1_PIN in config:
        conf = await cg.gpio_pin_expression(config[CONF_S1_PIN])
        cg.add(var.set_s1_pin(conf))

    if CONF_S2_PIN in config:
        conf = await cg.gpio_pin_expression(config[CONF_S2_PIN])
        cg.add(var.set_s2_pin(conf))

    if CONF_S3_PIN in config:
        conf = await cg.gpio_pin_expression(config[CONF_S3_PIN])
        cg.add(var.set_s3_pin(conf))

    if CONF_FREQUENCY in config:
        sens = await sensor.new_sensor(config[CONF_FREQUENCY])
        cg.add(var.set_frequency_sensor(sens))

    conf = await cg.gpio_pin_expression(config[CONF_OUTPUT_PIN])
    cg.add(var.set_output_pin(conf))
