#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace ip5306 {


enum class RegisterMap : uint8_t {
  SYS_CTL0 = 0x00,
  SYS_CTL1 = 0x01,
  SYS_CTL2 = 0x02,
  CHG_CTL0 = 0x20,
  CHG_CTL1 = 0x21,
  CHG_CTL2 = 0x22,
  CHG_CTL3 = 0x23,
  CHG_DIG  = 0x24,
  READ0    = 0x70,  // charge en
  READ1    = 0x71,  // charge full
  READ2    = 0x72,  // load
  READ3    = 0x77,  // buttons
  LEVEL    = 0x78,  // bat level
};

union RegSysCtl0 {
  struct {
    bool button_shutdown : 1;     // 0 rw
    bool boost_output_enable : 1; // 1 rw
    bool power_on_load : 1;       // 1 rw
    bool reserved_3 : 1;          // 1
    bool charger_enable : 1;      // 1 rw
    bool boost_enable : 1;        // 1 rw
    bool reserved_6 : 1;          // 0
    bool reserved_7 : 1;          // 1
  };
  uint8_t raw;
};

union RegSysCtl1 {
  struct {
    bool low_battery_shutdown : 1;             // 1 rw
    bool reserved_1 : 1;                       // 0
    bool boost_after_vin_unplug : 1;           // 1 rw 
    bool reserved_3 : 1;
    bool reserved_4 : 1;
    bool short_press_boost_switch : 1;         // 0 rw
    bool flashlight_ctrl_signal_selection : 1; // 0 rw
    bool boost_ctrl_signal_selection : 1;      // 0 rw
  };
  uint8_t raw;
};

enum class ShutdownTime : uint8_t {
  SHUTDOWN_8s  = 0b00,
  SHUTDOWN_32s = 0b01,
  SHUTDOWN_16s = 0b10,
  SHUTDOWN_64s = 0b11,
};

union RegSysCtl2 {
  struct {
    uint8_t reserved_0_1 : 2;             // 00
    ShutdownTime light_load_shutdown_time : 2; // 00 rw
    bool long_press_time : 1;             // 0 rw
    uint8_t reserved_5_7 : 3;
  };
  uint8_t raw;
};

enum class ChargeStopVoltage : uint8_t {
  CUT_OFF_VOLTAGE_0  = 0b00,     // 4.14/4.26/4.305/4.35  V
  CUT_OFF_VOLTAGE_1  = 0b01,     // 4.17/4.275/4.32/4.365 V
  CUT_OFF_VOLTAGE_2  = 0b10,     // 4.185/4.29/4.335/4.38 V
  CUT_OFF_VOLTAGE_3  = 0b11,     // 4.2/4.305/4.35/4.395  V
};

union RegChgCtl0 {
  struct {
    ChargeStopVoltage charging_full_stop_voltage : 2; // 10 rw
    uint8_t reserved_2_7 : 6;
  };
  uint8_t raw;
};

enum class StopCurrent : uint8_t {
  CURRENT_200  = 0,
  CURRENT_400  = 1,
  CURRENT_500  = 2,
  CURRENT_600  = 3,
};

enum class VoltageSetting : uint8_t {
  VOUT_4_45 = 0,
  VOUT_4_5  = 1,
  VOUT_4_55 = 2,
  VOUT_4_6  = 3,
  VOUT_4_65 = 4,
  VOUT_4_7  = 5,
  VOUT_4_75 = 6,
  VOUT_4_8  = 7,
};

union RegChgCtl1 {
  struct {
    uint8_t reserved_0_1 : 2;
    VoltageSetting charge_under_voltage_loop : 3; // 101 rw
    uint8_t reserved_5 : 1;
    StopCurrent end_charge_current_detection : 2; // 01 rw
  };
  uint8_t raw;
};

enum class BatteryVoltage : uint8_t {
  BAT_VOLTAGE_4_4  = 3,
  BAT_VOLTAGE_4_35 = 2,
  BAT_VOLTAGE_4_3  = 1,
  BAT_VOLTAGE_4_2  = 0,
};

enum class VoltagePressure : uint8_t {
  PRESSURIZED_42  = 3,
  PRESSURIZED_28  = 2,
  PRESSURIZED_14  = 1,
  PRESSURIZED_0   = 0,
};

union RegChgCtl2 {
  struct {
    VoltagePressure voltage_pressure : 2; // 01 rw
    BatteryVoltage battery_voltage : 2;  // 00 rw
    uint8_t reserved_4_7 : 4;     // 0000
  };
  uint8_t raw;
};

union RegChgCtl3 {
  struct {
    uint8_t reserved_0_4 : 5;
    bool charging_constant_current_loop_selection : 1; // 1 rw 
    uint8_t reserved_6_7 : 2;
  };
  uint8_t raw;
};

union RegChgDig {
  struct {
    uint8_t charger_vin_end_current : 5; // rw 
    uint8_t reserved_5_7 : 3;
  };
  uint8_t raw;
};

union RegRead0 {
  struct {
    bool reserved_0_2 : 3;
    bool charge_enable : 1;
    bool reserved_4_7 : 4;
  };
  uint8_t raw;
};

union RegRead1 {
  struct {
    bool reserved_0_2 : 3; // r
    bool battery_full : 1; // r
    bool reserved_4_7 : 4; // r
  };
  uint8_t raw;
};

union RegRead2 {
  struct {
    bool reserved_0_1 : 2; // r
    bool load_level : 1;   // r
    bool reserved_3_7 : 5; // r
  };
  uint8_t raw;
};

union RegRead3 {
  struct {
    bool short_press_detect : 1;  // 0 rw
    bool long_press_detect : 1;   // 0 rw
    bool double_click_detect : 1; // 0 rw
    bool reserved_3_7 : 5;
  };
  uint8_t raw;
};

enum class ChargeLevel : uint8_t {
  LEVEL_0   = 0b1111,
  LEVEL_25  = 0b1110,
  LEVEL_50  = 0b1100,
  LEVEL_75  = 0b1000,
  LEVEL_100 = 0b0000,
};

union RegLevel {
  struct {
    uint8_t reserved_0_3 : 4;
    ChargeLevel charge_level : 4;
  };
  uint8_t raw;
};




class IP5306 : public sensor::Sensor, public switch_::Switch, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;

  float get_setup_priority() const override;

  void set_battery_level(sensor::Sensor *sensor) { this->battery_level_ = sensor; }
  void set_charger_connected(binary_sensor::BinarySensor *sensor) { this->charger_connected_ = sensor; }
  void set_charge_full(binary_sensor::BinarySensor *sensor) { this->charge_full_ = sensor; }
  void set_output_load_sensor(text_sensor::TextSensor *sensor) { this->output_load_ = sensor; }
  void set_charger_enable_switch(switch_::Switch *sw) { this->charger_enable_ = sw; }

  void set_battery_voltage(BatteryVoltage voltage);
  void set_voltage_pressure(VoltagePressure pressure);

  void set_light_load_shutdown_time(ShutdownTime time);

 protected:
  sensor::Sensor *battery_level_{nullptr};
  binary_sensor::BinarySensor *charger_connected_{nullptr};
  binary_sensor::BinarySensor *charge_full_{nullptr};
  text_sensor::TextSensor *output_load_{nullptr};
  switch_::Switch *charger_enable_{nullptr};
  void write_state(bool state) override;
  struct {
    RegSysCtl0 reg_sys_ctl0;
    RegSysCtl1 reg_sys_ctl1;
    RegSysCtl2 reg_sys_ctl2;
    RegChgCtl2 reg_chg_ctl2;
    RegRead0 reg_read0;
    RegRead1 reg_read1;
    RegRead2 reg_read2;
    RegLevel reg_level;
  } status_{};
};

}  // namespace ip5306
}  // namespace esphome

