#define _XTAL_FREQ 500000
#include <xc.h>
#include <math.h>
#define RS PORTCbits.RC4
#define EN PORTCbits.RC5
#define trisin TRISBbits.TRISB1
#define pulsein PORTBbits.RB1
unsigned int count_pulsein=0,i=0;
char nghin=0,tram=0,chuc=0,donvi=0,tmp=0;
unsigned int pwm=0xc8;
unsigned int voltage=0,current=0,result=0;;
//Start LCD
void Cmd(char c){
    RS=0;
    PORTD=c;
    EN=1;
    __delay_us(4);
    EN=0;
}
void LCD_init(void){
    TRISD=0;
    PORTD=0x00;
    TRISC=0;
    PORTC=0;
    __delay_ms(100);
    Cmd(0x30);
    __delay_ms(5);
    Cmd(0x30);
    __delay_us(100);
    Cmd(0x30);
    __delay_us(100);
    Cmd(0x38);
    __delay_us(55);
    Cmd(0x08);
    __delay_us(55);
    Cmd(0x01);
    __delay_ms(3);
    Cmd(0x06);
    __delay_us(53);
    Cmd(0x0c);
    __delay_us(55);
}
void putc(char c){
    RS=1;
    PORTD=c;
    EN=1;
    __delay_us(5);
    EN=0;
}
void putstring(char* c){
    for(int i=0;c[i]!='\0';++i) putc(c[i]);
}
//End LCD
void push_speed(unsigned int t){
    nghin=t/1000+48;
    tmp=t%1000;
    tram=tmp/100+48;
    tmp%=100;
    chuc=tmp/10+48;
    donvi=tmp%10+48;
    Cmd(0xcc);
    //putstring("Toc do: ");
    putc(nghin);
    putc(tram);
    putc(chuc);
    putc(donvi);
}
void push_voltage(){
    voltage=(unsigned int)sqrt(voltage/10)*10;
    nghin=voltage/1000+48;
    tmp=voltage%1000;
    voltage=0;
    tram=tmp/100+48;
    tmp%=100;
    chuc=tmp/10+48;
    donvi=tmp%10+48;
    Cmd(0xa0);
    //putstring("Toc do: ");
    putc(nghin);
    putc(tram);
    putc(chuc);
    putc(donvi);
}
void push_current(){
    current=(unsigned int)sqrt(current/10)*10;
    nghin=current/1000+48;
    tmp=current%1000;
    current=0;
    tram=tmp/100+48;
    tmp%=100;
    chuc=tmp/10+48;
    donvi=tmp%10+48;
    Cmd(0xe0);
    //putstring("Toc do: ");
    putc(nghin);
    putc(tram);
    putc(chuc);
    putc(donvi);
}
//PWM
void PWM_init(){
    T2CKPS0=0;
    T2CKPS1=0;
    PR2=124;
    CCP1X=(pwm>>1)&1;
    CCP1Y=pwm&1;
    CCPR1L=pwm>>2;
    CCP2X=(pwm>>1)&1;
    CCP2Y=pwm&1;
    CCPR2L=pwm>>2;
    TOUTPS0=TOUTPS1=1;
    TOUTPS2=TOUTPS3=0;
    TMR2ON=1;
    CCP1M3=0;
    CCP1M2=0;
    CCP2M3=1;
    CCP2M2=1;
    PEIE=1;
    INTE=1;    
    TMR2IE=1;
    GIE=1;    
}
//PWM
//ADC
void ADC_init(){
    ADFM=1;
    PCFG3=0;
    PCFG2=0;
    PCFG1=1;
    PCFG0=0;    
    ADCS2=0;
    ADCS1=0;
    ADCS0=0;
    ADON=1;
    ADIF=0;
    ADIE=1;    
    __delay_ms(4);
    GO_DONE=0;    
}
void channel_select(char c){
    if(c){
        CHS2=0;//channel 1
        CHS1=0;
        CHS0=0;
    }
    else{
        CHS2=0;//channel 1
        CHS1=0;
        CHS0=1;
    }
}
//ADC
void LoadOut(void){
    Cmd(0x80);
    putstring("Motor Stopped");
    Cmd(0xc0);
    putstring("Toc do: ");
    Cmd(0x94);
    putstring("Dien ap: ");
    Cmd(0xd4);
    putstring("Dien the: ");
}
void __interrupt() LCD(){
    if(INTF) {
            INTF=0;
            count_pulsein++;
    }
    if(TMR2IF){
        TMR2IF=0;        
        i++;
        if(i%10==0&&i<110){
            channel_select(1);
            GO_DONE=1;
            while(GO_DONE);
            result=ADRESL+(ADRESH<<8);
            voltage+=result*result;
            channel_select(2);
            GO_DONE=1;
            while(GO_DONE); 
            result=ADRESL+(ADRESH<<8);
            current+=result*result;
        }
        if(i==100){
            i=0;
            push_speed(count_pulsein);
            count_pulsein=0;           
            push_voltage();            
            push_current();
        }
    }
    
}
void main(){
    LCD_init();//TRISC=0;    
    LoadOut();
    PWM_init();   
    ADC_init();
    trisin=1;
    while(1){ 
        
    }
}