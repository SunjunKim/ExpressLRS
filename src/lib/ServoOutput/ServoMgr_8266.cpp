#if defined(PLATFORM_ESP8266)

#include <ServoMgr_8266.h>
#include <waveform_8266.h>

ServoMgr_8266::ServoMgr_8266(const uint8_t * const pins, const uint8_t outputCnt, uint32_t defaultInterval)
  : _pins(pins), _outputCnt(outputCnt), _refreshInterval(new uint16_t[outputCnt]), _activePwmChannels(0)
{
    for (uint8_t ch=0; ch<_outputCnt; ++ch)
    {
        _refreshInterval[ch] = defaultInterval;
    }
}

void ServoMgr_8266::initialize()
{
    for (uint8_t ch=0; ch<_outputCnt; ++ch)
    {
        const uint8_t pin = _pins[ch];
        if (pin != PIN_DISCONNECTED)
        {
            pinMode(pin, OUTPUT);
            digitalWrite(pin, LOW);
        }
    }
}

void ServoMgr_8266::writeMicroseconds(uint8_t ch, uint16_t valueUs)
{
    const uint8_t pin = _pins[ch];
    if (pin == PIN_DISCONNECTED)
        return;
    _activePwmChannels |= (1 << pin);
    startWaveform8266(pin, valueUs, _refreshInterval[ch] - valueUs);
}

void ServoMgr_8266::setRefreshInterval(uint8_t ch, uint16_t intervalUs)
{
    if (intervalUs != 0)
        _refreshInterval[ch] = intervalUs;
}

void ServoMgr_8266::stopPwm(uint8_t ch)
{
    const uint8_t pin = _pins[ch];
    if (pin == PIN_DISCONNECTED)
        return;
    _activePwmChannels &= ~(1 << pin);
    stopWaveform8266(pin);
}

void ServoMgr_8266::stopAllPwm()
{
    for (uint8_t ch=0; ch<_outputCnt; ++ch)
    {
        stopPwm(ch);
        const uint8_t pin = _pins[ch];
        digitalWrite(pin, LOW);
    }
    _activePwmChannels = 0;
}

void ServoMgr_8266::writeDigital(uint8_t ch, bool value)
{
    const uint8_t pin = _pins[ch];
    if (pin == PIN_DISCONNECTED)
        return;
    if (isPwmActive(ch))
    {
        stopPwm(ch);
        // Wait for the last edge, which is at most 1 cycle from now
        delay((_refreshInterval[ch] / 1000U) + 1);
    }
    digitalWrite(pin, value);
}

#endif