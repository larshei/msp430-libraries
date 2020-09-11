/* =========================================================
 *                       D S 1 8 2 0 o w n . c
 *
 *      Author: Lars Heinrichs           Date: Nov 2014
 *
 * Functions for using a DS1820 Temperature Sensor. Needs
 * the "OneWire.c - Lars Heinrichs" - or another OneWire
 * library (might need to adjust function names for OneWire
 * functionalities in that case.)
 * =========================================================
 */

#include <MS5803clean.h>
#include <I2C_UCB0.h>
#define append(x,y)		((x<<8) + y)

unsigned int *RXValues_Sensor; // Pointer to Receive Values
unsigned short values[32];		// 32 Byte array to store incoming I2C data
unsigned int Byte;						// byte count for I2C communication
long long Off, Sens;					// variables for MS5803 calculations (compensation 1. order)
int T2, Off2, Sens2;					// variables for MS5803 calculations (compensation 2. order)


void MS_SendCommand(short command);
void MS_SendAndRead(char command, unsigned char byteCount);
void MS_Calibration(sensor_properties *sensor);

/****************************************************************************
 * Sets up the UCB0 interface for communication via I2C
 * Master mode, 100kHz. Needs to be called once before using the bus
****************************************************************************/
void I2C_Setup_Sensor() // Initialise the I2C interface
{
	P3OUT |= BIT0 | BIT1;					// Auf High setzen
	P3SEL |= BIT0 | BIT1;					// I2C Lanes (P3.0-SDA und P3.1-SCL) (Auf peripheriegerät stellen)
	UCB0CTL1 |= UCSWRST;                 	// USCI im Reset Status festhalten
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;	// master + I2C + synchroner Modus
	UCB0CTL1 = UCSSEL_2 + UCSWRST;			// SMCLK = 1MHz, USCI nicht freigeben
	UCB0BR0 = 10;                  			// fSCL = SMCLK/10 = ~100kHz
	UCB0BR1 = 0;                   			// Nichts aktiverien, da kein krummer Teiler
	UCB0CTL1 &= ~UCSWRST;          			// USCI nicht mehr im Reset Status festhalten
	UCB0IE |= UCTXIE;                  		// Interrupts für Sendebuffer einschalten
	UCB0IE |= UCRXIE;               		// Interrupts für Empfangsbuffer einschalten
}

/****************************************************************************
 * Allocates memory and creates variables for all important inforamtion
 * of an MS5803. To access the added sensor, store the return value
 * - SensorType: set to 1
 * - SensorAdress: I2C 7 bit adress of the sensor
 * - calibrate: != 0 to immediately read the sensors calibration data (recomended)
****************************************************************************/
sensor_properties *newPressureSensor(short SensorType, short SensorAdress, short calibrate)
{	sensor_properties *temp;
	temp = malloc(sizeof(sensor_properties));// reserve memory
	if (temp == NULL) {return NULL;}	// not enough ram available, exit
	temp->SensorAdress = SensorAdress;	// store sensor adress
	temp->SensorType = SensorType;		// store sensor type (unused, might be useful if you add multiple sensors)

	if (calibrate){
		I2C0_SetSlaveAdress(SensorAdress);
		MS_Calibration(temp);	// calibrate the sensor
		sleep(100);
	}

	return temp;	// sensor has been added and calibrated, exit
}


/*****************************************************************
 * MS_Calibration(sensor_properties *sensor)
 * Return: -
 * Param:  sensor_properties *sensor: Pointer to a sensor handle
 *
 * Starts a sensor calibration data read, validates results
 * stores values in sensor->CalValues[]
 *****************************************************************/
void MS_Calibration(sensor_properties *sensor)
{	unsigned short i;
	unsigned short countRetry = 0;
	retryCalibrationMS:;			// jump for retrying the calibration
	unsigned short validationCounter = 0;
	I2C0_SendCommand(MS5803_RESET);	// initialize by sending a reset
	sleep(50);						// wait for MS to finish reseting

	RXValues_Sensor = (unsigned int*) values;
	for (i=0 ; i<6 ; i++)
	{	I2C0_SendAndRead(MS5803_PROM_1 + (i<<1), 2, (unsigned short*) values + 2*i);
		sleep(10);
	}

	sensor->CalValues[0] = append(values[0], values[1]);
	sensor->CalValues[1] = append(values[2], values[3]);
	sensor->CalValues[2] = append(values[4], values[5]);
	sensor->CalValues[3] = append(values[6], values[7]);
	sensor->CalValues[4] = append(values[8], values[9]);
	sensor->CalValues[5] = append(values[10], values[11]);

	for (i=0 ; i<6 ; i++)
	{	if (!(sensor->CalValues[i]==0x0000) && !(sensor->CalValues[i]==0xFFFF))
		{	validationCounter +=  1;
		}
	}
	if (validationCounter < 6)
	{	UART_Send("Calibration fail\r\n", sizeof "Calibration fail\r\n");
		sleep(2000);
		UART_Send("retry\r\n", sizeof "retry\r\n");
		sleep(2000);
		countRetry++;
		if (countRetry > 10)
		{	UART_Send("Cant calibrate, will sleep\r\n", sizeof "Cant calibrate, will sleep\r\n");
			while(1)
			{	sleep(20000);
			}
		}
		goto retryCalibrationMS;
	}
	else
	{	UART_Send("Calibration done\r\n", sizeof "Calibration done\r\n");
	}
}

/*****************************************************************
 * MS_Measure(sensor_properties *sensor)
 * Return: -
 * Param:  sensor_properties *sensor: Pointer to a sensor handle
 *
 * Starts temperature and pressure measuring, stores data in
 * sensor->uT and sensor->uT. Calculates pressure in Pascal
 * and Temperature in 0.01°C and stores the results in sensor->P
 * and sensor->T
 *****************************************************************/
void MS_Measure(sensor_properties *sensor)
{	I2C0_SetSlaveAdress(sensor->SensorAdress);
	MS_ReadTemperature(sensor);
	MS_calcTemperature(sensor);
	MS_ReadPressure(sensor);
	MS_calcPressure(sensor);
}

/*****************************************************************
 * MS_Measure(sensor_properties *sensor)
 * Return: -
 * Param:  sensor_properties *sensor: Pointer to a sensor handle
 *
 * Starts temperature and pressure measuring, stores data in
 * sensor->uT and sensor->uT. Calculates pressure in Pascal
 * and Temperature in 0.01°C and stores the results in sensor->P
 * and sensor->T
 *****************************************************************/
void MS_ReadTemperature(sensor_properties *sensor)
{	long tmp;
	I2C0_SendCommand(MS5803_CON_T_4096);
	sleep(40);
	I2C0_SendAndRead(MS5803_ADC_READ, 3, (unsigned short*) values);
	sleep(5);
	tmp = append(values[0], values[1]);
	sensor->uT = append(tmp, values[2]);
	__no_operation();
}

void MS_ReadPressure(sensor_properties *sensor)
{	long tmp;
	I2C0_SendCommand(MS5803_CON_P_4096);// + (Pres_overSampling<<1));
	sleep(40);
	I2C0_SendAndRead(MS5803_ADC_READ, 3, (unsigned short*) values);
	sleep(5);
	tmp = append(values[0], values[1]);
	tmp = append(tmp, values[2]);
	sensor->uP = tmp;
}

/*****************************************************************
 * MS_calcTemperature(sensor_properties *sensor)
 * Return: -
 * Param:  sensor_properties *sensor: Pointer to a sensor handle
 *
 * uses the raw temperature value sensor->uT to calculate
 * the temperature in 0.01°C. Stores the result in sensor->T
 *****************************************************************/
void MS_calcTemperature(sensor_properties *sensor){
	long long tmp;

	T2 = 0;
	Off2 = 0;
	Sens2 = 0;

	// Temperaturkompensation 1. Ordnung
	sensor->dT = sensor->uT - ((long)sensor->CalValues[4] << 8);
	tmp = ((long long)(sensor->dT) * sensor->CalValues[5])>>10;
	sensor->T = 2000 + (tmp >> 13);

	// Temperaturkopensation 2. Ordnung (siehe Datenblatt)
	if (sensor->T < 2000)
	{	// Low Temperature
		T2 = (long long)((sensor->dT>>1) * (sensor->dT>>1)) >> 29;
		Off2 = 3 * (sensor->T - 2000) * (sensor->T - 2000);
		Sens2 = 7 * (sensor->T - 2000) * (sensor->T - 2000) >> 3;
		// Very Low Temperature
		if (sensor->T < -1500)	{Sens2 = Sens2 + 2 * (sensor->T + 1500) * (sensor->T + 1500);}
	} else
	{	// High Temperature
		T2 = 0;
		Off2 = 0;
		Sens2 = 0;
		// Very High Temperature
		if (sensor->T > 4500)	{Sens2 = Sens2 - (((sensor->T + 4500) * (sensor->T + 4500))>>3) ;}
	}
	sensor->T = sensor->T - T2;
}


/*****************************************************************
 * MS_calcPressure(sensor_properties *sensor)
 * Return: -
 * Param:  sensor_properties *sensor: Pointer to a sensor handle
 *
 * uses the raw pressure value sensor->uP to calculate
 * the pressure in Pascal. Stores the result in sensor->P.
 * Make sure to calculate the temperature before calculating the
 * pressure
 *****************************************************************/
void MS_calcPressure(sensor_properties *sensor)
// Berechnungsvorschrifht aus Sensor Datenblatt
// press: Rohwert der Messung
{	Off = ((long long)sensor->CalValues[1] << 16) + (long long)(((sensor->CalValues[3]>>3)*(sensor->dT>>4)));
	Sens = ((long)sensor->CalValues[0] << 15) + (((sensor->CalValues[2]>>2) * sensor->dT) >> 6);
	sensor->P =  (long long)(((((sensor->uP>>6) * (Sens-Sens2)) >> 15) - (Off-Off2) ) >> 15);
}


/*****************************************************************
 * MS_MultiMeasure(int MeasureCount, sensor_properties *sensor, long *saveTo)
 * Return: 	(long) Sum of all measurements
 * Param: 	MeasureCount:
 * 			sensor_properties *sensor: Pointer to a sensor handle
 *
 * Not fully implemented!
 *****************************************************************/
long MS_MultiMeasure(int MeasureCount, sensor_properties *sensor, long *saveTo)
{	// returns the sum of all measurements
	// writes n=MeasureCount individual measurements to adresses of *saveTo to *saveTo+MeasureCount-1
	// needs a parameter 'sensor' (pointer, storing the result of an addNewSensor() )
	long returnMe = 0;
	int ctr;
	for (ctr=MeasureCount; ctr>0 ; ctr--)
	{	MS_Measure(sensor);
		*saveTo++ = sensor->T;
		returnMe += sensor->T;
		sleep(10);
	}
	sensor->T = sensor->T / MeasureCount;
	return returnMe;
}
