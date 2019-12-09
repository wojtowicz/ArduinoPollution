class TimerManager {
  private:
    uint32_t timer;
    uint32_t timerDefault;
    uint32_t timerWebFetcher;
    TimerManager();
  public:
    static TimerManager& getInstance();
    boolean isTimerDefaultExceeded();
    boolean isTimerWebFetcherExceeded();
    void setTimer(uint32_t timer);
    void setTimerDefault();
    void setTimerWebFetcher();
};
