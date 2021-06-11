#include <TinyGPS.h>       //Libreria Moudlo GPS
#include <SoftwareSerial.h> //Libreria añade puertos de comunicacion serial
#include <EEPROM.h>  //Libreria permite manipular memoria EEPROM
SoftwareSerial SIM900(7, 8); //Decalara los pines para el nuevo puerto serial que ocupa el modulo GSM
TinyGPS gps;            //crea una instancia del objeto TinyGPS llamada gps
  bool newData = false;  //Variable auxiliar para saber si se recibio informacion en el modulo GPS  
  const int GPS = 2;      //Declaracion de puertos de entrada y salida
  const int Releon = 3;
  const int Releoff = 4;
  const int AgregaAdmin = 5;
  const int AgregaNum = 6;
  const int Salida = 12;
  int estado1 = HIGH;       //Inicializacion de variables de estado de pines     
  int estado2 = HIGH;
  int estado3 = HIGH;
  int estado4 = HIGH;
  int estado5 = HIGH;

void setup()
{
  
  Serial.begin(9600); //Inicializacion puerto serial modulo GPS
  SIM900.begin(9600); //Inicializacion puerto serial modulo GSM
  pinMode(GPS,INPUT); //Asigna el tipo de pin a cada puerto 
  pinMode(Releon,INPUT);
  pinMode(Releoff,INPUT);
  pinMode(AgregaAdmin,INPUT);
  pinMode(AgregaNum,INPUT);
  pinMode(Salida,OUTPUT);
  digitalWrite(Salida,LOW); // el pin de salida se inicializa con un valor bajo
}

void loop()
{
  LeeMensaje(); //Lectura de funcion todo el tiempo para detectar si ha llegado un mensaje  
}



void LeeMensaje(){
    estado1 = digitalRead(GPS);  //Lectura de los valores de cada una de las entradas
    estado2 = digitalRead(Releon);
    estado3 = digitalRead(Releoff);
    estado4 = digitalRead(AgregaAdmin);
    estado5 = digitalRead(AgregaNum); 
    
    
    if(estado4 == LOW){          //si se lee la funcion de Agragegr administrador 
      if (EEPROM.read(0)== '-'){ //y la memoria EEPROM esta vacia entonces invoca a la funcion administrador     
        Admin();
      }
    }
    else{ 
      while(Autentica()){ //Llama a la funcion Autentica para verificar que el numero entrante esta registrado
        estado1 = digitalRead(GPS);//Lectura de pines de entrada;
        estado2 = digitalRead(Releon);
        estado3 = digitalRead(Releoff);
        estado5 = digitalRead(AgregaNum); 
        if (estado1 == LOW){ //Llamado a diferentes funciones del programa
          Localiza();
        }
        else if(estado2 == LOW){
          Enciende();
        }
        else if(estado3 == LOW){
          Apaga();
        }
       
        else if(estado5 == LOW){
          NumNuevo();
        }
        
      }
    }
   

}

void Admin(){  //Agrega el primer numero(Administrador)
  String val="OK"; //Variable auxiliar
  String Admin = "5530707218"; //Numero Administrador
  int i=0; //Variable contador
  delay(400);
  if(val.equals("OK")){
    for ( i = 0; i < 10; i++)
    {
    EEPROM.write(i, Admin[i]);//Escribe el numero de administrador desde el inicio de la memoria EEPROM
    }
  }
  SIM900.println("AT+CMGF=1\r"); 
  SIM900.println("Administardor Agregado");//Envio de mensaje 
  SIM900.println((char)26); //Termino de comando
}  


bool Autentica(){ //Verifica si el numero entrante esta registrado 
  int i; //variable contador 
  String s = ""; //Almacena numero telefonicos de memoria EEPROM
  int m = 0; //Indice de memoria EEPROM 
  char data ; //almacena cada caracter de la memoria EEPROM para despues concatenarlo
  bool Comparar = false; //Variable que muestra si el Numero telefonico esta en la memoria
  do{
    s = ""; //Inicializacion vacia 
     for ( i = 0; i < 10; i++)
     {
        data = EEPROM.read(m); //Lectura de 10 en 10 digitos de la memoria EEPROM 
        s= s+data;  //Almacena los 10 digitos en la cadena s;
        m++;
     }
     String Cadena1 = "5530144955"; //Numero a comparar 1
     Comparar = s.equals(Cadena1); //Comparacion de s con Cadena1 
     if(Comparar){
      return true;//Si es la misma cadena regresa valor verdadero
     }  
     String Cadena2 = "5530707218"; //Numero a comparar 2
     Comparar = s.equals(Cadena2); //Comparacion de s con Cadena1
     if(Comparar){
      return true;//Si es la misma cadena regresa valor verdadero
     }  
   }while(data != '-');//El proceso de comparacion termina cuando se lee toda la memoria EEPROM o se encuetra un caracter vacio '-'
   return false; //En caso de no estar registrado regresa un valor falso
}

void NumNuevo(){ // Funcion para agregar nuevo numero 
  int i; //Variable contador
  int j; //variable contador 
  int memo; //variable auxiliar 
   SIM900.println("AT+CMGF=1\r"); 
   SIM900.println("Numero Agregado");
   SIM900.println((char)26);
  String Nuevo = "5530144955";//Numero a agregar
    for(i=0; i<1024; i++){
      memo = i;
      if(EEPROM.read(i) == '-'){
        for ( j = 0; j < 10; j++)
        {
          EEPROM.write(memo, Nuevo[j]);//Agrega el nuevo nuemro despues del ultimo nuemero registrado  
          memo++;
        }
      delay(100);
      break;
      }
    }
}




void Localiza(){ //Funcion para localizar y enviar coordenadas a el tenefono entrante 
    for (unsigned long start = millis(); millis() - start < 1000;) //Tiempo de lectura GPS
  {
    while (Serial.available()) //Lectura de datos provenientes del mmodulo GPS
    {
      char c = Serial.read();
      if (gps.encode(c)) 
        newData = true;  //Verifica si ha llegado nueva informacion 
    }
  }
      if (newData)      //Si ha llegado nueva informacion
      {
        float flat, flon;    //latitud y longitud
        unsigned long age;  
        String lat;
        String lon;
        gps.f_get_position(&flat, &flon, &age); //Obtiene datos mas importantes   
        SIM900.print("AT+CMGF=1\r"); //Modo SMS
        delay(400);
        SIM900.println("AT + CMGS = \"+525617364186\""); //Numero de telefono enviado
        delay(300);
        SIM900.print("Latitud = ");    //Impresion de longitud y latitud 
        SIM900.print( flat, 6);
        SIM900.print(" Longitud = ");
        SIM900.println( flon, 6);
        SIM900.print("https://www.google.com/maps/search/?api=1&query="); //URL para acceder a la pagina de google maps con las coordenadas recibidas
    SIM900.println( flon, 6);
        SIM900.print(",");
    SIM900.print( flat, 6);
        
        delay(200);
        SIM900.println((char)26); // Termino de comando
        delay(200);
        SIM900.println();
   
      }
}



void Apaga(){//Funcion para desactivar bomba gasolina
   digitalWrite(Salida,HIGH);
   SIM900.println("AT+CMGF=1\r");  
   SIM900.println("Auto apgado");
   SIM900.println((char)26);
   delay(200);
}

void Enciende(){ //Funcion para activar bomba gasolina 
   digitalWrite(Salida,LOW);
   SIM900.println("AT+CMGF=1\r"); 
   SIM900.println("Auto listo para encender");
   SIM900.println((char)26);
   delay(200);
}
