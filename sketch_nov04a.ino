
#include "driver.h"
#include "ssd1306.h"
#include "font.h"
#include "ADC_Conversion.h"
#include"pwm_time.h"


#define V_devider 18
#define Raw_to_user  (2.4/1023.0)
#define cell3_diff   (cell1V+cell2V)

#define firstcell   4.2
#define secondcell  8.4
#define offset_duty_cycle 0.5

volatile uint16_t duty_cycle_timer = 512;
uint8_t setpoint= 1 ;

float cell1V = 0.00;          // sets variable for the voltage of the first cell
float cell2V = 0.00;          // sets variable for the voltage of the second cell
float cell3V = 0.00;          // sets variable for the voltage of the third cell
float calV = 0.00;            // sets variable for supply voltage compensation
float supplyV = 0.00;         // sets variable for the voltage of the supply to the arduino
float cell1Cal = 80.00 + 20;  // allows for voltage calibration of the first cell
float cell2Cal = 70.00 + 25;  // allows for voltage calibration of the second cell
float cell3Cal = 80.00 + 15;  // allows for voltage calibration of the third cell
float zenerV = 2.4;           // measured voltage of the voltage reference
float cell1AverageVal = 0.00; // sets variable for use in calculating the average voltage of cell 1
float cell2AverageVal = 0.00; // sets variable for use in calculating the average voltage of cell 2
float cell3AverageVal = 0.00; // sets variable for use in calculating the average voltage of cell 3
float cell1AverageV = 0.00;   // sets variable for the current voltage of cell 1
float cell2AverageV = 0.00;   // sets variable for the current voltage of cell 2
float cell3AverageV = 0.00;   // sets variable for the current voltage of cell 3

float averageAmps = 0.00;         // set variable for the average amperage going into the battery pack
float ampsAverageVal = 0.00;      // variable used for calculating the average amperage
float ampSensorMillivolts = 0.00; // used for ACS 712 readings
float currentAmps = 0.00;         // used for the present amp reading
volatile float  ampsVal = 0;                // store the analog value for the ACS 712 without the offset
float ampsSensorVal = 0;          // store the analog value for the ACS 712 sensor
float Amps = 0;

float balanceVal = 4.0; // voltage where the balancing circuits kick in

float zenerVal = 0;   // variable used for analog reading of the voltage reference
float cell1Val = 0;   // variable used for analog reading of cell 1
float cell2Val = 0;   // variable used for analog reading of cell 2
float cell3Val = 0;   // variable used for analog reading of cell 3
float averages = 100; // sets the number of averages taken during each voltage Measurement

int cell1Bal = 2; // sets pin 2 as the output to control the balance circuit for cell 1
int cell2Bal = 3; // sets pin 3 as the output to control the balance circuit for cell 2
int cell3Bal = 4; // sets pin 4 as the output to control the balance circuit for cell 3
int flag = 0;     // sets up a flag for later use in cell balancing

int battery_flag = 0;
int battery_flag_1 = 0;
int battery_flag_2 = 0;
int battery_flag_3 = 0;
int pwm =1;

char v[10];
char p[10];
char d[10];
char e[20];

double offset = 0;
double sensor = 0.066;

void setup()
{

PinMode(PORTD,cell1Bal,OUTPUT);
PinMode(PORTD,cell2Bal,OUTPUT);
PinMode(PORTD,cell3Bal,OUTPUT);
PinMode(PORTB,pwm,OUTPUT);


PinMode(PORTC,3,INPUT);
PinMode(PORTC,6,INPUT);

WrtiePin(PORTC, 0, RESET);
WrtiePin(PORTC, 1, RESET);
WrtiePin(PORTC, 2, RESET);
// WrtiePin(PORTC, 3, RESET);
// WrtiePin(PORTC, 6, RESET);
// WrtiePin(PORTC, 7, RESET);



I2C_Init();
ssd1306_setup();
ssd1306_update();
I2C_Stop();
Timer_init();
 

ssd1306_Strings(1,10,"Cell1:");
ssd1306_Strings(1,20,"Cell2:");
ssd1306_Strings(1,30,"Cell3:");
ssd1306_Strings(1,1,"Amps:");
ssd1306_update();


}

void loop()
{ 
// for(int i=0;i< averages;i++)
// {
// zenerVal = ADC_Configuration(ADC3);                     //read the analog value for the voltage reference
// supplyV = (zenerV * 1023.00) / zenerVal;        //calculate the supply voltage of the arduino using the analog value from above
// calV = (supplyV - 5.00) * 10.00; 

sei();

cell1Val = ADC_Configuration(ADC0);
cell1V = (cell1Val*Raw_to_user)*V_devider;

dtostrf(cell1V,2,1,v);
ssd1306_Strings(37,10,v);
ssd1306_update();
delay(10);

float diffcel1 = 4.2 - cell1V;


cell2Val = ADC_Configuration(ADC1);
cell2V = ((cell2Val*Raw_to_user)*V_devider)-cell1V;

dtostrf(cell2V,2,1,p);
ssd1306_Strings(37,20,p);
ssd1306_update();
delay(10);

cell3Val = ADC_Configuration(ADC2);
cell3V = ((cell3Val*Raw_to_user)*V_devider)-cell3_diff;

dtostrf(cell3V,2,1,d);
ssd1306_Strings(37,30,d);
ssd1306_update();
delay(50);


ampsVal = ADC_Configuration(ADC6)-133;
volatile double tension = (ampsVal*11)/1023.0;
volatile double current = (tension - offset)/(sensor);
delay(10);

dtostrf(current,2,1,e);
ssd1306_Strings(35,1,e);
ssd1306_update();
Clean_ADC_reg();
delay(10);

if(current >= setpoint)
{
    duty_cycle_timer++;
}
else 
{
if(current < (setpoint-offset_duty_cycle))
{
    duty_cycle_timer--;
}
}
Duty_Cycle(duty_cycle_timer);

}
