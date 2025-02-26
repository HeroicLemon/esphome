#include "tsl230r.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tsl230r {

static const char *const TAG = "tsl230r.sensor";

void TSL230RComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TSL230R...");
  this->output_store_.pulse_counter_setup(this->output_pin_);

  setup_output_enabled_();
  setup_sensitivity_();
  setup_output_frequency_scaling_();
}

void TSL230RComponent::setup_output_enabled_() {
  if (this->output_enable_pin_ != nullptr) {
    this->output_enable_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->output_enable_pin_->digital_write(!this->output_enabled_);
  }
}

void TSL230RComponent::setup_sensitivity_() {
  if (this->s0_pin_ != nullptr && this->s1_pin_ != nullptr) {
    this->s0_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->s1_pin_->pin_mode(gpio::FLAG_OUTPUT);
    switch (this->sensitivity_) {
      case TSL230R_SENSITIVITY_OFF:
        this->s0_pin_->digital_write(false);
        this->s1_pin_->digital_write(false);
        break;
      case TSL230R_SENSITIVITY_X1:
        this->s0_pin_->digital_write(true);
        this->s1_pin_->digital_write(false);
        break;
      case TSL230R_SENSITIVITY_X10:
        this->s0_pin_->digital_write(false);
        this->s1_pin_->digital_write(true);
        break;
      case TSL230R_SENSITIVITY_X100:
        this->s0_pin_->digital_write(true);
        this->s1_pin_->digital_write(true);
        break;
    }
  }
}

void TSL230RComponent::setup_output_frequency_scaling_() {
  if (this->s2_pin_ != nullptr && this->s3_pin_ != nullptr) {
    this->s2_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->s3_pin_->pin_mode(gpio::FLAG_OUTPUT);
    switch (this->output_frequency_scaling_) {
      case TSL230R_OUTPUT_FREQUENCY_SCALING_1:
        this->s2_pin_->digital_write(false);
        this->s3_pin_->digital_write(false);
        break;
      case TSL230R_OUTPUT_FREQUENCY_SCALING_2:
        this->s2_pin_->digital_write(true);
        this->s3_pin_->digital_write(false);
        break;
      case TSL230R_OUTPUT_FREQUENCY_SCALING_10:
        this->s2_pin_->digital_write(false);
        this->s3_pin_->digital_write(true);
        break;
      case TSL230R_OUTPUT_FREQUENCY_SCALING_100:
        this->s2_pin_->digital_write(true);
        this->s3_pin_->digital_write(true);
        break;
    }
  }
}

void TSL230RComponent::dump_config() {
  std::string sensitivity = "unknown";
  switch (this->sensitivity_) {
    case TSL230R_SENSITIVITY_OFF:
      sensitivity = "OFF";
      break;
    case TSL230R_SENSITIVITY_X1:
      sensitivity = "x1";
      break;
    case TSL230R_SENSITIVITY_X10:
      sensitivity = "x10";
      break;
    case TSL230R_SENSITIVITY_X100:
      sensitivity = "x100";
      break;
  }

  std::string output_frequency_scaling = "unknown";
  switch (this->output_frequency_scaling_) {
    case TSL230R_OUTPUT_FREQUENCY_SCALING_1:
      output_frequency_scaling = "1";
      break;
    case TSL230R_OUTPUT_FREQUENCY_SCALING_2:
      output_frequency_scaling = "2";
      break;
    case TSL230R_OUTPUT_FREQUENCY_SCALING_10:
      output_frequency_scaling = "10";
      break;
    case TSL230R_OUTPUT_FREQUENCY_SCALING_100:
      output_frequency_scaling = "100";
      break;
  }

  ESP_LOGCONFIG(TAG, "TSL230R:");
  LOG_PIN(" S0 Pin: ", this->s0_pin_);
  LOG_PIN(" S1 Pin: ", this->s1_pin_);
  LOG_PIN(" S2 Pin: ", this->s2_pin_);
  LOG_PIN(" S3 Pin: ", this->s3_pin_);
  LOG_PIN(" OE Pin: ", this->output_enable_pin_);
  LOG_PIN(" Out Pin: ", this->output_pin_);
  ESP_LOGCONFIG(TAG, "  Sensitivity: %s", sensitivity.c_str());
  ESP_LOGCONFIG(TAG, "  Output Frequency Scaling: %d", output_frequency_scaling.c_str());
  ESP_LOGCONFIG(TAG, "  Output Enabled: %s", ONOFF(this->output_enabled_));
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Frequency Sensor", this->frequency_sensor_);
}

void TSL230RComponent::update() {
  if (this->output_enabled_) {
    // TSL230R has 50% duty cycle
    pulse_counter::pulse_counter_t raw_output = this->output_store_.read_raw_value();

    float output_hz = 0.0f;
    if (raw_output > 1) {
      output_hz = raw_output / (this->get_update_interval() / 1000.0f);

      // Multiply the measured value by the output frequency scaling in order to normalize the output.
      output_hz = output_hz * this->output_frequency_scaling_;

      ESP_LOGD(TAG, "Got frequency=%.1f Hz", output_hz);
      if (this->frequency_sensor_ != nullptr) {
        this->frequency_sensor_->publish_state(output_hz);
      }
    }
  }
}

float TSL230RComponent::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace tsl230r
}  // namespace esphome
