#include "ip5306.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ip5306 {

static const char *TAG = "ip5306";

float IP5306::get_setup_priority() const { return setup_priority::IO; }

void IP5306::setup() {
  ESP_LOGD(TAG, "Setting up ip5306...");
  if (this->read_register(static_cast<uint8_t>(RegisterMap::SYS_CTL0), &this->status_.reg_sys_ctl0.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }
  this->status_.reg_sys_ctl0.boost_enable = true;
  this->status_.reg_sys_ctl0.charger_enable = true;
  this->status_.reg_sys_ctl0.power_on_load = true;
  this->status_.reg_sys_ctl0.boost_output_enable = true;
  this->status_.reg_sys_ctl0.button_shutdown = true;
  if (this->write_register(static_cast<uint8_t>(RegisterMap::SYS_CTL0), &this->status_.reg_sys_ctl0.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }

  if (this->read_register(static_cast<uint8_t>(RegisterMap::SYS_CTL1), &this->status_.reg_sys_ctl1.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }
  this->status_.reg_sys_ctl1.boost_after_vin_unplug = true;
  if (this->write_register(static_cast<uint8_t>(RegisterMap::SYS_CTL1), &this->status_.reg_sys_ctl1.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }

  set_battery_voltage(BatteryVoltage::BAT_VOLTAGE_4_2);
  set_voltage_pressure(VoltagePressure::PRESSURIZED_28);
  set_light_load_shutdown_time(ShutdownTime::SHUTDOWN_64s);
}

void IP5306::update() {
  // Work to be done at each update interval
  if (this->battery_level_ != nullptr) {
    if (this->read_register(static_cast<uint8_t>(RegisterMap::LEVEL), &this->status_.reg_level.raw, 1) != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "unable to read level");
      this->status_set_warning();
      return;
    }
    float value = 0;
    switch (this->status_.reg_level.charge_level) {
      case ChargeLevel::LEVEL_25:  value = 25; break;
      case ChargeLevel::LEVEL_50:  value = 50; break;
      case ChargeLevel::LEVEL_75:  value = 75; break;
      case ChargeLevel::LEVEL_100: value = 100; break;
      //default: value = 0; break;
    }
    if (this->battery_level_ != nullptr)
      this->battery_level_->publish_state(value);
  }
  if (this->read_register(static_cast<uint8_t>(RegisterMap::READ0), &this->status_.reg_read0.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "unable to read status");
    this->status_set_warning();
    return;
  }
  if (this->read_register(static_cast<uint8_t>(RegisterMap::READ1), &this->status_.reg_read1.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "unable to read status");
    this->status_set_warning();
    return;
  }
  if (this->read_register(static_cast<uint8_t>(RegisterMap::READ2), &this->status_.reg_read2.raw, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "unable to read status");
    this->status_set_warning();
    return;
  }

  if (this->charger_connected_ != nullptr)
    this->charger_connected_->publish_state(this->status_.reg_read0.charge_enable);
  if (this->charge_full_ != nullptr)
    this->charge_full_->publish_state(this->status_.reg_read1.battery_full);

  if (this->output_load_ != nullptr) {
    if (this->status_.reg_read2.load_level == 0)
      this->output_load_->publish_state("high");
    else if (this->status_.reg_read2.load_level == 1)
      this->output_load_->publish_state("low");
  }

  if (this->charger_enable_ != nullptr) {
    this->charger_enable_->publish_state(this->status_.reg_sys_ctl0.charger_enable);
  }
}

void IP5306::set_light_load_shutdown_time(ShutdownTime time) {
  this->read_register(static_cast<uint8_t>(RegisterMap::SYS_CTL2), &this->status_.reg_sys_ctl2.raw, 1);
  this->status_.reg_sys_ctl2.light_load_shutdown_time = time;
  this->write_register(static_cast<uint8_t>(RegisterMap::SYS_CTL2), &this->status_.reg_sys_ctl2.raw, 1);
}

void IP5306::set_battery_voltage(BatteryVoltage voltage) {
  this->read_register(static_cast<uint8_t>(RegisterMap::CHG_CTL2), &this->status_.reg_chg_ctl2.raw, 1);
  this->status_.reg_chg_ctl2.battery_voltage = voltage;
  this->write_register(static_cast<uint8_t>(RegisterMap::CHG_CTL2), &this->status_.reg_chg_ctl2.raw, 1);
}

void IP5306::set_voltage_pressure(VoltagePressure pressure) {
  this->read_register(static_cast<uint8_t>(RegisterMap::CHG_CTL2), &this->status_.reg_chg_ctl2.raw, 1);
  this->status_.reg_chg_ctl2.voltage_pressure = pressure;
  this->write_register(static_cast<uint8_t>(RegisterMap::CHG_CTL2), &this->status_.reg_chg_ctl2.raw, 1);
}

void IP5306::write_state(bool state) {
  if (this->status_.reg_sys_ctl0.charger_enable != state) {
    this->read_register(static_cast<uint8_t>(RegisterMap::SYS_CTL0), &this->status_.reg_sys_ctl0.raw, 1);
    this->status_.reg_sys_ctl0.charger_enable = state;
    this->write_register(static_cast<uint8_t>(RegisterMap::SYS_CTL0), &this->status_.reg_sys_ctl0.raw, 1);
  }
  this->charger_enable_->publish_state(this->status_.reg_sys_ctl0.charger_enable);
}

}  // namespace ip5306
}  // namespace esphome

