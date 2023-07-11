
#include "./src/Pin_Config/driver.h"
#include "./src/Display/ssd1306.h"
#include "./src/ADC/ADC_Conversion.h"
#include "./src/PWM/pwm_time.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define V_devider 5.6
#define Raw_to_user (2.37 / 1023.0)
#define cell3_diff (cell1V + cell2V)

#define firstcell 4.2
#define secondcell 8.4
#define offset_duty_cycle 0.5
#define offset_cell1 0.0
#define offset_cell2 0.0
#define offset_cell3 0.0

unsigned long startMillis = millis();

uint16_t duty_cycle_timer = 1023;




float cell1AverageVal = 0.00; // sets variable for use in calculating the average voltage of cell 1
float cell2AverageVal = 0.00; // sets variable for use in calculating the average voltage of cell 2
float cell3AverageVal = 0.00; // sets variable for use in calculating the average voltage of cell 3
float panelAverage = 0.00;


float balanceVal = 4.0; // voltage where the balancing circuits kick in

float voltageLimit = 0.0; /// voltage limit for buck converter output
float currentLimit = 0.0;

float cell1Limit = 0.0;
float cell1and2Limit = 0.0;

uint8_t cell1Bal = 2;    // sets pin 2 as the output to control the balance circuit for cell 1
uint8_t cell2Bal = 3;    // sets pin 3 as the output to control the balance circuit for cell 2
uint8_t cell3Bal = 4;    // sets pin 4 as the output to control the balance circuit for cell 3
uint8_t flagbalance = 0; // flag sets to 1 whe battery balance circuit is activated to avoid oscilations
float averageBalance = 150.0;
uint8_t flagBalancecell1 = 0;
uint8_t flagBalancecell2 = 0;
uint8_t flagBalancecell3 = 0;
uint8_t flagbatteriescomplete = 0;
uint8_t emergencyFlag = 0;

uint8_t enterButton = 7;
uint8_t downButton = 3;
uint8_t backButton = 0;
uint8_t upButton = 4;
volatile uint8_t menuSelection = 0;
uint8_t actualStateupButton = 0;
uint8_t actualStatedownButton = 0;
uint8_t stateEnterButton = 0;
uint8_t stateStopButton = 0;
uint8_t startchargeFlag = 0;
volatile uint8_t stopCharge = 0;
uint8_t selectedMode = 0;
uint8_t configCharge = 1;



char r[5]={0};

void Chargin_Enable();
void Test_Batteries(void);

ISR(PCINT0_vect)
{
    stopCharge = 1;
}

void setup()
{

    // PinMode(PORTD,cell1Bal,OUTPUT);
    // PinMode(PORTD,cell2Bal,OUTPUT);
    // PinMode(PORTD,cell3Bal,OUTPUT);
    PinMode(PORTD, 6, OUTPUT);

    pinMode(cell1Bal, OUTPUT);
    pinMode(cell2Bal, OUTPUT);
    pinMode(cell3Bal, OUTPUT);

    PinMode(PORTB, upButton, INPUT);
    PinMode(PORTB, downButton, INPUT);
    PinMode(PORTB, backButton, INPUT);
    PinMode(PORTD, enterButton, INPUT);
    pinMode(enterButton, INPUT);
    pinMode(6, OUTPUT);

    WrtiePin(PORTB, upButton, SET);
    WrtiePin(PORTB, downButton, SET);
    WrtiePin(PORTB, backButton, SET);
    // WrtiePin(PORTD, enterButton, SET);
    digitalWrite(enterButton, HIGH);

    // PinMode(PORTB,9,OUTPUT); //// output pin 9 portb

    WrtiePin(PORTC, 0, RESET);
    WrtiePin(PORTC, 1, RESET);
    WrtiePin(PORTC, 2, RESET);

    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT0);

    sei();

    I2C_Init();
    ssd1306_setup();
    ssd1306_update();
    I2C_Stop();
    Timer_init();
    // Enable_Timer();
}

void loop()
{
    duty_cycle_timer = 1023;

    stateEnterButton = digitalRead(7);
    if (stateEnterButton != SET)
    {
        startchargeFlag = 1;
    }
    else
    {
        startchargeFlag = 0;
    }

    actualStateupButton = digitalRead(12);
    if (actualStateupButton != HIGH)
    {
        menuSelection++;
    }

    actualStatedownButton = digitalRead(11);
    if (actualStatedownButton != HIGH)
    {
        menuSelection--;
    }

    Init_Userinterface(menuSelection);

    if (menuSelection > 2)
    {
        menuSelection = 0;
    }

    if ((menuSelection == 0) && (startchargeFlag == 1))
    {
        Init_Chargeinterface();
        while (stopCharge == 0)
        {
            Chargin_Enable();
        }
        startchargeFlag = 0;
        stopCharge = 0;
        menuSelection = 0;
    }

    if ((menuSelection == 1) && (startchargeFlag == 1))
    {
        while ((selectedMode == 0) && (stopCharge == 0))
        {
            selectedMode = Init_BatterySetup();
        }
        configCharge = selectedMode;
        stopCharge = 0;
        menuSelection = 0;
        startchargeFlag = 0;
        selectedMode = 0;
    }

    if ((menuSelection == 2) && (startchargeFlag == 1))
    {
        while (stopCharge == 0)
        {
            Test_Batteries();
        }
        stopCharge = 0;
        startchargeFlag = 0;
        menuSelection = 0;
        emergencyFlag = 0;
    }
}
void Chargin_Enable()
{
    float cell1Val = 0; // variable used for analog reading of cell 1
    float cell2Val = 0; // variable used for analog reading of cell 2
    float cell3Val = 0; // variable used for analog reading of cell 3
    float conversion_voltage = 0.0;
    float cell1V = 0.00;          // sets variable for the voltage of the first celle
    float cell2V = 0.00;          // sets variable for the voltage of the second cell
    float cell3V = 0.00;          // sets variable for the voltage of the third cell

    char v[5]={0};
    char p[5]={0};
    char d[5]={0};
    char e[5]={0};
    char f[5]={0};

    float sensitivity = 0.185;
    float current = 0.0;
    float currentSum = 0.0;
    float diff = 2.37;
    float averageAmps = 0.00; // set variable for the average amperage going into the battery pack
    float ampsVal = 0;        // store the analog value for the ACS 712 without the offset
 

    switch (configCharge)
    {
    case 1:
        voltageLimit = 12.6;
        currentLimit = 1.25;
        cell1Limit = 4.2;
        cell1and2Limit = 4.2;
        break;
    case 2:
        voltageLimit = 12.6;
        currentLimit = 2.5;
        cell1Limit = 4.2;
        cell1and2Limit = 4.2;
        break;
    case 3:
        voltageLimit = 16.8;
        currentLimit = 1.25;
        cell1Limit = 8.4;
        cell1and2Limit = 4.2;
        break;
    case 4:
        voltageLimit = 16.8;
        currentLimit = 2.5;
        cell1Limit = 8.4;
        cell1and2Limit = 4.2;
        break;
    default:
        voltageLimit = 16.8;
        currentLimit = 2.5;
    }

    for (int i = 0; i < averageBalance; i++)
    {
        cell1Val = ADC_Configuration(ADC0);
        delay(1);
        cell1V = (cell1Val * Raw_to_user) * V_devider;
        cell1AverageVal = cell1AverageVal + cell1V;

        cell2Val = ADC_Configuration(ADC1);
        delay(1);
        cell2V = ((cell2Val * Raw_to_user) * V_devider) - cell1V;
        cell2AverageVal = cell2AverageVal + cell2V;

        cell3Val = ADC_Configuration(ADC2);
        delay(1);
        cell3V = ((cell3Val * Raw_to_user) * V_devider) - cell3_diff;
        cell3AverageVal = cell3AverageVal + cell3V;

        conversion_voltage = ((cell3Val * Raw_to_user) * V_devider);
        panelAverage = panelAverage + conversion_voltage;
        delay(1);

        ampsVal = ADC_Configuration(ADC3) + 263;
        delay(1);
        averageAmps = (diff - (ampsVal * (Raw_to_user))) / sensitivity;
        currentSum = currentSum + averageAmps;
    }
    current = (currentSum / averageBalance);

    if (((panelAverage / averageBalance)+0.4) > voltageLimit)
    {
        duty_cycle_timer++;
        constrain(duty_cycle_timer, 0, 1023);
    }
    else
    {
     if (current > currentLimit)
        {
            duty_cycle_timer++;
            constrain(duty_cycle_timer, 0, 1023);
        }
        else
        {
            duty_cycle_timer--;
            constrain(duty_cycle_timer, 0, 1023);
        }

    }

    dtostrf(((cell1AverageVal / averageBalance) + offset_cell1), 2, 1, v);
    ssd1306_Strings(0, 55, v, 1);
    ssd1306_update();

    dtostrf(((cell2AverageVal / averageBalance) + offset_cell2), 2, 1, p);
    ssd1306_Strings(28, 55, p, 1);
    ssd1306_update();

    dtostrf(((cell3AverageVal / averageBalance) + offset_cell3), 2, 1, d);
    ssd1306_Strings(56, 55, d, 1);
    ssd1306_update();

    dtostrf((panelAverage / averageBalance) + 0.4, 2, 1, f);
    ssd1306_Strings(95, 30, f, 1);
    ssd1306_update();

    dtostrf(current, 1, 1, e);
    ssd1306_Strings(35, 1, e, 1);
    ssd1306_update();

    flagBalancecell1 = 0;
    flagBalancecell2 = 0;
    flagBalancecell3 = 0;

    if ((((cell1AverageVal / averageBalance) + offset_cell1) > cell1Limit) && (((cell2AverageVal / averageBalance) + offset_cell2) > cell1and2Limit) && (((cell3AverageVal / averageBalance) + offset_cell3) > cell1and2Limit))
    {
        duty_cycle_timer = 1023;
    }

    if ((((cell3AverageVal / averageBalance) + offset_cell3) > cell1and2Limit) && (flagbatteriescomplete == 0))
    {
        WrtiePin(PORTD, cell3Bal, SET);
        flagbalance = 1;
        flagBalancecell3 = 1;
    }
    else
    {
        WrtiePin(PORTD, cell3Bal, RESET);
    }
    if ((((cell2AverageVal / averageBalance) + offset_cell2) > cell1and2Limit) && (flagbatteriescomplete == 0))
    {
        WrtiePin(PORTD, cell2Bal, SET);
        flagbalance = 1;
        flagBalancecell2 = 1;
    }
    else
    {
        WrtiePin(PORTD, cell2Bal, RESET);
    }
    if ((((cell1AverageVal / averageBalance) + offset_cell1) > cell1Limit) && (flagbatteriescomplete == 0))
    {
        WrtiePin(PORTD, cell1Bal, SET);
        flagbalance = 1;
        flagBalancecell1 = 1;
    }
    else
    {
        WrtiePin(PORTD, cell1Bal, RESET);
    }

    if ((flagBalancecell1 == 1) && (flagBalancecell2 == 1) && (flagBalancecell3 == 1))
    {
        flagbatteriescomplete = 1;
    }
    else
    {
        flagbatteriescomplete = 0;
    }

    if (flagbalance == 1)
    {
        delay(2000);
        WrtiePin(PORTD, cell1Bal, RESET);
        WrtiePin(PORTD, cell2Bal, RESET);
        WrtiePin(PORTD, cell3Bal, RESET);
        flagbalance = 0;
        delay(3000);
    }
    cell1AverageVal = 0;
    cell2AverageVal = 0;
    cell3AverageVal = 0;
    panelAverage = 0;
    currentSum = 0;
}

ISR(TIMER1_OVF_vect)
{
    OCR1A = 2 * duty_cycle_timer;
}

void Test_Batteries()
{
       uint8_t startCheck = 0;
    uint8_t statusCheck = 0;

    float cell1Freevoltage = 0.0;
    float cell2Freevoltage = 0.0;
    float cell3Freevoltage = 0.0;
    float cell1Loadvoltage = 0.0;
    float cell2Loadvoltage = 0.0;
    float cell3Loadvoltage = 0.0;
    float cell1Loadcurrent = 0.0;
    float cell2Loadcurrent = 0.0;
    float cell3Loadcurrent = 0.0;
    float cell1Freecurrent = 0.0;
    float cell2Freecurrent = 0.0;
    float cell3Freecurrent = 0.0;
    float loadResistance = 8.2;
    float freeResistance = 18000.0;
    float cell1Rinternal = 0.0;
    float cell2Rinternal = 0.0;
    float cell3Rinternal = 0.0;

    float cell1F =0.0;
    float cell2F =0.0;
    float cell3F =0.0;

   float cell1V =0.0;
    float cell2V =0.0;
    float cell3V =0.0;

    float cell1ADC = 0.0;
    float cell2ADC = 0.0;
    float cell3ADC = 0.0;

    float cell1Volt = 0.0;
    float cell2Volt = 0.0;
    float cell3Volt = 0.0;

    float cell1ADCLoad = 0.0;
    float cell2ADCLoad = 0.0;
    float cell3ADCLoad = 0.0;

    char a[10];
    char b[10];
    char c[10];

    char res1[10];
    char res2[10];
    char res3[10];

    ssd1306_clear();
    ssd1306_drawrectagle(4, 18, 1, 120, 7);
    ssd1306_Strings(1, 18, "Cell1", 0);
    ssd1306_Strings(1, 31, "Rb: ", 1);
    ssd1306_Strings(1, 52, "V: ", 1);

    ssd1306_Strings(43, 18, "Cell2", 0);
    ssd1306_Strings(43, 31, "Rb: ", 1);
    ssd1306_Strings(43, 52, "V: ", 1);

    ssd1306_Strings(83, 18, "Cell3", 0);
    ssd1306_Strings(83, 31, "Rb: ", 1);
    ssd1306_Strings(83, 52, "V: ", 1);

    ssd1306_drawlinev(45, 10, 1, 40);
    ssd1306_drawlinev(85, 10, 1, 40);
    ssd1306_update();

    

        for (int i = 0; i <= 500; i++)
        {
            cell1ADC = ADC_Configuration(ADC0);
            delay(1);
            cell1Volt = (cell1ADC * Raw_to_user) * V_devider;
            cell1AverageVal = cell1AverageVal + cell1Volt;

            cell2ADC = ADC_Configuration(ADC1);
            delay(1);
            cell2Volt = ((cell2ADC * Raw_to_user) * V_devider) - cell1Volt;
            cell2AverageVal = cell2AverageVal + cell2Volt;

            cell3ADC = ADC_Configuration(ADC2);
            delay(1);
            cell3Volt = ((cell3ADC * Raw_to_user) * V_devider) - (cell1Volt+cell2Volt);
            cell3AverageVal = cell3AverageVal + cell3Volt;
        }


        cell1Freevoltage = ((cell1AverageVal / 500) + offset_cell1);
        cell2Freevoltage = ((cell2AverageVal / 500) + offset_cell2);
        cell3Freevoltage = ((cell3AverageVal / 500) + offset_cell3);

        dtostrf(cell1Freevoltage, 2, 1, a);
        ssd1306_Strings(13, 52, a, 1);
        ssd1306_update();

        dtostrf(cell2Freevoltage, 2, 1, b);
        ssd1306_Strings(57, 52, b, 1);
        ssd1306_update();

        dtostrf(cell3Freevoltage, 2, 1, c);
        ssd1306_Strings(98, 52, c, 1);
        ssd1306_update();


    while (stopCharge == 0)
    {

   startCheck = digitalRead(7);

        if (startCheck != SET)
        {
            statusCheck = 1;

            cell1F = cell1Freevoltage;
            cell2F = cell2Freevoltage;
            cell3F = cell3Freevoltage;

        cell1AverageVal = 0.0;
        cell2AverageVal = 0.0;
        cell3AverageVal = 0.0;

            for (int i = 0; i <= 1000; i++)
            {
                digitalWrite(6, HIGH);

                cell1ADCLoad = ADC_Configuration(ADC0);
                delay(1);
                cell1V = (cell1ADCLoad * Raw_to_user) * V_devider;
                cell1AverageVal = cell1AverageVal + cell1V;

                cell2ADCLoad = ADC_Configuration(ADC1);
                delay(1);
                cell2V = ((cell2ADCLoad * Raw_to_user) * V_devider) - cell1V;
                cell2AverageVal = cell2AverageVal + cell2V;

                cell3ADCLoad = ADC_Configuration(ADC2);
                delay(1);
                cell3V = ((cell3ADCLoad * Raw_to_user) * V_devider) - cell3_diff;
                cell3AverageVal = cell3AverageVal + cell3V;
            }

            cell1Loadvoltage = cell1AverageVal / 1000;
            cell2Loadvoltage = cell2AverageVal / 1000;
            cell3Loadvoltage = cell3AverageVal / 1000;


            digitalWrite(6, LOW);
        }
        if (statusCheck == 1)
        {
            cell1Freecurrent = cell1F / freeResistance;
            cell1Loadcurrent = (cell1Loadvoltage / loadResistance)*3.0;
            cell1Rinternal = (cell1F - cell1Loadvoltage) / (cell1Loadcurrent - cell1Freecurrent);

            cell2Freecurrent = cell2F / freeResistance;
            cell2Loadcurrent = (cell2Loadvoltage /loadResistance)*3.0;
            cell2Rinternal = (cell2F - cell2Loadvoltage) / (cell2Loadcurrent - cell2Freecurrent);

            cell3Freecurrent = cell3F / freeResistance;
            cell3Loadcurrent = (cell3Loadvoltage / loadResistance)*3.0;
            cell3Rinternal = (cell3F - cell3Loadvoltage) / (cell3Loadcurrent - cell3Freecurrent);

        dtostrf(cell1Rinternal, 2, 1, res1);

        dtostrf(cell2Rinternal, 2, 1, res2);

        dtostrf(cell3Rinternal, 2, 1, res3);
       

        }

        if ((cell1Rinternal < 0.2) && (statusCheck == 1))
        {
            ssd1306_Strings(11, 42, "OK", 1);
            ssd1306_Strings(20, 31, res1, 1);
            ssd1306_update();
        }
        if ((cell1Rinternal >= 0.2) && (statusCheck == 1))
        {
            ssd1306_Strings(11, 40, "BAD", 1);
            ssd1306_Strings(20, 31, res1, 1);
            ssd1306_update();
        }

        if ((cell2Rinternal < 0.2) && (statusCheck == 1))
        {
            ssd1306_Strings(52, 42, "OK", 1);
            ssd1306_Strings(60, 31, res2, 1);
            ssd1306_update();
        }
        if ((cell2Rinternal >= 0.2) && (statusCheck == 1))
        {
            ssd1306_Strings(52, 40, "BAD", 1);
            ssd1306_Strings(60, 31, res2, 1);
            ssd1306_update();
        }

        if ((cell3Rinternal < 0.2) && (statusCheck == 1))
        {
            ssd1306_Strings(95, 42, "OK", 1);
            ssd1306_Strings(100, 31, res3, 1);
            ssd1306_update();
        }
        if ((cell3Rinternal >= 0.2) && (statusCheck == 1))
        {
            ssd1306_Strings(95, 40, "BAD", 1);
            ssd1306_Strings(100, 31, res3, 1);
            ssd1306_update();
        }

        cell1AverageVal = 0.0;
        cell2AverageVal = 0.0;
        cell3AverageVal = 0.0;
        statusCheck = 0;
    }
}