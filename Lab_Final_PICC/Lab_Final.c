#include <16f887.h>
#device ADC = 10
#use delay(clock=4000000)
#fuses XT, NOWDT, NOPROTECT
#include <LCD.c>

#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
#use fast_io(D)

void main() {
   //Variables
   //Temperatura de control (0-100 °C)
   float tempc = 0;
   int16 q_tempc = 0;
   //Temperatura medida
   float tempm = 0;
   int16 q_tempm = 0;
   int16 bits25 = (0.25*1024)/5;   //El sensor LM35 entrega 0.25V a 25°C
   //Nivel de luminosidad
   int16 max_q_ni = 1023;
   int16 q_ni = 0;
   
   //Puerto Elemento Calefactor
   set_tris_b(0xFE);
   
   //Puertos PWM
   set_tris_c(0xF9);
   
   //Configuración ADC
   setup_adc_ports(0x07);           //Utiliza los canales AN0, AN1 y AN2
   setup_adc(ADC_CLOCK_INTERNAL);
   
   //Configuración PWM
   setup_timer_2(T2_DIV_BY_1, 249, 1);
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   
   //Inicio LCD
   lcd_init();
   delay_ms(300);
   printf(lcd_putc, "\fBienvenido!!!");
   delay_ms(300);
   
   while (true) {
      //Lectura de la temperatura de control
      set_adc_channel(0);
      delay_us(20);
      q_tempc = read_adc();
      tempc = 0.097752*q_tempc;
      
      //Lectura de la temperatura medida
      set_adc_channel(1);
      delay_us(20);
      q_tempm = read_adc();
      tempm = (25*q_tempm)/bits25;
      
      //Visualización de las dos temperaturas
      printf(lcd_putc, "\fT.C = %.0f oC", tempc);
      printf(lcd_putc, "\nT. = %03.2f oC", tempm);
      delay_ms(100);
      
      //Control de temperatura
      //Condiciones
      if ((tempm >= (tempc-1)) && (tempm <= (tempc+1))) {
         set_pwm2_duty(0);
         output_low(PIN_B0);
      } else if (tempm < (tempc-1)) {
         set_pwm2_duty(0);
         output_high(PIN_B0);
      } else if ((tempm > (tempc+1)) && (tempm < (tempc+3))) {
         set_pwm2_duty(512);
         output_low(PIN_B0);
      } else if ((tempm >= (tempc+3)) && (tempm >= (tempc+5))) {
         set_pwm2_duty(818);
         output_low(PIN_B0);
      } else if (tempm > (tempc+5)) {
         set_pwm2_duty(1023);
         output_low(PIN_B0);
      } else {}
      
      //Lectura del nivel de luminosidad
      set_adc_channel(2);
      delay_us(20);
      q_ni = read_adc();
      
      //Control del nivel de luminosidad
      //Condiciones
      if (q_ni < (max_q_ni*0.2)) {
         set_pwm1_duty(1023);
      } else if ((q_ni > (max_q_ni*0.2)) && (q_ni < (max_q_ni*0.3))) {
         set_pwm1_duty(818);
      } else if ((q_ni > (max_q_ni*0.3)) && (q_ni < (max_q_ni*0.4))) {
         set_pwm1_duty(613);
      } else if ((q_ni > (max_q_ni*0.4)) && (q_ni < (max_q_ni*0.6))) {
         set_pwm1_duty(512);
      } else if ((q_ni > (max_q_ni*0.6)) && (q_ni < (max_q_ni*0.8))) {
         set_pwm1_duty(306);
      } else if (q_ni > (max_q_ni*0.8)) {
         set_pwm1_duty(0);
      } else {}
   }
   
}
