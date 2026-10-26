/* Integrated demonstration firmware; thresholds provisional. */
/* External NE555 clock; existing pin map retained. */
#include <REG52.H>

/* Keil C51, AT89S52, 11.0592 MHz, standard 12-clock core.
 * Build with DEFINE(LCD_ONLY) for the display-only test.
 * HW-61 mapping ASSUMED: P0 RS, P1 RW, P2 E, P3 backlight,
 * P4..P7 D4..D7. Address is detected, not hard-coded.
 */
sbit SDA = P1^2;
sbit SCL = P1^3;
sbit K1 = P3^4;
sbit K2 = P3^5;
unsigned char lcd_address;
bit bus_fault;
unsigned char code hex_digits[] = "0123456789ABCDEF";
/* Timer 0 polling: 10 us nominal, plus call/loading overhead. */
void short_wait(void)
{
    TR0=0; TF0=0; TH0=0xFF; TL0=0xF7;
    TR0=1; while(!TF0) {} TR0=0; TF0=0;
}
void delay_ms(unsigned int ms)
{
    while(ms--) {
        TR0=0; TF0=0; TH0=0xFC; TL0=0x66;
        TR0=1; while(!TF0) {} TR0=0; TF0=0;
    }
}
void clock_high(void)
{
    unsigned char timeout=255;
    SCL=1;
    while(!SCL && --timeout) {}
    if(!SCL) bus_fault=1;
    short_wait();
}
void start(void)
{
    SDA=1; short_wait(); clock_high();
    if(!SDA) bus_fault=1;
    SDA=0; short_wait(); SCL=0;
}
void stop(void)
{
    SCL=0; SDA=0; short_wait(); clock_high();
    SDA=1; short_wait();
}
bit send(unsigned char value)
{
    unsigned char i;
    bit ack;
    for(i=0;i<8;i++) {
        SDA=(value & 0x80)!=0;
        short_wait(); clock_high(); SCL=0;
        value<<=1;
    }
    SDA=1; short_wait(); clock_high();
    ack=!SDA; SCL=0;
    return ack;
}
bit probe(unsigned char address)
{
    bit ack;
    start(); ack=send(address<<1); stop();
    return ack && !bus_fault;
}
void backpack(unsigned char value)
{
    start();
    if(!send(lcd_address<<1)) bus_fault=1;
    if(!send(value)) bus_fault=1;
    stop();
}
void nibble(unsigned char value, unsigned char rs)
{
    unsigned char out=(value & 0xF0)|0x08|rs;
    backpack(out); backpack(out|0x04); backpack(out);
}
void lcd_byte(unsigned char value, unsigned char rs)
{
    nibble(value,rs); nibble(value<<4,rs);
    delay_ms(2); /* Conservative delay, RW held LOW. */
}
void text(char code *p)
{
    while(*p) lcd_byte(*p++,1);
}
void init_lcd(void)
{
    backpack(0x08); delay_ms(50);
    nibble(0x30,0); delay_ms(5);
    nibble(0x30,0); delay_ms(2);
    nibble(0x30,0); delay_ms(2);
    nibble(0x20,0); delay_ms(2);
    lcd_byte(0x28,0); /* 4-bit, 2 lines */
    lcd_byte(0x08,0); lcd_byte(0x01,0);
    lcd_byte(0x06,0); lcd_byte(0x0C,0);
}

sbit ADC_A = P3^2;
sbit ADC_START_ALE = P3^3;
sbit ADC_EOC = P3^6;
unsigned char adc_value;

/* Timer1: 10ms timeout, 9216 counts, preload DC00. */
void timeout_start(void)
{
    TR1=0; TF1=0; TH1=0xDC; TL1=0x00; TR1=1;
}
/* Hold START high until EOC LOW, then start conversion on falling edge.
 * Return 1: EOC never LOW; 2: EOC never HIGH; 0: valid conversion. */
unsigned char adc_read(unsigned char channel)
{
    ADC_START_ALE=0; ADC_A=channel & 1;
    P2=0xFF; /* Release port for ADC input. */
    short_wait(); ADC_START_ALE=1; short_wait();
    timeout_start();
    while(ADC_EOC && !TF1) {}
    TR1=0;
    if(ADC_EOC) { ADC_START_ALE=0; return 1; }
    ADC_START_ALE=0; timeout_start();
    while(!ADC_EOC && !TF1) {}
    TR1=0;
    if(!ADC_EOC) return 2;
    short_wait(); adc_value=P2;
    return 0;
}
void decimal3(unsigned int value)
{
    lcd_byte('0'+value/100,1);
    lcd_byte('0'+(value/10)%10,1);
    lcd_byte('0'+value%10,1);
}
void adc_error(unsigned char channel, unsigned char error)
{
    text("CH"); lcd_byte('0'+channel,1);
    if(error==1) text(" EOC NO LOW   ");
    else text(" EOC NO HIGH  ");
}

/* Demonstration settings, NOT calibrated gas safety limits.
 * Temperature compare in ADC counts: >=21 is41.02C; <=18 is35.16C.
 * These implement nominal ON40C/OFF36C with the coarse 5V ADC resolution.
 */
#define TEMP_ON_RAW 21
#define TEMP_OFF_RAW 18
#define GAS_ON_RAW 140
#define GAS_OFF_RAW 100
#define CONFIRM_BATCHES 3
unsigned char temp_samples[5], gas_samples[5];
unsigned char fan_count, gas_count;
bit fan_on, gas_on;

/* Sort a five-sample batch; median rejects up to two isolated extremes.
 * It cannot correct a bad reference, sustained noise or wrong sensor voltage.
 */
unsigned char median5(unsigned char *values)
{
    unsigned char i,j,v;
    for(i=1;i<5;i++) {
        v=values[i]; j=i;
        while(j>0 && values[j-1]>v) {
            values[j]=values[j-1]; j--;
        }
        values[j]=v;
    }
    return values[2];
}
void control(unsigned char t, unsigned char g)
{
    if((!fan_on && t>=TEMP_ON_RAW)||(fan_on && t<=TEMP_OFF_RAW)) {
        if(++fan_count>=CONFIRM_BATCHES) { fan_on=!fan_on; fan_count=0; }
    } else fan_count=0;
    if((!gas_on && g>=GAS_ON_RAW)||(gas_on && g<=GAS_OFF_RAW)) {
        if(++gas_count>=CONFIRM_BATCHES) { gas_on=!gas_on; gas_count=0; }
    } else gas_count=0;
    K1=!fan_on; K2=!gas_on;
}
void main(void)
{
    unsigned char a,found=0,error=0,ch=0,i,t,g;
    unsigned int temperature;
    K1=1; K2=1; EA=0;
    fan_on=0; gas_on=0; fan_count=0; gas_count=0;
    ADC_START_ALE=0; ADC_A=0; ADC_EOC=1; P2=0xFF;
    TR0=0; TR1=0; TMOD=0x11;
    SDA=1; SCL=1; delay_ms(100);
    for(a=0x20;a<=0x3F;a++) {
        if(a==0x28) a=0x38;
        bus_fault=0;
        if(probe(a)) { lcd_address=a; found++; }
    }
    /* Missing LCD: motor OFF, buzzer ON; reset after fixing interface. */
    if(found!=1) { K2=0; while(1) {} }
    bus_fault=0; init_lcd(); text("DEMO MONITOR");
    lcd_byte(0xC0,0); text("T40/36 G140/100"); delay_ms(1500);
    while(1) {
        /* Two conversions per channel: discard first after mux change.
         * Check every handshake. Acquire batch before updating LCD/outputs.
         */
        for(i=0;i<5;i++) {
            ch=0; error=adc_read(0); if(error) break;
            error=adc_read(0); if(error) break;
            temp_samples[i]=adc_value;
            ch=1; error=adc_read(1); if(error) break;
            error=adc_read(1); if(error) break;
            gas_samples[i]=adc_value;
            delay_ms(80);
        }
        if(error) {
            /* Latched fault: no stale samples or automatic restart. */
            K1=1; K2=0;
            lcd_byte(0x80,0); adc_error(ch,error);
            lcd_byte(0xC0,0); text("FAULT RESET REQD");
            while(1) {}
        }
        t=median5(temp_samples); g=median5(gas_samples);
        control(t,g);
        temperature=(unsigned int)(((unsigned long)t*500UL+128UL)/256UL);
        lcd_byte(0x80,0);
        text("Temp:"); decimal3(temperature); text("C G:"); decimal3(g); text(" ");
        lcd_byte(0xC0,0);
        text("R:"); decimal3(t); text(" F:"); lcd_byte(fan_on?'1':'0',1);
        text(" B:"); lcd_byte(gas_on?'1':'0',1); text("   ");
        if(bus_fault) {
            /* LCD failure after detection: latched fault, buzzer ON. */
            K1=1; K2=0; while(1) {}
        }
    }
}

