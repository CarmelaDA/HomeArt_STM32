/*
 * RH.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include <RH.h>

int rh_value(char i, char j){

	int a, b;

	if(i == '0') a = 0;
	else if(i == '1') a = 10;
	else if(i == '2') a = 20;
	else if(i == '3') a = 30;
	else if(i == '4') a = 40;
	else if(i == '5') a = 50;
	else if(i == '6') a = 60;
	else if(i == '7') a = 70;
	else if(i == '8') a = 80;
	else if(i == '9') a = 90;


	if(j == '0') b = 0;
	else if(j == '1') b = 1;
	else if(j == '2') b = 2;
	else if(j == '3') b = 3;
	else if(j == '4') b = 4;
	else if(j == '5') b = 5;
	else if(j == '6') b = 6;
	else if(j == '7') b = 7;
	else if(j == '8') b = 8;
	else if(j == '9') b = 9;

	return a+b;
}








