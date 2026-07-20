#include "charger_enable.h"

namespace esphome::ip5306 {

static const char *TAG = "ip5306_switch";

void ChargerEnableSwitch::write_state(bool state) {
  ESP_LOGE(TAG, "switch!!!!");
  ESP_LOGE(TAG, "this=%p", this);
  ESP_LOGE(TAG, "parent=%p", this->parent_);
  this->parent_->set_charger_enable(state);
  this->publish_state(state);
}

}  // namespace esphome::ip5306
