#include "ip5306.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ip5306 {

static const char *TAG = "ip5306";

static const uint8_t IP5306_REG_SYS_CTL0 = 0x00;
static const uint8_t IP5306_REG_SYS_CTL1 = 0x01;
//static const uint8_t IP5306_REG_SYS_CTL2 = 0x02;
//static const uint8_t IP5306_REG_CHG_CTL0 = 0x20;
//static const uint8_t IP5306_REG_CHG_CTL1 = 0x21;
static const uint8_t IP5306_REG_CHG_DIG  = 0x24;

static const uint8_t IP5306_REG_READ0    = 0x70;  // charge en
static const uint8_t IP5306_REG_READ1    = 0x71;  // charge full
static const uint8_t IP5306_REG_READ2    = 0x72;  // load
//static const uint8_t IP5306_REG_READ3    = 0x77;
static const uint8_t IP5306_REG_LEVEL    = 0x78;  // bat level

// REG_SYS_CTL0
static const uint8_t BOOST_ENABLE_BIT    = 0x20;
static const uint8_t CHARGE_OUT_BIT      = 0x10;
static const uint8_t BOOT_ON_LOAD_BIT    = 0x04;
static const uint8_t BOOST_OUT_BIT       = 0x02;
static const uint8_t BOOST_BUTTON_EN_BIT = 0x01;

// REG_SYS_CTL1
//static const uint8_t BOOST_SET_BIT   = 0x80;
static const uint8_t WLED_SET_BIT      = 0x40;
static const uint8_t SHORT_BOOST_BIT   = 0x20;
static const uint8_t VIN_ENABLE_BIT    = 0x04;
static const uint8_t LOW_POWER_SHUTDOWN_BIT  = 0x01;

// REG_READ0
static const uint8_t CHARGE_ENABLE_BIT = 0x08;

// REG_READ1
static const uint8_t CHARGE_FULL_BIT   = 0x08;

// REG_READ2
static const uint8_t HEAVY_LOAD_BIT   = 0x04;


float IP5306::get_setup_priority() const { return setup_priority::IO; }

void IP5306::setup() {
  ESP_LOGD(TAG, "Setting up ip5306...");
  uint8_t data;
  if (this->read_register(IP5306_REG_SYS_CTL0, &data, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }
  data = data | BOOST_ENABLE_BIT | CHARGE_OUT_BIT | BOOT_ON_LOAD_BIT | BOOST_OUT_BIT | BOOST_BUTTON_EN_BIT;
  if (this->write_register(IP5306_REG_SYS_CTL0, &data, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }

  if (this->read_register(IP5306_REG_SYS_CTL1, &data, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }
  data = data | VIN_ENABLE_BIT;
  if (this->write_register(IP5306_REG_SYS_CTL1, &data, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "setup failed");
    this->mark_failed();
    return;
  }
}

void IP5306::update() {
  // Work to be done at each update interval
  uint8_t data;
  if (this->battery_level_ != nullptr) {
    if (this->read_register(IP5306_REG_LEVEL, &data, 1) != i2c::ERROR_OK) {
      ESP_LOGW(TAG, "unable to read level");
      this->status_set_warning();
      return;
    }
    float value = 0;
    switch (data & 0xF0) {
      case 0xE0: value = 25; break;
      case 0xC0: value = 50; break;
      case 0x80: value = 75; break;
      case 0x00: value = 100; break;
      //default: value = 0; break;
    }
    if (this->battery_level_ != nullptr)
      this->battery_level_->publish_state(value);
  }
  if (this->read_register(IP5306_REG_READ0, &data, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "unable to read status");
    this->status_set_warning();
    return;
  }
  if (this->charger_connected_ != nullptr)
    this->charger_connected_->publish_state(data & CHARGE_ENABLE_BIT);
  if (this->read_register(IP5306_REG_READ1, &data, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "unable to read status");
    this->status_set_warning();
    return;
  }
  if (this->charge_full_ != nullptr)
    this->charge_full_->publish_state(data & CHARGE_FULL_BIT);
}

}  // namespace ip5306
}  // namespace esphome

