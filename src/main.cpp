#include <Arduino.h>
#include "main.h"

// BluetoothSerial SerialBT;

// void getStatus(){
//     ECHOLN("getStatus");
//     CAN_frame_t tx_frame;
//     tx_frame.FIR.B.FF = CAN_frame_std;
//     tx_frame.MsgID = control_motor.device_id;
//     tx_frame.FIR.B.DLC = 4;
//     if(control_motor.forward){
//         tx_frame.data.u8[0] = 0;        //1 - open;  0 - close
//     }
//     else{
//         tx_frame.data.u8[0] = 1;        //1 - open;  0 - close
//     }
//     tx_frame.data.u8[1] = control_led.red_after;
//     tx_frame.data.u8[2] = control_led.green_after;
//     tx_frame.data.u8[3] = control_led.blue_after;
//     ESP32Can.CANWriteFrame(&tx_frame);
// }


void motor_init(){
    //set varialble
    control_motor.normal_mode = true;
    control_motor.forward = true;
    control_motor.daytay = true;
    control_motor.first_run = true;
    control_motor.pul_speed = 0;
    control_motor.pre_pul_speed = 0;
    control_motor.count_first_run = 0;
    control_motor.count_pul_FG = 0;

    if(EEPROM.read(EEPROM_DISTANT) != 255 && EEPROM.read(EEPROM_DISTANT) != 0){
        control_motor.is_save_distant = true;
        control_motor.count_pul_distant = EEPROM.read(EEPROM_DISTANT);
        ECHO("Distant = ");
        ECHOLN(control_motor.count_pul_distant);
    }else{
        control_motor.is_save_distant = false;
        ECHOLN("is_save_distant fasle!");
    }

    control_motor.speed_run_slow = char(EEPROM.read(EEPROM_SET_MODE_RUN_BEGIN));
    if(control_motor.speed_run_slow == 255){
        control_motor.speed_run_slow = 150;
        ECHOLN("read set moderun EEPROM fail, auto set 150");
    }else{
        ECHO("Mode Run =  ");
        ECHOLN(control_motor.speed_run_slow);
    }

    if(EEPROM.read(EEPROM_SET_TIME_RETURN) != 255 && EEPROM.read(EEPROM_SET_TIME_RETURN) != 0){
        control_motor.time_return = EEPROM.read(EEPROM_SET_TIME_RETURN);
        ECHO("time_return = ");
        ECHO(control_motor.time_return);
        ECHOLN("0 (ms)");
    }else{
        control_motor.time_return = 35;
        ECHOLN("isSetTimeReurn fasle, auto set 350(ms)");
    }


    if(EEPROM.read(EEPROM_SET_PERCENT_OUT_LOW_SPEED) != 255 && EEPROM.read(EEPROM_SET_PERCENT_OUT_LOW_SPEED) != 0
        && EEPROM.read(EEPROM_SET_PERCENT_IN_LOW_SPEED) != 255 && EEPROM.read(EEPROM_SET_PERCENT_IN_LOW_SPEED) != 0){
        control_motor.percent_low_in = EEPROM.read(EEPROM_SET_PERCENT_IN_LOW_SPEED);
        ECHO("percentLowSpeedIn = ");
        ECHOLN(control_motor.percent_low_in);

        control_motor.percent_low_out = EEPROM.read(EEPROM_SET_PERCENT_OUT_LOW_SPEED);
        ECHO("percentLowSpeedOut = ");
        ECHOLN(control_motor.percent_low_out);
    }else{
        control_motor.percent_low_in = 20;
        control_motor.percent_low_out = 20;
        ECHOLN("isSavePercentLowSpeed fasle, auto set 20");
    }

    //count time check analog sensor
    control_motor.time_delay_analog = EEPROM.read(EEPROM_COUNT_TIME_ANALOG);
    if(control_motor.time_delay_analog == 0 || control_motor.time_delay_analog == 255){
        control_motor.time_delay_analog = 2;
    }
    ECHO("count_check_analog_pin = ");
    ECHO(control_motor.time_delay_analog*100);
    ECHOLN("ms");

    //value error analog 
    control_motor.define_error_analog = EEPROM.read(EEPROM_VALUE_ERROR_ANALOG);
//    if(control_motor.define_error_analog == 0 || control_motor.define_error_analog == 255){
//        control_motor.define_error_analog = 100;
//    }
    ECHO("define Error Analog Read: ");
    ECHOLN(control_motor.define_error_analog);

    //time auto close
    control_motor.define_time_auto_close = EEPROM.read(EEPROM_TIME_AUTO_CLOSE);
    // if(definecontrol_motor.time_auto_close == 0){
    //     definecontrol_motor.time_auto_close = 10;       //10 min
    // }
    ECHO("Time Auto Close: ");
    ECHO(control_motor.define_time_auto_close);
    ECHOLN("min");

    //stop speed
    control_motor.stop_speed = EEPROM.read(EEPROM_MIN_STOP_SPEED);
    if(control_motor.stop_speed == 0 || control_motor.stop_speed == 255){
        control_motor.stop_speed = MINSPEED;
    }
    ECHO("Min stop speed: ");
    ECHOLN(control_motor.stop_speed);


    control_motor.value_error_analog = analogRead(ANALOG_READ_BUTTON);
    control_motor.pre_value_error_analog = control_motor.value_error_analog;


}

void led_init(){
    //set varialble
    control_led.count_change_led = 0;

    control_led.red_after = char(EEPROM.read(EEPROM_WIFI_LED_RED));
	control_led.green_after  = char(EEPROM.read(EEPROM_WIFI_LED_GREEN));
    control_led.blue_after  = char(EEPROM.read(EEPROM_WIFI_LED_BLUE));
    control_led.red_before  = control_led.red_after;
    control_led.green_before  = control_led.green_after;
    control_led.blue_before  = control_led.blue_after;
	ledcWrite(LED_CHANNEL_R, control_led.red_after);
    ledcWrite(LED_CHANNEL_G, control_led.green_after);
    ledcWrite(LED_CHANNEL_B, control_led.blue_after);
    ECHO("red: ");
    ECHOLN(control_led.red_after);
    ECHO("green: ");
    ECHOLN(control_led.green_after);
    ECHO("blue: ");
    ECHOLN(control_led.blue_after);
}


void Open(){
    // ECHOLN("open");

    control_motor.status_stop = false;
    control_motor.forward = true;
    control_motor.daytay = false;
    control_motor.count_calcu_speed = 0;
    control_led.status_led = true;
    digitalWrite(DIR, QUAY_THUAN);
    tickerCaculateSpeed.start();

	//bat den led
	if(control_led.count_change_led == 0){
		tickerSetPwmLedLightOff.stop();
		tickerSetPwmLedLightOn.start();
	}
}

void Close(){
    // ECHOLN("close");

    control_motor.status_stop = false;
    control_motor.forward = false;
    control_motor.daytay = false;
    control_motor.count_calcu_speed = 0;
    
    digitalWrite(DIR, QUAY_NGHICH);
    tickerCaculateSpeed.start();
    tickerSetPwmLedLightOn.stop();
}


void Stop(){
    tickerCaculateSpeed.stop();

    if(control_motor.forward){
        digitalWrite(DIR, QUAY_NGHICH);     //cho dong co quay nghich
        ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_MAX);
        delay(control_motor.time_return*10);
        ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_STOP);
        control_motor.forward = false;
    }else{
        digitalWrite(DIR, QUAY_THUAN);
        ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_MAX);
        delay(control_motor.time_return*10);
        ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_STOP);
        control_motor.forward = true;
    }

    control_motor.status_stop = true;
    control_motor.daytay = true;
    control_motor.count_calcu_speed = 0;

    //reset_value_analog
    control_motor.value_error_analog = analogRead(ANALOG_READ_BUTTON);
    control_motor.pre_value_error_analog = control_motor.value_error_analog;
    // ECHO("pre_value_error_analog: ");
    // ECHOLN(control_motor.pre_value_error_analog);
    // getStatus();


}


void caculateSpeed(){
    //ECHOLN("speed");
    if(control_motor.count_calcu_speed <= 30){
        control_motor.count_calcu_speed++;
    }
    control_motor.speed_velectory = (control_motor.pul_speed - control_motor.pre_pul_speed)/(0.1*6);
    control_motor.pre_pul_speed = control_motor.pul_speed;
    
    // ECHO("van toc: ");
    // ECHOLN(speed);
    if(abs(control_motor.speed_velectory) <= control_motor.stop_speed && control_motor.count_calcu_speed >= 5){   //sau 5 lan chay thi moi tinh den van toc
        ECHOLN("Da dung lai");
        tickerCaculateSpeed.stop();

        if(control_motor.is_save_distant && control_motor.first_run){
            control_motor.first_run = false;
            control_motor.count_pul_FG = 0;
            control_motor.pre_pul_speed = 0;
        }
        

        if(control_motor.first_run && control_motor.count_first_run < 5){
            control_motor.count_first_run++;
        }
        
        if(control_motor.first_run == true && control_motor.count_first_run == 1){
            control_motor.count_pul_FG = 0;
            control_motor.pre_pul_speed = 0;
        }

        else if(control_motor.first_run == true && control_motor.count_first_run == 2){
            control_motor.count_pul_distant = abs(control_motor.count_pul_FG);
            EEPROM.write(EEPROM_DISTANT, control_motor.count_pul_distant);
            EEPROM.commit();
            control_motor.is_save_distant = true;
            if(control_motor.count_pul_FG < 0){
                control_motor.count_pul_FG = 0;
                control_motor.pre_pul_speed = 0;
            }
            control_motor.first_run = false;
        }


        if(!control_motor.first_run && control_motor.count_pul_FG <= 3){
			//tat den
			tickerSetPwmLedLightOff.stop();
			tickerSetPwmLedLightOn.stop();
			control_led.count_change_led = 0;
			ledcWrite(LED_CHANNEL_R, 0);
            ledcWrite(LED_CHANNEL_G, 0);
            ledcWrite(LED_CHANNEL_B, 0);

			control_motor.count_pul_FG = 0;
        }else if(!control_motor.first_run && (control_motor.count_pul_distant -3) <= control_motor.count_pul_FG){
            //bat den
			tickerSetPwmLedLightOff.stop();
			tickerSetPwmLedLightOn.stop();
			control_led.count_change_led = 0;
			ledcWrite(LED_CHANNEL_R, uint8_t(control_led.red_after));
            ledcWrite(LED_CHANNEL_G, uint8_t(control_led.green_after));
            ledcWrite(LED_CHANNEL_B, uint8_t(control_led.blue_after));
			
			control_motor.count_pul_FG = control_motor.count_pul_distant;
        }
        
        control_motor.count_calcu_speed = 0;

        Stop();
    }
}



void setLedApMode() {
    digitalWrite(ledTestWifi, !digitalRead(ledTestWifi));
}

void setPwmLedLighton(){
    control_led.count_change_led++;
    float out_led_red, out_led_green, out_led_blue;
    out_led_red = (float)0 + (((float)control_led.red_after - (float)0)/255)*control_led.count_change_led;
    out_led_red = abs(out_led_red);
    ledcWrite(LED_CHANNEL_R, uint8_t(out_led_red));

    out_led_green = (float)0 + (((float)control_led.green_after - (float)0)/255)*control_led.count_change_led;
    out_led_green = abs(out_led_green);
    ledcWrite(LED_CHANNEL_G, uint8_t(out_led_green));

    out_led_blue = (float)0 + (((float)control_led.blue_after - (float)0)/255)*control_led.count_change_led;
    out_led_blue = abs(out_led_blue);
    ledcWrite(LED_CHANNEL_B, uint8_t(out_led_blue));

    if(control_led.count_change_led == 255){
        ECHOLN("On Led");
		control_led.count_change_led = 0;
	}

}

void setPwmLedLightoff(){
    control_led.count_change_led++;
	float out_led_red, out_led_green, out_led_blue;
    out_led_red = (float)control_led.red_before + (((float)0 - (float)control_led.red_before)/255)*control_led.count_change_led;
    out_led_red = abs(out_led_red);
	ledcWrite(LED_CHANNEL_R, uint8_t(out_led_red));
    
    out_led_green = (float)control_led.green_before + (((float)0 - (float)control_led.green_before)/255)*control_led.count_change_led;
    out_led_green = abs(out_led_green);
	ledcWrite(LED_CHANNEL_G, uint8_t(out_led_green));
    
    out_led_blue = (float)control_led.blue_before + (((float)0 - (float)control_led.blue_before)/255)*control_led.count_change_led;
    out_led_blue = abs(out_led_blue);
	ledcWrite(LED_CHANNEL_B, uint8_t(out_led_blue));
    
    if(control_led.count_change_led == 255){
        ECHOLN("Off Led");
		control_led.count_change_led = 0;
	}

}

void setPwmLedLightChange(){
    control_led.count_change_led++;
    float out_led_red, out_led_green, out_led_blue;
    out_led_red = (float)control_led.red_before + (((float)control_led.red_after - (float)control_led.red_before)/255)*control_led.count_change_led;
    out_led_red = abs(out_led_red);
    ledcWrite(LED_CHANNEL_R, uint8_t(out_led_red));

    out_led_green = (float)control_led.green_before + (((float)control_led.green_after - (float)control_led.green_before)/255)*control_led.count_change_led;
    out_led_green = abs(out_led_green);
    ledcWrite(LED_CHANNEL_G, uint8_t(out_led_green));

    out_led_blue = (float)control_led.blue_before + (((float)control_led.blue_after - (float)control_led.blue_before)/255)*control_led.count_change_led;
    out_led_blue = abs(out_led_blue);
    ledcWrite(LED_CHANNEL_B, uint8_t(out_led_blue));

    // ECHO(uint8_t(out_led_red));
    // ECHO("-----");
    // ECHO(uint8_t(out_led_green));
    // ECHO("-----");
    // ECHOLN(uint8_t(out_led_blue));
    if(control_led.count_change_led == 255){
        ECHOLN("Change Led");
		control_led.count_change_led = 0;
        control_led.red_before = control_led.red_after;
        control_led.green_before = control_led.green_after;
        control_led.blue_before = control_led.blue_after;
	}
}



void IRAM_ATTR dirhallSensor1(){      //nhan du lieu tu cam bien ben ngoai
    if(control_motor.sensor_hall_duplicate != 1){
        control_motor.sensor_hall_duplicate = 1;
        // ECHOLN("1");
        if(control_motor.forward){
            control_motor.count_pul_FG++;
        }else{
            control_motor.count_pul_FG--;
        }

        if(control_motor.daytay && control_motor.status_stop){
            // control_motor.sensor_hall_duplicate = 0;
            if(control_motor.hall_sensor_2 == true){
                //cho dong co chay thuan
                control_motor.hall_sensor_2 = false;
                // ECHOLN("open");
                Open();
            }
            else if(control_motor.hall_sensor_6 == true){
                //cho dong co chay nghich
                control_motor.hall_sensor_6 = false;
                // ECHOLN("close");
                Close();
            }
            else{
                control_motor.hall_sensor_1 = true;
            }
        }
    }
    
}

void IRAM_ATTR dirhallSensor2(){      //nhan du lieu tu cam bien ben ngoai
    if(control_motor.sensor_hall_duplicate != 2){
        control_motor.sensor_hall_duplicate = 2;
        // ECHOLN("2");
        if(control_motor.forward){
            control_motor.count_pul_FG++;
        }else{
            control_motor.count_pul_FG--;
        }

        if(control_motor.daytay && control_motor.status_stop){
            // control_motor.sensor_hall_duplicate = 0;
            if(control_motor.hall_sensor_3 == true){
                //cho dong co chay thuan
                control_motor.hall_sensor_3 = false;
                // ECHOLN("open");
                Open();
            }
            else if(control_motor.hall_sensor_1 == true){
                //cho dong co chay nghich
                control_motor.hall_sensor_1 = false;
                // ECHOLN("close");
                Close();
            }
            else{
                control_motor.hall_sensor_2 = true;
            }
        }
    }
    
}

void IRAM_ATTR dirhallSensor3(){      //nhan du lieu tu cam bien ben ngoai
    if(control_motor.sensor_hall_duplicate != 3){
        control_motor.sensor_hall_duplicate = 3;
        // ECHOLN("3");
        if(control_motor.forward){
            control_motor.count_pul_FG++;
        }else{
            control_motor.count_pul_FG--;
        }

        if(control_motor.daytay && control_motor.status_stop){
            // control_motor.sensor_hall_duplicate = 0;
            if(control_motor.hall_sensor_4 == true){
                //cho dong co chay thuan
                control_motor.hall_sensor_4 = false;
                // ECHOLN("open");
                Open();
            }
            else if(control_motor.hall_sensor_2 == true){
                //cho dong co chay nghich
                control_motor.hall_sensor_2 = false;
                // ECHOLN("close");
                Close();
            }
            else{
                control_motor.hall_sensor_3 = true;
            }
        }
    }
    
}

void IRAM_ATTR dirhallSensor4(){      //nhan du lieu tu cam bien ben ngoai
    if(control_motor.sensor_hall_duplicate != 4){
        control_motor.sensor_hall_duplicate = 4;
        // ECHOLN("4");
        if(control_motor.forward){
            control_motor.count_pul_FG++;
        }else{
            control_motor.count_pul_FG--;
        }

        if(control_motor.daytay && control_motor.status_stop){
            // control_motor.sensor_hall_duplicate = 0;
            if(control_motor.hall_sensor_5 == true){
                //cho dong co chay thuan
                control_motor.hall_sensor_5 = false;
                // ECHOLN("open");
                Open();
            }
            else if(control_motor.hall_sensor_3 == true){
                //cho dong co chay nghich
                control_motor.hall_sensor_3 = false;
                // ECHOLN("close");
                Close();
            }
            else{
                control_motor.hall_sensor_4 = true;
            }
        }
    }
    
}

void IRAM_ATTR dirhallSensor5(){      //nhan du lieu tu cam bien ben ngoai
    if(control_motor.sensor_hall_duplicate != 5){
        control_motor.sensor_hall_duplicate = 5;
        // ECHOLN("5");
        if(control_motor.forward){
            control_motor.count_pul_FG++;
        }else{
            control_motor.count_pul_FG--;
        }

        if(control_motor.daytay && control_motor.status_stop){
            // control_motor.sensor_hall_duplicate = 0;
            if(control_motor.hall_sensor_6 == true){
                //cho dong co chay thuan
                control_motor.hall_sensor_6 = false;
                // ECHOLN("open");
                Open();
            }
            else if(control_motor.hall_sensor_4 == true){
                //cho dong co chay nghich
                control_motor.hall_sensor_4 = false;
                // ECHOLN("close");
                Close();
            }
            else{
                control_motor.hall_sensor_5 = true;
            }
        }
    }
    
}

void IRAM_ATTR dirhallSensor6(){      //nhan du lieu tu cam bien ben ngoai
    if(control_motor.sensor_hall_duplicate != 6){
        control_motor.sensor_hall_duplicate = 6;
        // ECHOLN("6");
        if(control_motor.forward){
            control_motor.count_pul_FG++;
        }else{
            control_motor.count_pul_FG--;
        }

        if(control_motor.daytay && control_motor.status_stop){
            // control_motor.sensor_hall_duplicate = 0;
            if(control_motor.hall_sensor_1 == true){
                //cho dong co chay thuan
                control_motor.hall_sensor_1 = false;
                // ECHOLN("open");
                Open();
            }
            else if(control_motor.hall_sensor_5 == true){
                //cho dong co chay nghich
                control_motor.hall_sensor_5 = false;
                // ECHOLN("close");
                Close();
            }
            else{
                control_motor.hall_sensor_6 = true;
            }
        }
    }
    
}


void IRAM_ATTR inputSpeed(){
    if(control_motor.forward){
        control_motor.pul_speed++;
    }else{
        control_motor.pul_speed--;
    }
    // ECHOLN(control_motor.pul_speed);
}



void tickerupdate(){
    tickerCaculateSpeed.update();
    tickerSetApMode.update();
	tickerSetPwmLedLightOn.update();
	tickerSetPwmLedLightOff.update();
    tickerSetPwmLedLightChange.update();
}


void setSpeedControl(){
    float inside = ((float)control_motor.percent_low_in/100)*control_motor.count_pul_distant;
    float outside = ((100 - (float)control_motor.percent_low_out)/100)*control_motor.count_pul_distant;
    if(!control_motor.first_run && !control_motor.status_stop && control_motor.forward && control_motor.count_pul_FG < outside){
        ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_MAX);
        // ECHOLN("MOTOR_MODE_SPEED_MAX");
    }
    
    else if(!control_motor.first_run && !control_motor.status_stop && !control_motor.forward && control_motor.count_pul_FG > inside){
        ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_MAX);  
        // ECHOLN("MOTOR_MODE_SPEED_MAX"); 
    }
    
    else if(!control_motor.status_stop && (control_motor.first_run || (control_motor.forward && control_motor.count_pul_FG > outside) || (!control_motor.forward && control_motor.count_pul_FG < inside))){
        ledcWrite(MOTOR_CHANNEL, control_motor.speed_run_slow);
    }
    // else{
    //     ledcWrite(MOTOR_CHANNEL, MOTOR_MODE_SPEED_STOP);
    // }
}

String MacID(){
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    String macID;
    WiFi.mode(WIFI_AP);
    WiFi.softAPmacAddress(mac);
    macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    return macID;
}

void handleRoot(){
	char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
	server.send(200, "text/html", index_html);
}

void apiResetDistant(){
    EEPROM.write(EEPROM_DISTANT, 0);
    EEPROM.commit();
    control_motor.is_save_distant = false;
    control_motor.first_run = true;
    control_motor.count_first_run = 0;
    ECHOLN("resetDistant");
    char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
    server.send(200, "text/html", index_html);
}

void apiLowSpeed(){
    String lowSpeed;
    for (uint8_t i = 0; i < server.args(); i++) {
        if(server.argName(i) == "low_speed" && server.arg(i) != ""){
            lowSpeed = server.arg(i);
            control_motor.speed_run_slow = lowSpeed.toInt();
            ECHO("control_motor.speed_run_slow: ");
            ECHOLN(control_motor.speed_run_slow);
            EEPROM.write(EEPROM_SET_MODE_RUN_BEGIN, char(control_motor.speed_run_slow));
            EEPROM.commit();
            break;
        }
    }
    char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
    server.send(200, "text/html", index_html);
}

void apiTimeReturn(){
    String lowSpeed;
    for (uint8_t i = 0; i < server.args(); i++) {
        if(server.argName(i) == "time_return" && server.arg(i) != ""){
            lowSpeed = server.arg(i);
            control_motor.time_return = lowSpeed.toInt();
            ECHO("control_motor.time_return: ");
            ECHOLN(control_motor.time_return);
            EEPROM.write(EEPROM_SET_TIME_RETURN, char(control_motor.time_return));
            EEPROM.commit();
            break;
        }
    }
    char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
    server.send(200, "text/html", index_html);
}

void apiPercentLowIn(){
    String lowSpeed;
    for (uint8_t i = 0; i < server.args(); i++) {
        if(server.argName(i) == "percent_low_speed_in" && server.arg(i) != ""){
            lowSpeed = server.arg(i);
            control_motor.percent_low_in = lowSpeed.toInt();
            ECHO("control_motor.percent_low_in: ");
            ECHOLN(control_motor.percent_low_in);
            EEPROM.write(EEPROM_SET_PERCENT_IN_LOW_SPEED, char(control_motor.percent_low_in));
            EEPROM.commit();
            break;
        }
    }
    char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
    server.send(200, "text/html", index_html);
}

void apiPercentLowOut(){
    String lowSpeed;
    for (uint8_t i = 0; i < server.args(); i++) {
        if(server.argName(i) == "percent_low_speed_out" && server.arg(i) != ""){
            lowSpeed = server.arg(i);
            control_motor.percent_low_out = lowSpeed.toInt();
            ECHO("control_motor.percent_low_out: ");
            ECHOLN(control_motor.percent_low_out);
            EEPROM.write(EEPROM_SET_PERCENT_OUT_LOW_SPEED, char(control_motor.percent_low_out));
            EEPROM.commit();
            break;
        }
    }
    char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
    server.send(200, "text/html", index_html);
}

void apiMinStopStpped(){
    String lowSpeed;
    for (uint8_t i = 0; i < server.args(); i++) {
        if(server.argName(i) == "min_stop_speed" && server.arg(i) != ""){
            lowSpeed = server.arg(i);
            control_motor.stop_speed = lowSpeed.toInt();
            ECHO("control_motor.stop_speed: ");
            ECHOLN(control_motor.stop_speed);
            EEPROM.write(EEPROM_MIN_STOP_SPEED, char(control_motor.stop_speed));
            EEPROM.commit();
            break;
        }
    }
    char index_html[2048];
	snprintf_P(index_html, sizeof(index_html), index_html_handle_root,
                control_motor.speed_run_slow,
                control_motor.time_return,
                control_motor.percent_low_in,
                control_motor.percent_low_out,
                control_motor.stop_speed);
    server.send(200, "text/html", index_html);
}

void SetupConfigMode(){
    ECHOLN("[WifiService][setupAP] Open AP....");
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    server.close();
    delay(500);
    WiFi.mode(WIFI_AP_STA);
    String SSID_AP_MODE = SSID_PRE_AP_MODE + MacID();
    WiFi.softAP(SSID_AP_MODE.c_str(), PASSWORD_AP_MODE);
    IPAddress APIP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(APIP, gateway, subnet);
    ECHOLN(SSID_AP_MODE);

    ECHOLN("[WifiService][setupAP] Softap is running!");
    IPAddress myIP = WiFi.softAPIP();
    ECHO("[WifiService][setupAP] IP address: ");
    ECHOLN(myIP);
}


void StartConfigServer(){    
    ECHOLN("[HttpServerH][startConfigServer] Begin create new server...");
    server.on("/", HTTP_GET, handleRoot);
	server.on("/reset_distant", HTTP_GET, apiResetDistant);
    server.on("/low_speed", HTTP_GET, apiLowSpeed);
    server.on("/time_return", HTTP_GET, apiTimeReturn);
    server.on("/percent_low_speed_in", HTTP_GET, apiPercentLowIn);
    server.on("/percent_low_speed_out", HTTP_GET, apiPercentLowOut);
    server.on("/min_stop_speed", HTTP_GET, apiMinStopStpped);

    server.begin();
    ECHOLN("[HttpServerH][startConfigServer] HTTP server started");
}


void checkButtonConfigClick(){
    //hold to config mode
    if(digitalRead(PIN_CONFIG) == HIGH){
        configAPmode.time_click_button_config = millis();
    }
    if(digitalRead(PIN_CONFIG) == LOW && (configAPmode.time_click_button_config + CONFIG_HOLD_TIME) <= millis()){
        configAPmode.time_click_button_config = millis();
        tickerSetApMode.start();
        control_motor.normal_mode = false;
        SetupConfigMode();
        StartConfigServer();
        // EEPROM.write(EEPROM_DISTANT, 0);
        // EEPROM.commit();
        // control_motor.is_save_distant = false;
        // control_motor.first_run = true;
        // control_motor.count_first_run = 0;
        // ECHOLN("resetDistant");
    }
}

void setup() {
  // put your setup code here, to run once:
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);
    EEPROM.begin(EEPROM_WIFI_MAX_CLEAR);
    // analogSetClockDiv(255);
     
    ledcSetup(LED_CHANNEL_R, 1000, 8); // 1 kHz PWM, 8-bit resolution
    ledcSetup(LED_CHANNEL_G, 1000, 8); // 1 kHz PWM, 8-bit resolution
    ledcSetup(LED_CHANNEL_B, 1000, 8); // 1 kHz PWM, 8-bit resolution
    ledcSetup(MOTOR_CHANNEL, 30000, 8); // 30 kHz PWM, 8-bit resolution

    ledcAttachPin(PIN_LED_LIGHT_R, LED_CHANNEL_R); // analog pin to channel led_R
    ledcAttachPin(PIN_LED_LIGHT_G, LED_CHANNEL_G); // analog pin to channel led_G
    ledcAttachPin(PIN_LED_LIGHT_B, LED_CHANNEL_B); // analog pin to channel led_B
    ledcAttachPin(PWM, MOTOR_CHANNEL);              // analog pin to channel Motor
    
    ECHOLN("");
    ECHOLN("START!!!");
    pinMode(DIR, OUTPUT);
    pinMode(PIN_CONFIG, INPUT_PULLUP);
    pinMode(inputFG, INPUT_PULLUP);
    pinMode(hallSensor1, INPUT_PULLUP);
    pinMode(hallSensor2, INPUT_PULLUP);
    pinMode(hallSensor3, INPUT_PULLUP);
    pinMode(hallSensor4, INPUT_PULLUP);
    pinMode(hallSensor5, INPUT_PULLUP);
    pinMode(hallSensor6, INPUT_PULLUP);
    pinMode(ledTestWifi, OUTPUT);
    digitalWrite(ledTestWifi, HIGH);
    
    motor_init();
    led_init();


    attachInterrupt(digitalPinToInterrupt(hallSensor1), dirhallSensor1, RISING);
    attachInterrupt(digitalPinToInterrupt(hallSensor2), dirhallSensor2, RISING);
    attachInterrupt(digitalPinToInterrupt(hallSensor3), dirhallSensor3, RISING);
    attachInterrupt(digitalPinToInterrupt(hallSensor4), dirhallSensor4, RISING);
    attachInterrupt(digitalPinToInterrupt(hallSensor5), dirhallSensor5, RISING);
    attachInterrupt(digitalPinToInterrupt(hallSensor6), dirhallSensor6, RISING);
    attachInterrupt(digitalPinToInterrupt(inputFG), inputSpeed, FALLING);

    Close();
}

void loop() {
    // put your main code here, to run repeatedly:
    if(!control_motor.forward && control_led.status_led && !control_motor.status_stop  && control_motor.count_pul_FG <= 15){
        control_led.status_led = false;
        tickerSetPwmLedLightOn.stop();
        tickerSetPwmLedLightOff.start();
    }
    checkButtonConfigClick();
    setSpeedControl();
    tickerupdate();
    if(!control_motor.normal_mode){
        server.handleClient();
    }
}
