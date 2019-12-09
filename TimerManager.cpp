#include <ESP8266WiFi.h>
#include "TimerManager.h"

TimerManager::TimerManager() {
  this->timer = 0;
  this->timerDefault = 0;
  this->timerWebFetcher = 0;
}

TimerManager& TimerManager::getInstance() {
  static TimerManager instance;
  return instance;
}

boolean TimerManager::isTimerDefaultExceeded(){
  return this->timer - this->timerDefault >= 5000;
}

boolean TimerManager::isTimerWebFetcherExceeded(){
  return this->timer - this->timerWebFetcher >= 30000;
}

void TimerManager::setTimer(uint32_t timer){
  this->timer = timer;
}

void TimerManager::setTimerDefault(){
  this->timerDefault = this->timer;
}

void TimerManager::setTimerWebFetcher(){
  this->timerWebFetcher = this->timer;
}
