
#include "stdafx.h"
#include "tserial.h"
#include "bot_control.h"
#include <Windows.h>


serial::serial() {
			
		 }
	
	bool serial::startDevice(char *port,int speed)
	{
		com = new Tserial();
		if (com!=0)
		{
			if(com->connect(port, speed, spNONE))
				printf("Not Connected...\n");
			else
				printf("connetced..\n");
			return TRUE;
		}
		else
			return FALSE; 
	}

	void serial::stopDevice()
	{
		com->disconnect();
        // ------------------
        delete com;
        com = 0;
	}

	void serial::send_data(unsigned char data)
	{
	//	unsigned char data = 0;
	
		
		com->sendChar(data);
		printf("%c",data);
		
	}