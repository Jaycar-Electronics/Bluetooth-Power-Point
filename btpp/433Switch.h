#ifndef _433_SWITCH_H__
#define _433_SWITCH_H__
#include "Arduino.h"
unsigned char reverseByte(unsigned char d)
{
  return ((d & 0x80) >> 7) | ((d & 0x40) >> 5) | ((d & 0x20) >> 3) | ((d & 0x10) >> 1) | ((d & 0x08) << 1) | ((d & 0x04) << 3) | ((d & 0x02) << 5) | ((d & 0x01) << 7);
}
class WirelessSwitch
{
private:
  const unsigned char outpin;
  const unsigned int shortpulse = 316;
  const unsigned int longpulse = 818;
  const unsigned int repeats = 20;
  const unsigned long address = 0x12340;
  const unsigned char rfcmds[10] = {
      //ALLON ALLOFF 1ON 1OFF 2ON 2OFF 3ON 3OFF 4ON 4OFF
      0x8, 0x4, 0xF, 0xE, 0xD, 0xC, 0xB, 0xA, 0x7, 0x6};

  void sendRF(unsigned long pk);
  unsigned long packetise(unsigned char c);
  unsigned char rfcrc(unsigned long d);
  void pulse(bool bit);

public:
  WirelessSwitch(const unsigned int pin) : outpin(pin)
  {
    pinMode(pin, OUTPUT);
  };
  ~WirelessSwitch(){};

  //by default, on/off will switch ALL, unless given a number.
  void on(unsigned char sw = 0);
  void off(unsigned char sw = 0);
};

void WirelessSwitch::off(unsigned char sw)
{
  this->sendRF(
      this->packetise(
          this->rfcmds[(sw * 2) + 1 % 10]));
}
void WirelessSwitch::on(unsigned char sw)
{
  this->sendRF(
      this->packetise(
          this->rfcmds[(sw * 2) % 10]));
}
unsigned long
WirelessSwitch::packetise(unsigned char c)
{
  unsigned long p;
  unsigned char cx;
  p = ((this->address & 0xFFFFF) << 4) | (c & 0xF);
  cx = this->rfcrc(p);
  p = (p << 8) | cx;
  return p;
}

unsigned char WirelessSwitch::rfcrc(unsigned long d)
{
  unsigned char a, b, c;
  a = reverseByte(d >> 16);
  b = reverseByte(d >> 8);
  c = reverseByte(d);
  return reverseByte(a + b + c);
}

void WirelessSwitch::sendRF(unsigned long pk)
{
  for (unsigned int r = 0; r < this->repeats; r++)
  {
    for (unsigned long i = 0x80000000UL; i > 0; i >>= 1)
    {
      this->pulse(pk & i);
    }
    // 3 more low bits
    this->pulse(0);
    this->pulse(0);
    this->pulse(0);
    //delay before repeating again
    delay(2);
  }
}
void WirelessSwitch::pulse(bool bit)
{
  digitalWrite(this->outpin, 1);
  delayMicroseconds(bit ? this->longpulse : this->shortpulse);
  digitalWrite(this->outpin, 0);
  delayMicroseconds(bit ? this->shortpulse : this->longpulse);
}
#endif