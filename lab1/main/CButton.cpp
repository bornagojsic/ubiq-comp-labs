#include "CButton.h"
#include "driver/gpio.h"
#include "esp_timer.h"
// #include "esp_log.h"

#define BUTTON_ACTION_NONE 0
#define BUTTON_ACTION_DOWN 1
#define BUTTON_ACTION_UP 2

CButton::CButton(int port)
{
    m_pinNumber = static_cast<gpio_num_t>(port);

    gpio_reset_pin(m_pinNumber);
    gpio_set_direction(m_pinNumber, GPIO_MODE_INPUT);
    gpio_pullup_en(m_pinNumber);

    m_lastState = gpio_get_level(m_pinNumber);
    lastDebounceTime = esp_timer_get_time();

    state = IDLE;
    stateTimestamp = lastDebounceTime;
}

int CButton::readAction(int currentState, int64_t now)
{
    if (currentState != m_lastState)
    {
        if ((now - lastDebounceTime) >= BUTTON_DEBOUNCE_MS * 1000)
        {
            lastDebounceTime = now;

            if (currentState == 0 && m_lastState == 1)
            {
                m_lastState = currentState;
                return BUTTON_ACTION_DOWN;
            }
            else if (currentState == 1 && m_lastState == 0)
            {
                m_lastState = currentState;
                return BUTTON_ACTION_UP;
            }
        }
    }

    return BUTTON_ACTION_NONE;
}

void CButton::tick()
{
    int64_t now = esp_timer_get_time();
    int currentState = gpio_get_level(m_pinNumber);
    int action = readAction(currentState, now);

    switch (state)
    {
    case IDLE:
        if (action == BUTTON_ACTION_DOWN)
        {
            state = CLICKED;
            stateTimestamp = now;
            lastDownTime = now;
        }
        break;

    case CLICKED:
        if (action == BUTTON_ACTION_UP)
        {
            lastUpTime = now;
            state = WAITING;
            stateTimestamp = now;
        }
        else if ((now - stateTimestamp) >= LONG_PRESS_THRESHOLD_MS * 1000)
        {
            state = LONG_PRESS;
        }
        break;

    case LONG_PRESS:
        if (action == BUTTON_ACTION_UP)
        {
            if (longPress)
                longPress();

            state = IDLE;
        }
        break;

    case WAITING:
        if (action == BUTTON_ACTION_DOWN)
        {
            state = DOUBLE_CLICK;
        }
        else if ((now - stateTimestamp) >= DOUBLE_CLICK_DELAY_MS * 1000)
        {
            if (singleClick)
                singleClick();

            state = IDLE;
        }
        break;

    case DOUBLE_CLICK:
        if (action == BUTTON_ACTION_UP)
        {
            if (doubleClick)
                doubleClick();

            state = IDLE;
        }
        break;
    }
}