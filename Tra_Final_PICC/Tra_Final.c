#include <16f887.h>
#device ADC = 10
#use delay(clock=32000, crystal)
#fuses XT, NOWDT, NOPROTECT
#define use_portb_lcd TRUE
#include <LCD.c>

#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
#use fast_io(D)

int1 start_stop = 0;
int8 cont_motor = 0;
const byte motor[8] = {0b1000, 0b1100, 0b0100, 0b0110, 0b0010, 0b0011, 0b0001, 0b1001};

void main () {
   //Variables
   //Frecuencia respiratoria (10-70) Respiraciones por minuto
   float frec_res = 0;
   int16 q_frec_res = 0;
   //Presion medida
   float pres_med = 0;
   int16 q_pres_med = 0;
   int16 bits100 = 838;        //El sensor MPX4115 entrega 4.092V a 100kPa
   
   //Puertos Motor Paso a Paso
   set_tris_d(0xF0);             //Se usan los puertos de RB0 hasta RB3
   
   //Switches de Start y Stop
   set_tris_c(0xF6);             //Se usan los puertos RC0 y RC3
   
   //Configuración ADC
   setup_adc_ports(0x03);        //Se usan los canales AN0 y AN1
   setup_adc(ADC_CLOCK_INTERNAL);
   
   //Configuración PWM
   setup_timer_2(T2_DIV_BY_1, 79, 4);
   setup_ccp1(CCP_PWM);
   
   //Configuración LCD
   lcd_init();
   delay_ms(300);
   printf(lcd_putc, "\fBienvenido!!!");
   delay_ms(300);
   
   while (true) {
      //Lectura de la frecuencia respiratoria
      set_adc_channel(1);
      delay_us(20);
      q_frec_res = read_adc();
      frec_res = 0.06842*q_frec_res;
      
      //Lectura de la presión
      set_adc_channel(0);
      delay_us(20);
      q_pres_med = read_adc();
      pres_med = (100*q_pres_med)/bits100;
      
      //Visualización de los datos
      printf(lcd_putc, "\fFr.R = %.1f Res/Min", frec_res);
      printf(lcd_putc, "\nPr. = %.1f kPa", pres_med);
      delay_ms(10);
      
      //Verificar si se presiona el pulsador de Start
      if (input(PIN_C0)) {
         start_stop = 1;
         delay_ms(100);
      } else if (input(PIN_C3)) {
         start_stop = 0;
         delay_ms(100);
      }
      
      //Movimiento del motor
      if (start_stop) {
         set_pwm1_duty();
      } else {
         set_pwm1_duty(0);
      }
   }
}
