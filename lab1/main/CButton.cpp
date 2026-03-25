#include "CButton.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define BUTTON_ACTION_NONE 0
#define BUTTON_ACTION_DOWN 1
#define BUTTON_ACTION_UP 2

CButton::CButton(int port)
{
    m_pinNumber = static_cast<gpio_num_t>(port);

    gpio_reset_pin(m_pinNumber);
    gpio_set_direction(m_pinNumber, GPIO_MODE_INPUT);
    gpio_pullup_en(m_pinNumber);

    m_lastPinLevel = gpio_get_level(m_pinNumber);
    m_lastDebounceTime = esp_timer_get_time();
    m_stateTimestamp = m_lastDebounceTime;
}

int CButton::readAction(int currentPinLevel, int64_t now)
{
    if (currentPinLevel == m_lastPinLevel || (now - m_lastDebounceTime) < BUTTON_DEBOUNCE_MS * 1000)
        return BUTTON_ACTION_NONE;

    m_lastDebounceTime = now;
    m_lastPinLevel = currentPinLevel;

    return currentPinLevel == 1 ? BUTTON_ACTION_UP : BUTTON_ACTION_DOWN;
}

void CButton::setState(State newState)
{
    m_state = newState;
    m_stateTimestamp = esp_timer_get_time();
}

void CButton::tick()
{
    int64_t now = esp_timer_get_time();
    int currentPinLevel = gpio_get_level(m_pinNumber);
    int action = readAction(currentPinLevel, now);

    if (action == BUTTON_ACTION_DOWN)
        m_lastDownTime = now;
    else if (action == BUTTON_ACTION_UP)
        m_lastUpTime = now;

    switch (m_state)
    {
    case IDLE:
        if (action == BUTTON_ACTION_DOWN)
            setState(CLICKED);
        break;

    case CLICKED:
        if (action == BUTTON_ACTION_UP)
            setState(WAITING);
        else if ((now - m_stateTimestamp) >= LONG_PRESS_THRESHOLD_MS * 1000)
            setState(LONG_PRESS);
        break;

    case LONG_PRESS:
        if (action == BUTTON_ACTION_UP)
        {
            if (longPress)
                longPress();

            setState(IDLE);
        }
        break;

    case WAITING:
        if (action == BUTTON_ACTION_DOWN)
            setState(DOUBLE_CLICK);
        else if ((now - m_stateTimestamp) >= DOUBLE_CLICK_DELAY_MS * 1000)
        {
            if (singleClick)
                singleClick();

            setState(IDLE);
        }
        break;

    case DOUBLE_CLICK:
        if (action == BUTTON_ACTION_UP)
        {
            if (doubleClick)
                doubleClick();

            setState(IDLE);
        }
        break;
    }
}