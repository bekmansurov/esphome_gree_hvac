#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gree {

// enum SwingMode : uint8_t { SWING_OFF = 0, SWING_VERTICAL = 1, SWING_HORIZONTAL = 2, SWING_BOTH = 3 };

enum ac_mode: uint8_t {
  AC_MODE_OFF = 0x10,
  // auto 0-1-2-3
  AC_MODE_AUTO = 0x80,
  // cool 0-1-2-3
  AC_MODE_COOL = 0x90,
  // dry 1 (only) but set it to AUTO (0) for setting it to 1 later
  AC_MODE_DRY = 0xA0,
  // fanonly 0-1-2-3
  AC_MODE_FANONLY = 0xB0,
  // heat 0-1-2-3
  AC_MODE_HEAT = 0xC0
};

enum ac_fan: uint8_t {
  AC_FAN_AUTO = 0x00,
  AC_FAN_LOW = 0x01,
  AC_FAN_MEDIUM = 0x02,
  AC_FAN_HIGH = 0x03
};

// not implemented yet
enum ac_swing: uint8_t {
  AC_SWING_OFF = 0x44,
  AC_SWING_VERTICAL = 0x14,
  AC_SWING_HORIZONTAL = 0x41,
  AC_SWING_BOTH = 0x11
};

// not implemented yet
enum ac_louver_H: uint8_t {
  AC_LOUVERH_OFF = 0x00,
  AC_LOUVERH_SWING_FULL = 0x10,
  AC_LOUVERH_SWING_TOP = 0x20,
  AC_LOUVERH_SWING_ABOVEMIDDLE = 0x30,
  AC_LOUVERH_SWING_MIDDLE = 0x40,
  AC_LOUVERH_SWING_BELOWMIDDLE = 0x50,
  AC_LOUVERH_SWING_BOTTOM = 0x60,
  AC_LOUVERH_SWING_MIDDLE_TO_BOTTOM = 0x70,
  AC_LOUVERH_SWING_ABOVEMIDDLE_TO_BELOWMIDDLE = 0x90,
  AC_LOUVERH_SWING_MIDDLE_TO_TOP = 0xB0
};

#define GREE_START_BYTE 0x7E
#define GREE_RX_BUFFER_SIZE 52

union gree_start_bytes_t {
//     uint16_t u16;
    uint8_t u8x2[2];
};

struct gree_header_t
{
  gree_start_bytes_t start_bytes;
  uint8_t data_length;
};

struct gree_raw_packet_t
{
  gree_header_t header;
  uint8_t data[1]; // first data byte
};


/*
class Constants {
  public:
    // ac update interval in ms
    static const uint32_t AC_STATE_REQUEST_INTERVAL;
};
const uint32_t Constants::AC_STATE_REQUEST_INTERVAL = 300;
*/

class GreeClimate : public climate::Climate, public uart::UARTDevice, public PollingComponent {
 public:
  // void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;
  void control(const climate::ClimateCall &call) override;
  void set_supported_presets(const std::set<climate::ClimatePreset> &presets) { this->supported_presets_ = presets; }
  // void set_supported_swing_modes(const std::set<climate::ClimateSwingMode> &modes) {
  //   this->supported_swing_modes_ = modes;
  // }

 protected:
  climate::ClimateTraits traits() override;
  void read_state_(const uint8_t *data, uint8_t size);
  void send_data_(const uint8_t *message, uint8_t size);
  void dump_message_(const char *title, const uint8_t *message, uint8_t size);
  uint8_t get_checksum_(const uint8_t *message, size_t size);

 private:
  // uint32_t _update_period = Constants::AC_STATE_REQUEST_INTERVAL;

  // Parts of the message that must have specific values for "send" command.
  // These are not 0x00 and the meaning of those values is unknown at the moment.
  // Others set to 0x00
  // data_write_[41] = 12; // unknown but not 0x00. TODO
  uint8_t data_write_[47] = {0x7E, 0x7E, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t data_read_[GREE_RX_BUFFER_SIZE] = {0};

  bool receiving_packet_ = false;

  std::set<climate::ClimatePreset> supported_presets_{};
  // std::set<climate::ClimateSwingMode> supported_swing_modes_{};
};

}  // namespace gree
}  // namespace esphome
