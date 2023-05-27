#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Coolix.h>

// Enumerations for various states of the AC.
enum class State { ON, OFF };
enum class Mode { COOL, DRY, HEAT, AUTO };
enum class FanSpeed { AUTO, MIN, MED, HI };

// Abstract base class for climate control system.
class ClimateControl {
public:
  // Pure virtual functions to be overridden in derived classes.
  virtual void setState() = 0;
  virtual void stateFromString(const String &s) = 0;
  virtual uint8_t modeToCmd() = 0;
  virtual void modeFromString(const String &m) = 0;
  virtual uint8_t fanSpeedToCmd() = 0;
  virtual void fanSpeedFromString(const String &f) = 0;
  virtual void setTemp(int t) = 0;
  virtual void sendCmd() = 0;
};

// Derived class for specific implementation of the climate control system.
class ClimateControlImpl : public ClimateControl {
private:
  // Private variables to store current state, temperature, mode, and fan speed.
  State state = State::ON;
  int temp = 20;
  Mode mode = Mode::COOL;
  FanSpeed fanspeed = FanSpeed::AUTO;
  IRCoolixAC ac;

public:
  // Constructor to initialize the system.
  ClimateControlImpl() : ac(15) {
    state = State::ON;
    temp = 24;
    mode = Mode::COOL;
    fanspeed = FanSpeed::AUTO;
    ac.begin();
  }

  // Override the setState function, set the AC state to ON or OFF.
  void setState() override {
    switch (state) {
    case State::ON:
      ac.on();
      break;
    case State::OFF:
      ac.off();
      break;
    }
  }

  // Override the stateFromString function, set the AC state based on the input
  // string.
  void stateFromString(const String &s) override {
    if (s == "off")
      state = State::OFF;
    else
      state = State::ON;
    setState();
  }

  // Override the modeToCmd function, translate the AC mode to command.
  uint8_t modeToCmd() override {
    switch (mode) {
    case Mode::COOL:
      return kCoolixCool;
    case Mode::DRY:
      return kCoolixDry;
    case Mode::HEAT:
      return kCoolixHeat;
    }
    return kCoolixAuto;
  }

  // Override the modeFromString function, set the AC mode based on the input
  // string.
  void modeFromString(const String &m) override {
    if (m == "cool")
      mode = Mode::COOL;
    else if (m == "dry")
      mode = Mode::DRY;
    else if (m == "heat")
      mode = Mode::HEAT;
    else
      mode = Mode::AUTO;
    ac.setMode(modeToCmd());
  }

  // Override the fanSpeedToCmd function, translate the fan speed to command.
  uint8_t fanSpeedToCmd() override {
    switch (fanspeed) {
    case FanSpeed::AUTO:
      return kCoolixFanAuto0;
    case FanSpeed::MIN:
      return kCoolixFanMin;
    case FanSpeed::MED:
      return kCoolixFanMed;
    case FanSpeed::HI:
      return kCoolixFanMax;
    }
    return kCoolixFanAuto;
  }

  // Override the fanSpeedFromString function, set the fan speed based on the
  // input string.
  void fanSpeedFromString(const String &f) override {
    if (f == "min")
      fanspeed = FanSpeed::MIN;
    else if (f == "med")
      fanspeed = FanSpeed::MED;
    else if (f == "high")
      fanspeed = FanSpeed::HI;
    else
      fanspeed = FanSpeed::AUTO;
    ac.setFan(fanSpeedToCmd());
  }

  // Override the setTemp function, set the temperature within a range.
  void setTemp(int t) override {
    if (t < 17)
      t = 17;
    if (t > 30)
      t = 30;
    temp = t;
    ac.setTemp(temp);
  }

  // Override the sendCmd function, send the current settings to the AC.
  void sendCmd() override { ac.send(); }
};