#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define GPIO_MAP_SIZE 0x10000
#define GPIO_DATA_OFFSET 0x00
#define GPIO_TRI_OFFSET 0x04

void delay()
{
	int i;
	for ( i = 0; i < 10000000; i++) ;
}

void led_blink(void *led_ptr, unsigned led_offset)
{
	*((unsigned *)(led_ptr + led_offset)) =  0xff;
	delay();
	*((unsigned *)(led_ptr + led_offset)) = 0x00;
	delay();
}

int main(int argc, char *argv[])
{
	int fd;
	int btn_value , state, num_1, num_2, op, result;
	
	//Read from switches using /dev/mem
	fd = open ("/dev/mem", O_RDWR);
	if (fd < 1) {
		perror(argv[0]);
		return -1;
	}
	unsigned page_size = sysconf(_SC_PAGESIZE);
	
	unsigned  sw_addr 	= (0x41200000 & (~(page_size-1)));
	unsigned  sw_offset 	= 0x41200000 - sw_addr;
	void 			*sw_ptr;
	sw_ptr 		= mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, sw_addr);
	
	unsigned  btn_addr 	= (0x41210000 & (~(page_size-1)));
	unsigned  btn_offset = 0x41210000 - btn_addr;
	void 			*btn_ptr;
	btn_ptr 		= mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, btn_addr);
	
	unsigned  led_addr 	= (0x41220000 & (~(page_size-1)));
	unsigned  led_offset = 0x41220000 - led_addr;
	void 			*led_ptr;
	led_ptr 		= mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, led_addr);
	
	state = 0;
	while(1)
	{
		switch(state)
		{
			case 0:
				num_1 		= 0;
				num_2 		= 0;
				op 				= 0;
				result 		= 0;
				*((unsigned *)(led_ptr + led_offset)) = 0x00;
				state 			= 1;
				break;
				
			case 1:
				printf("Enter first number , press middle button to continue \r\n");
				
				while(*((unsigned *)(btn_ptr + btn_offset)) != 0x01)
					num_1 = *((unsigned *)(sw_ptr + sw_offset));
				
				led_blink(led_ptr, led_offset);
				led_blink(led_ptr, led_offset);
				led_blink(led_ptr, led_offset);
				printf("Num_1 :  %d \n", num_1);
				state = 2;
				break;
				
				case 2:
				printf("Enter second number , press middle button to continue \r\n");
				
				while(*((unsigned *)(btn_ptr + btn_offset)) != 0x01)
					num_2 = *((unsigned *)(sw_ptr + sw_offset));
				
				led_blink(led_ptr, led_offset);
				led_blink(led_ptr, led_offset);
				led_blink(led_ptr, led_offset);
				printf("Num_2 :  %d \n", num_2);
				state = 3;
				break;
				
				case 3:
				printf("Enter operand , press a button to continue \r\n");
				
				while(*((unsigned *)(btn_ptr + btn_offset)) == 0x00) ;
					op = *((unsigned *)(btn_ptr + btn_offset));
		
				led_blink(led_ptr, led_offset);
				led_blink(led_ptr, led_offset);
				led_blink(led_ptr, led_offset);
				state = 4;
				break;
				
    		case 4:
				if (op == 4)
				{
					printf("operand : + \r\n",  op);
					result = num_1 + num_2;
				}

				else if (op == 2)
				{
					printf("operand : - \r\n",  op);
					result = num_1 - num_2;
				}

				else if(op == 8)
				{
					printf("operand : * \r\n",  op);
					result = num_1 * num_2;
				}

				else if (op == 16)
				{
					printf("operand : / \r\n",  op);
					result  = num_1 / num_2;
				}

				else
					result = result;

				*((unsigned *)(led_ptr + led_offset)) =  result;
				printf("result = %d \n\r", result);
				printf("press middle button to restart\n");
				printf("press other four button at the same time to leave\n");
				state = 5;
    			break;
				
				case 5:
					while (1)
					{
						if (*((unsigned *)(btn_ptr + btn_offset)) == 0x01)
						{
							state = 0;
							led_blink(led_ptr, led_offset);
							led_blink(led_ptr, led_offset);
							led_blink(led_ptr, led_offset);
							break;
						}

						else if(*((unsigned *)(btn_ptr + btn_offset)) == 0x1e)
						{
							printf("bye bye\n");
							munmap(sw_ptr, page_size);
							munmap(btn_ptr, page_size);
							munmap(led_ptr, page_size);
							return 0;
						}
						else ;
					}
					break;

    		default:
    			break;
		}
	}
	
	return 0;
}


