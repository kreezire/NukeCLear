#pragma once
#include "stdafx.h"
#include "tserial.h"
#include <Windows.h>
class serial{

  private:
        
		Tserial *com;
	  
	

  public:


	serial() ;
	
	bool startDevice(char *port,int speed);

	void stopDevice();

	void send_data(unsigned char data);
};
