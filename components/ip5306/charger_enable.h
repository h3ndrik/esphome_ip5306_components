#pragma once

#include "esphome/components/switch/switch.h"
#include "./ip5306.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome::ip5306 {

class ChargerEnableSwitch final : public switch_::Switch, public Parented<IP5306> {
 public:
  ChargerEnableSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace esphome::ip5306
