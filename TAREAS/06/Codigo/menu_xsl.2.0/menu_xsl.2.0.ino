#include<Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // dependiendo del fabricante del LCD el codigo 0x27 puede cambiar


#define DT  2       // DT a pin digital 2 (DT en modulo) Pin de la interrupcion 
#define CLK  4      // CLK a pin digital 4 (CLK en modulo)
#define btn 3       // Swiche del encoder al pin 3 Pin de interrupcion

//prototipado de funciones  a utlizarce
void Actualizacion_menu();                      // en este se  definen los menus y submenus
void Impresion( String *arrayMenu, int size);   //despliega los menus y el cursor
void encoder();                                 //interrucion de encoder, define el contador de jiro
void encoder_boton();                           //interrupcion del boton del encoder, define la navegacion y jerarquia de menus
void Contador();                                //

//variables volatiles manejadas por interrupciones y menus
volatile int posicion_encoder = 0;  // Posicion acumulada del encoder
volatile int Bandera_menu = 1;      // Valor anterior de la posicion del encoder
volatile int Estado_menu = 0;       //localizacion de direccionamiento dentro de menus o submenus
volatile int Estado_anterior_menu;  //localizacion desplegada dentro de menus o submenus

//Variables de funciones anidadas
int Contador_Acumulado = 0;



void setup() {
  // Declaracion de interrupciones
  //                                   pin,  funcion, tipo de señal;  
  attachInterrupt(digitalPinToInterrupt(DT), encoder, LOW);   //rotacion
  attachInterrupt(digitalPinToInterrupt(btn), encoder_boton, LOW); //click

  //definicion de pines
  pinMode(DT, INPUT);
  pinMode(CLK, INPUT);
  pinMode(btn, INPUT_PULLUP);    // Entrada con resistencia Pullup para el Switch del encoder

  //Se inicializa la pantalla LCD y se despliega el mensaje inicial
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" INICIANDO");
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print(" INICIANDO.");
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print(" INICIANDO..");
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print(" INICIANDO...");
  delay(500);

  Actualizacion_menu(); //primer despliegue de menu
}

//+++++++++++ CICLO PRINCIPAL DEL SISTEMA++++++++++++++++
void loop() {
  Actualizacion_menu();
  delay(50);
}



//      FUNCION DE IMPLEMENTACION DE  MENUS:
//       La variable estado de menu define que menu o submenu se desplegara 
//       siendo el primer digito el menu principal y los digitos de izquierda
//       a derecha reflejan la jerarquia de los submenus
//      arrayMenu[] contiene todas las opcines a desplegarse en cada menu
//      del mismo mimo modo desde esta rutina podemos llamar a las respectivas de cada opcion de menu.
void Actualizacion_menu() {
  if (Bandera_menu == 1) {
    Bandera_menu = 0;
    Serial.print(posicion_encoder);
    Serial.print("   ");
    Serial.println(Estado_menu);
    if (Estado_menu == 0) {
      String arrayMenu[] = {"Opcion1", "Opcion2", "Opcion3", "Opcion4", "Opcion5", "Opcion6"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]); //determinacion del tamaño del menu
      Impresion(arrayMenu, size); //Llamada a funcion de impresion 

    } else if (Estado_menu == 1) {
      String arrayMenu[] = {"Atras", "Opcion11"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]);
      Impresion(arrayMenu, size);
    } else if (Estado_menu == 11) {
      String arrayMenu[] = {"Atras", "Opcion111"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]);
      Impresion(arrayMenu, size);
    } else if (Estado_menu == 111) {
      String arrayMenu[] = {"Atras"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]);
      Impresion(arrayMenu, size);

    } else if (Estado_menu == 2) {
      String arrayMenu[] = {"Atras", "Opcion21"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]);
      Impresion(arrayMenu, size);
    } else if (Estado_menu == 21) {
      String arrayMenu[] = {"Atras"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]);
      Impresion(arrayMenu, size);

    } else if (Estado_menu == 3) {
      String arrayMenu[] = {"Atras"};
      int size = sizeof(arrayMenu) / sizeof(arrayMenu[0]);
      Impresion(arrayMenu, size);
    } else if (Estado_menu == 6) {
      Contador();
    }
  }
}


//FUNCION QUE CAPTURA EL GIRO DEL ENCODER
void encoder()  {                //Funcion que determina el sentido del giro del encoder
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // No lee posibles rebotes en pulsos menores a 5 mseg.
    if (digitalRead(CLK) == HIGH)     // si CLK es HIGH, rotacion a la derecha
    {
      posicion_encoder-- ;        // incrementa posicion del menu en 1
    } else {                      // si CLK es LOW, rotacion a la izquierda
      posicion_encoder++ ;        // decrementa posicion del menu en 1
    }
    posicion_encoder = min(100, max(0, posicion_encoder));  // numero de opciones del Menu, inferior 0 superior en 5
    ultimaInterrupcion = tiempoInterrupcion;  //actualiza valor del tiempo tiempo
  }
  Bandera_menu = 1;  //bandera de actualizacion de menu
}

//FUNCION QUE CAPTURA EL PULSO DEL ENCODER Y CONTRTOLA LA SINCRONIZACION DE MENUS 
//la variable Estado_Anterior_Menu despliega el estado actual desplegado en el pantalla
// y apartir del cual se determinaran los sigueintes menus
//Estado_menu representa el nuevo estado a desplegar, es decir, a que menu entraremos.
//en el Estado_menu=0 nos direccionaremos al menu principal 
void encoder_boton() {
  static unsigned long ultimaInterrupcion_B = 0;  // variable static con ultimo valor de
  unsigned long tiempoInterrupcion_B = millis();  // variable almacena valor de func. millis
  if (tiempoInterrupcion_B - ultimaInterrupcion_B > 5) {
  // Serial.println(" INTERRUPCION ENCODER");
    switch (Estado_anterior_menu) {
      case 0:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 1;
            break;
          case 1:
            Estado_menu = 2;
            break;
          case 2:
            Estado_menu = 3;
            break;
          case 5:
            Estado_menu = 6;
            break;
        }
        break;

      case 1:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 0;
            break;
          case 1:
            Estado_menu = 11;
            break;
        }
        break;
      case 11:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 1;
            break;
          case 1:
            Estado_menu = 111;
            break;
        }
        break;
      case 111:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 11;
            break;
        }
        break;

      case 2:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 0;
            break;
          case 1:
            Estado_menu = 21;
            break;
        }
        break;
      case 21:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 2;
            break;
        }
        break;
      case 3:
        switch (posicion_encoder) {
          case 0:
            Estado_menu = 0;
            break;
        }
        break;
      case 6:
        switch (posicion_encoder) {
          default:
            Estado_menu = 0;
            break;
        }
        break;
    }
    Estado_anterior_menu = Estado_menu;
    posicion_encoder = 0;
    Bandera_menu = 1;
  /*  Serial.print("  Datos int:btn   ");
    Serial.print(Estado_menu);
    Serial.print("   ");
    Serial.println(Bandera_menu);*/
    ultimaInterrupcion_B = tiempoInterrupcion_B;  //actualiza valor del tiempo tiempo
  }
}



//FUNCION QUE IMPRIME EL MENU CORRESPONDIENTE
void Impresion( String *arrayMenu, int size) {
  lcd.clear();
  lcd.setCursor(0, 0);
  posicion_encoder = min(size, max(0, posicion_encoder));
  if (posicion_encoder <= 3) {      //Despliega las primeras  opciones del menu
    for (int x = 0; x < size && x <= 3 ; x++) //
    {
      lcd.setCursor(2, x);
      lcd.print(arrayMenu[x]);
    }
    lcd.setCursor(0, posicion_encoder); //ubica el cursor en la posicion correspondiente al encoder
    lcd.print(">");     //imprime el cursor
  } else {
    int menu_Extra = posicion_encoder - 3;     // si la opcion buscada es superior a la 5 despliega las correspondientes
    for (int x = menu_Extra; x < size && x <= (3 + menu_Extra) ; x++) //
    {
      lcd.setCursor(2, x - menu_Extra);
      lcd.print(arrayMenu[x]);
    }
    lcd.setCursor(0, 3);
    lcd.print(">");
  }
}



//FUNCION DE CONTADOR
void Contador() {
  posicion_encoder = Contador_Acumulado;
  int posicion_encoder_anterior = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pulsa pasa salir...");
      lcd.setCursor(5, 1);
      lcd.print("Contador:");
  while (Estado_menu == 6) {
      Contador_Acumulado = posicion_encoder;
      lcd.setCursor(8, 2);
      lcd.print("       ");
      lcd.setCursor(8, 2);
      lcd.print(Contador_Acumulado);
      delay(50);
  }
}
