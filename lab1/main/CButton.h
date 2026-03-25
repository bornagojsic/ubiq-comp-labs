#ifndef _CButton_h
#define _CButton_h

#include "driver/gpio.h"

#define BUTTON_DEBOUNCE_MS 50
#define CLICK_MIN_DURATION_MS 50
#define LONG_PRESS_THRESHOLD_MS 500
#define DOUBLE_CLICK_DELAY_MS 300

typedef void (*ButtonEventHandler)(void);

class CButton
{
public:
    CButton(int port);

    void attachSingleClick(ButtonEventHandler method) { singleClick = method; }
    void attachDoubleClick(ButtonEventHandler method) { doubleClick = method; }
    void attachLongPress(ButtonEventHandler method) { longPress = method; }

    void tick();

private:
    enum State
    {
        IDLE,
        CLICKED,
        LONG_PRESS,
        WAITING,
        DOUBLE_CLICK
    };

    State state = IDLE;

    ButtonEventHandler singleClick = nullptr;
    ButtonEventHandler doubleClick = nullptr;
    ButtonEventHandler longPress = nullptr;

    gpio_num_t m_pinNumber;
    int m_lastState;
    int64_t lastDebounceTime;

    int64_t stateTimestamp;
    int64_t lastDownTime;
    int64_t lastUpTime;

    int readAction(int current_state, int64_t now);
};

#endif