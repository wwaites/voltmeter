/*
 * Copyleft 2012 William Waites <wwaites@tardis.ed.ac.uk>
 * This program is Free Software. You may use it according
 * to the terms of the GNU GPL Version 3 or at your option
 * any later version.
 *
 * Very simple voltage divider circuit connected to the
 * analogue input pin:
 * 
 *       Vin --- R1 ---+--- R2 --- Gnd
 *                     |
 *                     \-- A0
 *
 * This program reads the input and stores some recent
 * readings in a circular buffer. It then makes the contents
 * of the buffer available as JSON data via HTTP.
 *
 */

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"

/* Our circuit is on analogue pin 0 */
#define VOLTAGE_PIN 0
/*
 *  This scales the input voltage according to R1 and R2
 *  and the value read at A0 is between 0 and 5V in 1024
 *  steps.
 */
#define R1 683000.0
#define R2 219000.0
float scaling = 5.0 * (R1 + R2) / R2 / 1024;

/*
 * Circular buffer to hold the readings, also keep track
 * of the generation of the readings, i.e. the overflow
 * of the circular buffer
 */
#define NREADINGS 240
unsigned int voltage[NREADINGS];
unsigned int vptr = 0;
unsigned int gen = 0;

/* Configuration for the web server */
static unsigned char mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01 };
static unsigned char ip[] = { 192, 168, 1, 16 };
static unsigned char nameserver[] = { 8, 8, 8, 8 };
static unsigned char gateway[] = { 192, 168, 1, 1 };

WebServer webserver("", 80);

/* 
 * Interrupt Service Routine -- take a voltage reading and
 * store it in the circular buffer
 */
ISR(TIMER1_COMPA_vect) {
    int reading = analogRead(VOLTAGE_PIN);
    float v = reading * scaling;

    voltage[vptr++] = (int)v * 100 + (int)(v * 100) % 100;
    if (vptr == NREADINGS) {
	vptr = 0;
	gen += 1;
    }
}

/*
 * Web service routine. Return a JSON list of 2-tuples with
 * the sequence number of the reading and the reading itself
 */
void getVoltage(WebServer &server, WebServer::ConnectionType type, char *, bool) {
    server.httpSuccess("application/json");
    if (type != WebServer::HEAD) {
	char buf[16];
	memset(buf, 0, sizeof(buf));
	server.print("[\n");

	/* a counter, and save values to vptr
	   in case the interrupt service routine
	   changes them out from underneath us */
	unsigned int i, vptrx = vptr, genx = gen;

	/* if we are not the first iteration through
	   the list, start at the current pointer location
	   and wrap around. otherwise start at the 
	   beginning and go up to vptr */
	if (genx > 0) {
	    i = vptrx;
	} else {
	    i = 0;
	}

	for (;;) {
	    unsigned int thisgen;
	    if (i < vptrx) {
		thisgen = genx;
	    } else {
		thisgen = genx-1;
	    }

	    memset(buf, 0, sizeof(buf));
	    snprintf(buf, sizeof(buf), "\t[%u, ", (unsigned long)thisgen * NREADINGS + i);
	    server.print(buf);

	    memset(buf, 0, sizeof(buf));
	    snprintf(buf, sizeof(buf), "%d.%02d]", voltage[i] / 100, voltage[i] % 100);
	    server.print(buf);

	    i++;
	    if ( (genx > 0) && (i == NREADINGS) ) {
		i = 0;
	    } 

	    if (i == vptrx) {
		server.print("\n");
		break;
	    } else {
		server.print(",\n");
	    }

	}
	server.print("]\n");
    }
}

void setup() {
    // initialise memory to zero...
    memset(voltage, 0, sizeof(voltage));

    // DISABLE INTERRUPTS
    cli();

    // clear timer 1 configuration register
    TCCR1A = 0;
    TCCR1B = 0;

    OCR1A = 15624; // every second
    // turn on CTC mode
    TCCR1B |= (1 <<WGM12);  // CTC mode
    // set CS10 and CS12 bits for 1024x prescaler
    TCCR1B |= (1 <<CS10);
    TCCR1B |= (1 <<CS12);
    // enable timer compare interrupt
    TIMSK1 |= (1 <<OCIE1A);

    // ENABLE INTERRUPTS
    sei();
   
    Ethernet.begin(mac, ip, nameserver, gateway);
    webserver.setDefaultCommand(&getVoltage);
    webserver.begin();
}

/* Main loop */
void loop() {
    char buf[64];
    int len = 64;
    webserver.processConnection(buf, &len);
}
