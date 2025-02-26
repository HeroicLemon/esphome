#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/pulse_counter/pulse_counter_sensor.h"

#include <cinttypes>

namespace esphome {
namespace tsl230r {

enum TSL230RSensitivity : uint8_t {
  TSL230R_SENSITIVITY_OFF,
  TSL230R_SENSITIVITY_X1,
  TSL230R_SENSITIVITY_X10,
  TSL230R_SENSITIVITY_X100,
};

enum TSL230ROutputFrequencyScaling : uint8_t {
  TSL230R_OUTPUT_FREQUENCY_SCALING_1 = 1,
  TSL230R_OUTPUT_FREQUENCY_SCALING_2 = 2,
  TSL230R_OUTPUT_FREQUENCY_SCALING_10 = 10,
  TSL230R_OUTPUT_FREQUENCY_SCALING_100 = 100,
};

#ifdef HAS_PCNT
#define USE_PCNT true
#else
#define USE_PCNT false
#endif

/**
 * This class includes support for the TSL230R ambient light sensor.
 * The TSL230R outputs a 50% duty cycle square wave with a frequency proportional to the intensity of light.
 * It has configurable sensitivity levels as well as the ability to scale the output frequency
 */
class TSL230RComponent : public PollingComponent {
 public:
  TSL230RComponent() : output_store_(*pulse_counter::get_storage(USE_PCNT)) {}

  void set_s0_pin(InternalGPIOPin *pin) { this->s0_pin_ = pin; }
  void set_s1_pin(InternalGPIOPin *pin) { this->s1_pin_ = pin; }
  void set_s2_pin(InternalGPIOPin *pin) { this->s2_pin_ = pin; }
  void set_s3_pin(InternalGPIOPin *pin) { this->s3_pin_ = pin; }
  void set_output_enable_pin(InternalGPIOPin *pin) { this->output_enable_pin_ = pin; }
  void set_output_pin(InternalGPIOPin *pin) { this->output_pin_ = pin; }

  void set_frequency_sensor(sensor::Sensor *frequency_sensor) { this->frequency_sensor_ = frequency_sensor; }

  void set_sensitivity(TSL230RSensitivity sensitivity) { this->sensitivity_ = sensitivity; }
  void set_output_frequency_scaling(TSL230ROutputFrequencyScaling output_frequency_scaling) {
    this->output_frequency_scaling_ = output_frequency_scaling;
  }
  void set_output_enabled(bool output_enabled) { this->output_enabled_ = output_enabled; }

  /**
   * ESPHome functions
   */
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

 protected:
  InternalGPIOPin *s0_pin_{nullptr};
  InternalGPIOPin *s1_pin_{nullptr};
  InternalGPIOPin *s2_pin_{nullptr};
  InternalGPIOPin *s3_pin_{nullptr};
  InternalGPIOPin *output_enable_pin_{nullptr};
  InternalGPIOPin *output_pin_;
  sensor::Sensor *frequency_sensor_{nullptr};

  TSL230RSensitivity sensitivity_;
  TSL230ROutputFrequencyScaling output_frequency_scaling_;
  bool output_enabled_;
  pulse_counter::PulseCounterStorageBase &output_store_;

  void setup_output_enabled_();
  void setup_sensitivity_();
  void setup_output_frequency_scaling_();
};

}  // namespace tsl230r
}  // namespace esphome
