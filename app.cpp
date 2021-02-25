#include <kernel.h>
#include <math.h>
#include "kernel_cfg.h"
#include "app.h"

#include "mbed.h"

#include "app_config.h"

#include "Zumo.h"
#include "Milkcocoa.h"


#define DELTA_T 0.005
#define KP 0.0
#define KI 0.0
#define KD 0.0
#define STOP_TANK 0
#define FOWR_TANK 1
#define BACK_TANK 2
#define LEFT_TANK 3
#define RIGHT_TANK 4
extern void onpush(MQTT::MessageData& md);
Serial pc(USBTX, USBRX);
Zumo zumo;

static int tank_sta=FOWR_TANK;
static int tank_sp_r=100;
static int tank_sp_l=100;
static int speed = 80;
static int steer = 0;

static int diff[2];
static float integral;

float pid(int irValue, int targValue){
	float p,i,d;

	diff[0] = diff[1];
	diff[1] = irValue - targValue;
	integral += (diff[0] + diff[1]) / 2.0 / DELTA_T;

	p = KP + diff[1];
	i = KI * integral;
	d = KD * (diff[1] - diff[0])/2.0*DELTA_T;

	float op = p+i+d;

	if(op >= 100){
		return 100;
	}else if(op < 0){
		return 0;
	}else{
		return op;
	}

}

void isrRx() {
    char ch;
    ch = pc.getc();         // 1文字受信バッファより取り出し
    pc.putc(ch);            // 送信
    if(ch == 's'){
    	tank_sta=STOP_TANK;
    	tank_sp_l = 0;
    	tank_sp_r = 0;
    }else if(ch == 'f'){
    	if(tank_sta == STOP_TANK){
    		tank_sp_l = 100;
    		tank_sp_r = 100;
    	}else if(tank_sp_l < 250 || tank_sp_r < 250){
			tank_sp_l += 10;
			tank_sp_r += 10;
    	}
    	tank_sta=FOWR_TANK;
    }else if(ch == 'b'){
    	if(tank_sta == STOP_TANK){
    		tank_sp_l = -100;
    		tank_sp_r = -100;
    	}else if(tank_sp_l > -250 || tank_sp_r > -250){
			tank_sp_l -= 10;
			tank_sp_r -= 10;
    	}
    	tank_sta=BACK_TANK;
    }
    else if(ch == 'l'){
		if(tank_sta == STOP_TANK){
			tank_sp_l = 100;
			tank_sp_r = -100;
		}else if(tank_sp_l < 250 || tank_sp_r > -250){
			tank_sp_l += 10;
			tank_sp_r -= 10;
		}
		tank_sta=BACK_TANK;
	}else if(ch == 'r'){
		if(tank_sta == STOP_TANK){
			tank_sp_l = -100;
			tank_sp_r = 100;
		}else if(tank_sp_l > -250 || tank_sp_r < 250){
			tank_sp_l -= 10;
			tank_sp_r += 10;
		}
		tank_sta=BACK_TANK;
	}
}

/*
void onpush(MQTT::MessageData& md)
{
	pc.printf("onpush\n\r");
    MQTT::Message &message = md.message;
    DataElement de = DataElement((char*)message.payload);
	int v = de.getInt("z");
}
*/
void task_main(intptr_t exinf) {
	int k;
	pc.baud(9600);
	IrBitField_T irbits;
	unsigned int irVal[6];
	int prevIrValLeft;
	int prevIrValCenter;
	int prevIrValRight;
	unsigned int target[6];
	pc.attach(isrRx, Serial::RxIrq);

	while(1){
		//zumo.readIr(irbits);
		//zumo.readAnalogIrValue(irVal);
		//short x,y,z;
		//zumo.getMagetism(&x,&y,&z);
		//int op = pid(250,250);
		//short px=-2160;
		//short py=6675;
		//pc.printf("%d %d %d\r\n",x,y,z);
		//k = pc.getc();
		//pc.printf("%d\r\n",k);
		//pc.putc(pc.getc());
		//pc.printf("hello \r\n");

		zumo.driveTank(tank_sp_r,tank_sp_l);
	}
}
