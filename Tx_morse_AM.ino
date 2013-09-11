/*
     TX Código Morse Radio AM Arduino
	
Mario Pérez Esteso
http://www.geekytheory.com
https://www.youtube.com/user/telecoreference

https://twitter.com/geekytheory

Facebook page! Join us with a LIKE!
https://www.facebook.com/geekytheory
*/

#include <LiquidCrystal.h> 
/*Incluyo la librería del display LCD 
para poder utilizar las funciones*/

/*Defino los pines del Arduino a los que voy a conectar el display LCD*/

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

long milisegundos_comienzo=0;
long milisegundos_final=0;

const long periodo_transmitir=8; 
//Cuanto más grande, más tarda en transmitir un punto o raya. 
//La frecuencia no varía demasiado, pero sí la duración de la señal.

#define DURACION_PUNTO 64

const int duracion_punto=DURACION_PUNTO;//number of periods for punto
const int pausa_punto=DURACION_PUNTO;//pause after punto
const int duracion_raya=3*DURACION_PUNTO;//number of persots for raya
const int pausa_raya=DURACION_PUNTO;//pause after raya

char *letras[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.","....", "..", ".---", "-.-", ".-..", "--",
  "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."};

char *numeros[]={"-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."};

int delay_punto = 200; //0.2 segundos

void punto(void);
void guion(void);
void transmitir(int x_ciclos);
void no_transmitir(int x_ciclos);

// ### INC ### Increment Register (reg = reg + 1)
#define ASM_INC(reg) asm volatile ("inc %0" : "=r" (reg) : "0" (reg))

void setup()
{
  Serial.begin(9600);
  DDRB=0xFF;  //Puerto B declarado como salidas
  milisegundos_comienzo=millis(); //Mandamos un punto para saber la frecuencia aproximada
                                  //a la que trabajaremos.
  punto();
  milisegundos_final=millis();
  Serial.print(milisegundos_final-milisegundos_comienzo); 
  Serial.print(" ");
  Serial.print((duracion_punto+pausa_punto)*periodo_transmitir*256/( milisegundos_final- milisegundos_comienzo)/2);//Portadora
  Serial.print("kHz ");
  Serial.println(); 
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("BIENVENIDO AL Tx");
  lcd.setCursor(0,1);
  lcd.print("  DE MORSE AM!");
  delay(5000);
  for(int q=3;q>=0;q--)
  {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Podra transmitir");
  lcd.setCursor(0,1);
  lcd.print("en");
  lcd.setCursor(4,1);
  lcd.print(q);
  lcd.setCursor(5,1);
  lcd.print("s.");
  delay(1000);
  }
}

void loop()
{
  char ch[32]; //32 Caracteres, que son los que caben en el display LCD
  int fila=0; //Variable que controla la fila del display LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("- Teclee mensaje");
  lcd.setCursor(0,1);
  lcd.print("- Pulse Intro");
  if(Serial.available())
  {
    int i=0;
    int k=0;
  lcd.clear();
  while(Serial.available())
  {
    Serial.flush();
    ch[i] = Serial.read();
    lcd.setCursor(i,fila);
    lcd.print(ch[i]);
    Serial.println(ch[i]);
    i=i+1;
    if(i==16)
    {
      fila=1;
      i=0;
    }
    if (ch[k] >= 'a' && ch[k] <= 'z')
    {
      lcd.setCursor(11,1);
      lcd.print("     ");
      lcd.setCursor(11,1);
      lcd.print(letras[ch[k] - 'a']);
      flashsecuencia(letras[ch[k] - 'a']);
    }
    else if (ch[k] >= 'A' && ch[k] <= 'Z')
    {
      lcd.setCursor(11,1);
      lcd.print("     ");
      lcd.setCursor(11,1);
      lcd.print(letras[ch[k] - 'A']);
      flashsecuencia(letras[ch[k] - 'A']);
    }
    else if (ch[k] >= '0' && ch[k] <= '9')
    {
      lcd.setCursor(11,1);
      lcd.print("     ");
      lcd.setCursor(11,1);
      lcd.print(numeros[ch[k] - '0']);
      flashsecuencia(numeros[ch[k] - '0']);
    }
    else if (ch[k] == ' ')
    {
      delay(delay_punto*7);      //Espacio entre palabras 
    }
    k++;
  }
  }
  delay(2000);
}

void punto(void)//Función punto
{
  for(int i=0;i<duracion_punto;i++)
  {
    transmitir(periodo_transmitir);
  }
  for(int i=0;i<pausa_punto;i++)
  {
    no_transmitir(periodo_transmitir);
  }
}
void raya(void)//Función raya
{
  for(int i=0;i<duracion_raya;i++)
  {
    transmitir(periodo_transmitir);
  }
  for(int i=0;i<pausa_raya;i++)
  {
    no_transmitir(periodo_transmitir);
  }
}
void transmitir(int x_ciclos)
{
  unsigned int numero_puerto;
  for (int i=0;i<x_ciclos;i++)
  {
    numero_puerto=0;
    do
    {
	PORTB=numero_puerto;
	ASM_INC(numero_puerto);
    }
    while(numero_puerto<255);
  }
}
void no_transmitir(int x_ciclos)
{
  unsigned int numero_puerto;
  PORTB=0x00; // El pin 13 lo establecemos en OFF
  for (int i=0;i<x_ciclos;i++)
  {
    numero_puerto=0;
    do
    {
	ASM_INC(numero_puerto);
	asm volatile ("NOP"); //Siglas de No Operation Performed
    }
    while(numero_puerto<255);
  }
}
 void flashsecuencia(char *secuencia)
{
   int i = 0;
   while (secuencia[i] != NULL)
   {
       punto_o_raya(secuencia[i]);
       i++;
   }
   delay(delay_punto*3); //Espacio entre letras
}

void punto_o_raya(char punto_o_raya)//Distingue entre punto o raya
{
  if (punto_o_raya == '.')
  {
    punto();           
  }
  else
  {
    raya();           
  }  
  delay(delay_punto);
}


