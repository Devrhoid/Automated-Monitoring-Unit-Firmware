/*
 * Demo_Unit_02_May_2015.c
 * Syndicated from ReadSensorAndPostData
 * Created: 5/1/2015 8:33:19 AM
 *  Author: Devrhoid
 */ 


#define F_CPU 7372800UL
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Clock Definition
#define F_CPU 7372800UL

//Unit definition
unsigned char unit_id[20] = "uwi_wm_02,"; //Specific ID by which a unit is known
unsigned char unit_name[20] = "Post_Primary,";   //Name by which a unit is known, this does not have to be Specific.

//USART0 bit definitions
#define RXB80 1
#define TXB80 0
#define UPE0 2
#define OVR0 3
#define FE0 4
#define UDRE0 5
#define RXC0 7

//USART1 bit definitions
#define RXB81 1
#define TXB81 0
#define UPE1 2
#define OVR1 3
#define FE1 4
#define UDRE1 5
#define RXC1 7

//USART0 status check definitions
#define FRAMING_ERROR0 (1<<FE0)
#define PARITY_ERROR0 (1<<UPE0)
#define DATA_OVERRUN0 (1<<OVR0)
#define DATA_REGISTER_EMPTY0 (1<<UDRE0)
#define RX_COMPLETE0 (1<<RXC0)

//USART1 status check definitions
#define FRAMING_ERROR1 (1<<FE1)
#define PARITY_ERROR1 (1<<UPE1)
#define DATA_OVERRUN1 (1<<OVR1)
#define DATA_REGISTER_EMPTY1 (1<<UDRE1)
#define RX_COMPLETE1 (1<<RXC1)

//PORT and Pin Definitions
#define STATUS_DDR DDRA
#define STATUS_PORT PORTA
#define STAT_PIN 0
#define SUCCESS_PIN 1
#define MODEM_CNTRL_DDR DDRB
#define MODEM_CNTRL_PRT PORTB
#define ON_KEY 0
#define MODEM_USART 1
#define SENSOR_USART 0
#define SENSOR_BAUDRATE 0x17 //0x17 IS THE HEX representation for 19200 baud
#define MODEM_BAUDRATE 0x03 //0x03 IS THE HEX representation for 115200 baud

//Function Prototypes
void statusBlink(int amount);
void successBlink(int amount);
void delay_s(int seconds);
void configPorts(void);
void initialize_RX0TX0(unsigned char hex_baud);
void initialize_RX1TX1(unsigned char hex_baud);
void writeChar0(char c);
void writeChar1(char c);
void writeString(unsigned char chString[180], unsigned char usart);
char readChar0(void);
char readChar1(void);
void readLine(unsigned char string[80], unsigned char usart);
void readSensor(unsigned char sensor_data[80], unsigned char usart);
void encodeSpace(unsigned char data[180]);
void encodeColon(unsigned char data[180]);
void encodeFSlash(unsigned char data[180]);
void encodeComma(unsigned char data[180]);
void encodeData(unsigned char data[180]);
void powerModem(void);

//Global Variable declarations
char character, a, b, c, d, e = 0x00;
unsigned char my_data[180] = "";
//Function Definitions

void statusBlink(int amount)
{
	STATUS_PORT &= ~(1<<STAT_PIN);
	for(int i=0;i<amount;i++)
	{
		STATUS_PORT |= 1<<STAT_PIN;
		_delay_ms(500);
		STATUS_PORT &= ~(1<<STAT_PIN);
		_delay_ms(500);
	}
}


void successBlink(int amount)
{
	STATUS_PORT &= ~(1<<SUCCESS_PIN);
	for(int i=0;i<amount;i++)
	{
		STATUS_PORT |= 1<<SUCCESS_PIN;
		_delay_ms(500);
		STATUS_PORT &= ~(1<<SUCCESS_PIN);
		_delay_ms(500);
	}
}

void delay_s(int seconds)
{
	int i = 0;
	
	for(i=0;i<seconds;i++)
	{
		_delay_ms(1000);
	}
}

void configPorts()
{
	STATUS_DDR = 0xFF;
	MODEM_CNTRL_DDR = 0xFF;
	STATUS_PORT &= ~(0xFF);
	MODEM_CNTRL_PRT &= ~(0xFF);
}
void initialize_RX0TX0(unsigned char hex_baud)
{
	//Initializes both RX and TX of USART0 in Asynch mode with 8 bit data, no parity and 1 stop bit.
	UCSR0A=0x00;
	UCSR0B=0x18;
	UCSR0C=0x86;
	UBRR0H=0x00;
	UBRR0L=hex_baud;
	_delay_ms(300);
}

void initialize_RX1TX1(unsigned char hex_baud)
{
	//Initializes both RX and TX of USART1 in Asynch mode with 8 bit data, no parity and 1 stop bit.
	UCSR1A=0x00;
	UCSR1B=0x18;
	UCSR1C=0x86;
	UBRR1H=0x00;
	UBRR1L=hex_baud;
	_delay_ms(300);
}

void writeChar0(char c)
{
	while ((UCSR0A & DATA_REGISTER_EMPTY0)==0);
	UDR0=c;
}

void writeChar1(char c)
{
	while ((UCSR1A & DATA_REGISTER_EMPTY1)==0);
	UDR1=c;
}

void writeString(unsigned char chString[180], unsigned char usart)
{
	int i=0;
	while(chString[i] != 0x00) //Write until a NULL character is reached.
	//The function takes in a array of 50, if a string of 10 is sent then the null character will be at location the 11th location or chString[10]
	{
		if(usart == 0)
		{
			writeChar0(chString[i]);
		}
		else if(usart == 1)
		{
			writeChar1(chString[i]);
		}
		i++;
	}
}

char readChar0(void)
{
	char data;
	while (1)
	{
		while ((UCSR0A & RX_COMPLETE0)==0);
		data=UDR0;
		if ((UCSR0A & (FRAMING_ERROR0 | PARITY_ERROR0 | DATA_OVERRUN0))==0)
		return data;
	}
}

char readChar1(void)
{
	char data;
	while (1)
	{
		while ((UCSR1A & RX_COMPLETE1)==0);
		data=UDR1;
		if ((UCSR1A & (FRAMING_ERROR1 | PARITY_ERROR1 | DATA_OVERRUN1))==0)
		return data;
	}
}

void readLine(unsigned char string[80], unsigned char usart)
{
	
	int i = 0;
	unsigned char ch = 0x00;
	//Read character from respective usart until NEW LINE character is encountered.
	do
	{
		if (usart == 0) //IF reading USART 0
		ch = readChar0();
		else if (usart == 1) //IF Reading USART1
		ch = readChar1();
		
		//If the character is neither a Carriage Return nor a New Line then add it to the string pointer and increment the counter
		if ((ch != 0x0D) && (ch != 0x0A))
		{
			string[i] = ch;
			i++;
		}
	}
	while(ch != 0x0A);
	string[i] = 0x00;  //Make the last character in the string a NULL character.
}

void readSensor(unsigned char sensor_data[80], unsigned char usart)
{
	
	unsigned char c = 0x00;
	if(usart == 0)
	{
		do
		{
			c = readChar0();
		}
		while (c != 0x23);	//Read Characters until '#' (0x23) is reached. Then read content after # as the data.
		readLine(sensor_data, usart);
	}
	else if(usart == 1)
	{
		do
		{
			c = readChar1();
		}
		while (c != 0x23);
		readLine(sensor_data, usart); //Read Characters until '#' (0x23) is reached. Then read content after # as the data.
	}
}

void encodeSpace(unsigned char data[180])
{
	unsigned char ch,i,j,len;
	unsigned char temp[180];

	len = strlen(data);
	j = 0;

	for(i=0;i<len;i++)
	{
		ch = data[i];
		
		if (ch != 0x20)
		{
			temp[j] = ch;
			j++;
		}
		else
		{
			temp[j] = 0x25;
			j++;
			temp[j] = 0x32;
			j++;
			temp[j] = 0x30;
			j++;
		}
	}
	temp[j] = 0x00;
	strcpy(data, temp);
}


void encodeFSlash(unsigned char data[180])
{
	unsigned char ch,i,j,len;
	unsigned char temp2[180];
	
	len = strlen(data);
	j = 0;
	
	for(i=0;i<len;i++)
	{
		ch = data[i];
		
		if (ch != 0x2F)
		{
			temp2[j] = ch;
			j++;
		}
		else
		{
			temp2[j] = 0x25;
			j++;
			temp2[j] = 0x32;
			j++;
			temp2[j] = 0x46;
			j++;
		}
		
	}
	temp2[j] = 0x00;
	strcpy(data, temp2);
}

void encodeColon(unsigned char data[180])
{
	unsigned char ch,i,j,len;
	unsigned char temp3[180];
	
	len = strlen(data);
	j = 0;
	
	for(i=0;i<len;i++)
	{
		ch = data[i];
		if (ch != 0x3A)
		{
			temp3[j] = ch;
			j++;
		}
		else
		{
			temp3[j] = 0x25;
			j++;
			temp3[j] = 0x33;
			j++;
			temp3[j] = 0x41;
			j++;
		}
		
	}
	temp3[j] = 0x00;
	strcpy(data, temp3);
}


void encodeComma(unsigned char data[180])
{
	unsigned char ch,i,j,len;
	unsigned char temp4[180];
	
	len = strlen(data);
	j = 0;
	
	for(i=0;i<len;i++)
	{
		ch = data[i];
		if (ch != 0x2C)
		{
			temp4[j] = ch;
			j++;
		}
		else
		{
			temp4[j] = 0x25;
			j++;
			temp4[j] = 0x32;
			j++;
			temp4[j] = 0x43;
			j++;
		}
		
	}
	temp4[j] = 0x00;
	strcpy(data, temp4);
}

void encodeData(unsigned char data[180])
{
	encodeSpace(data);
	encodeFSlash(data);
	encodeColon(data);
	encodeComma(data);
}

void uploadData(unsigned char data[180])
{
	
	writeString("AT\r",1); //Write "AT AND PRESS ENTER "to the modem
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CREG?\r\n", 1);
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CGATT?\r\n",1);
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIPSTATUS\r\n",1);
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIPMUX=0\r\n",1);
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CSTT=\"web.digiceljamaica.com\"\r\n",1);
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIICR\r\n",1);
	delay_s(5); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIPSTART=\"TCP\",\"104.237.143.204\",\"80\"\r\n",1);
	delay_s(5); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIPSHUT\r\n",1);
	delay_s(5); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIPSTART=\"TCP\",\"104.237.143.204\",\"80\"\r\n",1);
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	
	writeString("AT+CIPSEND\r",1);
	delay_s(1); //Wait for command execution
	//successBlink(2);
	
	
	writeString("GET /process_sensor_data.php?&sData=",1);
	writeString(data,1);
	writeString(" HTTP/1.1\r\n",1);
	writeString("Host: 104.237.143.204:80\r\n",1);
	writeString("Connection: keep-alive\r\n\r\n",1);
	writeChar1(0x0D);
	writeChar1(0x1A);
	
	delay_s(2); //Wait for command execution
	successBlink(2);
	
	writeString("AT+CIPCLOSE\r\n",1);
	delay_s(2); //Wait for command execution
	successBlink(2);
}

/*Power up reference from Arduino Library
void powerUp()
{
	pinMode(9, OUTPUT);
	digitalWrite(9,LOW);
	delay(1000);
	digitalWrite(9,HIGH);
	delay(2000);
	digitalWrite(9,LOW);
	delay(3000);
}

*/
void powerModem(void)
{
	MODEM_CNTRL_PRT &= ~(1<<ON_KEY);
	_delay_ms(1000);
	MODEM_CNTRL_PRT |= (1<<ON_KEY);
	delay_s(2);
	MODEM_CNTRL_PRT &= ~(1<<ON_KEY);
	delay_s(3);
}


int main(void)
{
	unsigned char dbString[180];
	configPorts();
	successBlink(5);
	initialize_RX0TX0(SENSOR_BAUDRATE);
	initialize_RX1TX1(MODEM_BAUDRATE);
	powerModem(); //Turn on the Modem
	successBlink(3); //Indicates Successful initialization
	delay_s(10);	//wait for stabalization (wait time may be adjusted.)
	writeString("GUI -on\r", SENSOR_USART);
	delay_s(2);
	writeString("LOG -OFF\r", SENSOR_USART);	//Disable loggin of data. No data is stored on the sensor.
	delay_s(2);
	writeString("SCR -I 120\r", SENSOR_USART); //Send data across every two minutes. 
	delay_s(2);
	successBlink(3);
	writeString("READ\r",SENSOR_USART); //Send a read command to initiate comm between sensor and MCU. 
	
	// Example of the Encoded data from the sensor. 
	//01%2Cuwi_wm_01%2CDATA%3A%2004%2F30%2F15%2C02%3A41%3A07%2C0.0%2C30.88%2C14510%2C55.0%2C6.63%2C35.71%2C12.34
	delay_s(10);
	writeString("AT\r",0); //Write "AT AND PRESS ENTER "to the computer
	delay_s(2);
	successBlink(1);
	
	delay_s(10);
	
	while(1)
	{
		//Code to read Data from sensor and post via Modem to DB
		
		writeString("READ\r",SENSOR_USART); //Issue a read command to the sensor. 
		readSensor(my_data, SENSOR_USART);	//Store the response in an array my_data. 
		//data to be posted  = unit_id+unit_name+sensor_data;
		strcpy(dbString,unit_id);
		strcat(dbString,unit_name);
		strcat(dbString,my_data);
		encodeData(dbString);	//URL Encode the data so it pass to the URL without issue. 
		uploadData(dbString);	
		statusBlink(1);
		delay_s(30);//need to make this about 5 minutes instead. Can do this with the timer.
	}
}

//Lastly we need to start the modem from the MCU
//Hardware Power Sequencing 

//Power Modem and Wait for network Latch
//It appears that the communication will work if both sensor and MCU
//are powered at the same time however it is a certainty that powering the sensor after the MCU
//ensures proper communication between the two devices. 

//Power Sensor.

//Smooth sailing from here on out. 

