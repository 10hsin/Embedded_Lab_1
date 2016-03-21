#include "xparameters.h"
#include "xgpio.h"

#define reset 	0
#define data_1 	1
#define data_2 	2
#define operand 3
#define count 	4
#define done	5
//***********************************************************
// button 	mid 	= 1
//          left	= 4
//          down 	= 2
//			right 	= 8
//          up 		= 10
//***********************************************************

void delay()
{
	int i;
	for ( i = 0; i < 10000000; i++) ;
}

void led_blink(XGpio led)
{
	XGpio_DiscreteWrite(&led, 1, 0xff);
	delay();
	XGpio_DiscreteWrite(&led, 1, 0x00);
	delay();
}

int main (void)
{

    XGpio dip, push, led;
    int	state = reset;
    int word_1, word_2, op, result = 0;

    //Initialize switches
    XGpio_Initialize(&dip, XPAR_SW_8BIT_DEVICE_ID);
    //Set switches GPIO as input direction
    XGpio_SetDataDirection(&dip, 1, 0xffffffff);

    //Initialize buttons
    XGpio_Initialize(&push, XPAR_BTN_5BIT_DEVICE_ID);
    //Set buttons GPIO as input direction
    XGpio_SetDataDirection(&push, 1, 0xffffffff);

    //Initialize LEDs
    XGpio_Initialize(&led, XPAR_LED_8BIT_DEVICE_ID);
    //Set LEDs GPIO as output direction
    XGpio_SetDataDirection(&led, 1, 0xffffffff);

    while(1)
    {
    	switch (state)
    	{
    		case reset:
    			XGpio_DiscreteWrite(&led, 1, 0x00);
    			word_1 	= 0;
    			word_2 	= 0;
    			op 		= 0;
    			result 	= 0;
    			state 	= data_1;
    			break;

    		case data_1:
    			xil_printf("Enter first number, press middle button to continue \r\n");

    			while (XGpio_DiscreteRead(&push, 1) != 0x01)
        			word_1 = XGpio_DiscreteRead(&dip, 1);
    			state = data_2;
    			led_blink(led);
    			led_blink(led);
    			led_blink(led);
				xil_printf("word_1 : %d \r\n",  word_1);
    			break;

    		case data_2:
    			xil_printf("Enter second number, press middle button to continue \r\n");
    			while (XGpio_DiscreteRead(&push, 1) != 0x01)
        			word_2 = XGpio_DiscreteRead(&dip, 1);
    			state = operand;
    			led_blink(led);
    			led_blink(led);
    			led_blink(led);
				xil_printf("word_2 : %d \r\n",  word_2);
    			break;

    		case operand:
    			xil_printf("Enter operand \r\n");
    			while (XGpio_DiscreteRead(&push, 1) == 0x00);
        		op = XGpio_DiscreteRead(&push, 1);
    			state = count;
    			led_blink(led);
    			led_blink(led);
    			led_blink(led);
    			break;

    		case count:
				if (op == 4)
				{
					xil_printf("operand : + \r\n",  op);
					result = word_1 + word_2;
				}

				else if (op == 2)
				{
					xil_printf("operand : - \r\n",  op);
					result = word_1 - word_2;
				}

				else if(op == 8)
				{
					xil_printf("operand : * \r\n",  op);
					result = word_1 * word_2;
				}

				else if (op == 16)
				{
					xil_printf("operand : / \r\n",  op);
					result  = word_1 / word_2;
				}

				else
					result = result;

				XGpio_DiscreteWrite(&led, 1, result);
				xil_printf("result = %d \n\r", result);
				xil_printf("press middle button to restart \n");
				xil_printf("press other four buttons at the same time to end \n");
				state = done;
    			break;

			case done:
				while (1)
				{
					if (XGpio_DiscreteRead(&push, 1) == 0x01)
					{
						state = 0;
						led_blink(led);
						led_blink(led);
						led_blink(led);
						break;
					}

					else if(XGpio_DiscreteRead(&push, 1) == 0x1e)
						return 0;

					else ;
				}
				break;

    		default:
    			break;
    	}
    }
    return 0;
}
