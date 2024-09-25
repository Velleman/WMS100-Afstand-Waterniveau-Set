// Afstandsmeter V1.0.0

#include <LiquidCrystal.h>

#define MY_PI 3.1415926535897932384626433832795
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// Initialiseer het LCD-scherm met de juiste pinconfiguratie
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void afstandsmeting();
void waterniveaumeting();
void btnLEFT_RIGHT_is_ingedrukt();
void btnUP_is_ingedrukt();
void btnDOWN_is_ingedrukt();
void refresh_LCD();

int lcd_key     = 0;
int adc_key_in  = 0;
int select = 0;
int diepte = 100;
int breedte = 100;
int btn_up_ingedrukt = false;
int btn_down_ingedrukt = false;
int btn_select_ingedrukt = false;
const float my_pi = MY_PI; // Pi waarde
unsigned long buttonPressStartTime = 0;
int accelerationInterval = 1000; // Tijd in milliseconden voordat de versnelling begint
int accelerationStep = 2; // Stapgrootte voor versnelde veranderingen

// Definieer de pinnen voor de ultrasone sensor
const int trigPin = 3; // Triggernaald van de ultrasone sensor
const int echoPin = 2; // Echonaald van de ultrasone sensor

// Variabele om de vorige tijd van metingen bij te houden
unsigned long vorigeMillis = 0;

// Interval in milliseconden tussen opeenvolgende metingen
const long interval = 200; // Pas dit aan naar wens voor de meetfrequentie

// Maximale toegestane afwijking (in microseconden) voor een enkele meting
const long maxToegestaneAfwijking = 1000;

// Drempelwaarde voor opeenvolgende metingen met een grotere afwijking
const int opeenvolgendeMetingenDrempelwaarde = 10;

// Vorige metingsduur
long vorigeDuur = 0;

// Aantal opeenvolgende metingen met een grotere afwijking
int opeenvolgendeMetingen = 0;

// Knoppen inlezen
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // adc inlezen 
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   

 return btnNONE; 
}

void setup() {
  // Start de LiquidCrystal-bibliotheek voor het LCD-scherm
  lcd.begin(16, 2); 
  refresh_LCD(0, diepte, breedte);
}

void loop() {

  // Inlezen drukknoppen
  lcd_key = read_LCD_buttons();

  switch (lcd_key) {
   case btnSELECT:
   {
    if (!btn_select_ingedrukt) { // Als de knop nog niet is ingedrukt
      if(select < 4){
        select++;
      }
      else{
        select = 1;
      }
      refresh_LCD(select, diepte, breedte);
      btn_select_ingedrukt = true;
    }
    break;
   }
   case btnUP:
   {
    if (!btn_up_ingedrukt) { // Als de knop nog niet is ingedrukt
      btnUP_is_ingedrukt(select, diepte, breedte);
      btn_up_ingedrukt = true;
    }
    break;
   }
   case btnDOWN:
   {
    if (!btn_down_ingedrukt) { // Als de knop nog niet is ingedrukt  
      btnDOWN_is_ingedrukt(select, diepte, breedte);
      btn_down_ingedrukt = true;
    }
    break;
   }
   default:
     // Als geen knop is ingedrukt, zet de vlag op false
    btn_up_ingedrukt = false;
    btn_down_ingedrukt = false;
    btn_select_ingedrukt = false;
    break;
  }

  //Modus instellen
  switch (select) {
   case 1:
   {
    afstandsmeting();
    break;
   }
   case 2:
   {
    waterniveaumeting();
    break;
   }
   default:
    break;
  }

  delay(5);
}

// Functie om microseconden naar centimeters om te rekenen
long microsecondenNaarCentimeters(long microseconden) {
  return microseconden / 29 / 2;
}

void afstandsmeting(){
  
  // Huidige tijd vastleggen
  unsigned long huidigeMillis = millis();

  // Controleer of het tijd is om een nieuwe meting uit te voeren
  if (huidigeMillis - vorigeMillis >= interval) {
    vorigeMillis = huidigeMillis;

    long duur, cm;

    // Activeer de ultrasone sensor om een meting uit te voeren
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    pinMode(echoPin, INPUT);
    duur = pulseIn(echoPin, HIGH);

    // Controleer of de afwijking binnen het toegestane bereik ligt voor een enkele meting
    if (abs(duur - vorigeDuur) <= maxToegestaneAfwijking) {
      // Als de afwijking binnen het toegestane bereik ligt, accepteer de meting.
      vorigeDuur = duur;
      opeenvolgendeMetingen = 0;

      cm = microsecondenNaarCentimeters(duur);

      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(cm);
      lcd.print(" cm");
    }
    else {
      // Als de afwijking buiten het toegestane bereik ligt, verhoog de teller voor opeenvolgende metingen.
      opeenvolgendeMetingen++;

      // Als het aantal opeenvolgende metingen met een grotere afwijking de drempel bereikt, accepteer de meting.
      if (opeenvolgendeMetingen >= opeenvolgendeMetingenDrempelwaarde) {
        vorigeDuur = duur;
        opeenvolgendeMetingen = 0;

        cm = microsecondenNaarCentimeters(duur);

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(cm);
        lcd.print(" cm");
      }
    }
  }
}

void waterniveaumeting(){

    // Huidige tijd vastleggen
  unsigned long huidigeMillis = millis();

  // Controleer of het tijd is om een nieuwe meting uit te voeren
  if (huidigeMillis - vorigeMillis >= interval) {
    vorigeMillis = huidigeMillis;

    long duur, cm;

    // Activeer de ultrasone sensor om een meting uit te voeren
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    pinMode(echoPin, INPUT);
    duur = pulseIn(echoPin, HIGH);

    // Controleer of de afwijking binnen het toegestane bereik ligt voor een enkele meting
    if (abs(duur - vorigeDuur) <= maxToegestaneAfwijking) {
      // Als de afwijking binnen het toegestane bereik ligt, accepteer de meting.
      vorigeDuur = duur;
      opeenvolgendeMetingen = 0;

      cm = microsecondenNaarCentimeters(duur);
      float hoogte = diepte - cm;
      float straal = breedte / 2;
      float inhoud = my_pi * straal * straal * hoogte;
      float liters = inhoud / 1000.0;

      // Controleer of liters negatief is en corrigeer het indien nodig
      if (liters < 0.0) {
        liters = 0.0; // Zet liters op 0 als het negatief is
      }

      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print(liters);
      lcd.print(" l");
    }
    else {
      // Als de afwijking buiten het toegestane bereik ligt, verhoog de teller voor opeenvolgende metingen.
      opeenvolgendeMetingen++;

      // Als het aantal opeenvolgende metingen met een grotere afwijking de drempel bereikt, accepteer de meting.
      if (opeenvolgendeMetingen >= opeenvolgendeMetingenDrempelwaarde) {
        vorigeDuur = duur;
        opeenvolgendeMetingen = 0;

        cm = microsecondenNaarCentimeters(duur);
        float hoogte = diepte - cm;
        float straal = breedte / 2;
        float inhoud = my_pi * straal * straal * hoogte;
        float liters = inhoud / 1000.0;

        // Controleer of liters negatief is en corrigeer het indien nodig
        if (liters < 0.0) {
          liters = 0.0; // Zet liters op 0 als het negatief is
        }

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(liters);
        lcd.print(" l");
      }
    }
  }
}

void btnUP_is_ingedrukt(int select, int &diepte, int &breedte){
  if (select == 3){
    diepte++;
    lcd.setCursor(0, 1);      
    lcd.print("   "); // Wis drie tekens (voor het geval het vorige getal meer dan één cijfer had)
    lcd.setCursor(0, 1);
    lcd.print(max(diepte, 0));
  }
  else if (select == 4){ //Breedte
      breedte++;
      lcd.setCursor(0, 1);      
      lcd.print("   "); // Wis drie tekens (voor het geval het vorige getal meer dan één cijfer had)
      lcd.setCursor(0, 1);
      lcd.print(max(breedte, 0));
  }
}

void btnDOWN_is_ingedrukt(int select, int &diepte, int &breedte){
  if (select == 3){
    diepte--;
    lcd.setCursor(0, 1);      
    lcd.print("   "); // Wis drie tekens (voor het geval het vorige getal meer dan één cijfer had)
    lcd.setCursor(0, 1);
    lcd.print(max(diepte, 0));
  }
  else if (select == 4){ //Breedte
      breedte--;
      lcd.setCursor(0, 1);      
      lcd.print("   "); // Wis drie tekens (voor het geval het vorige getal meer dan één cijfer had)
      lcd.setCursor(0, 1);
      lcd.print(max(breedte, 0));
  }
}

void refresh_LCD(int select, int diepte, int breedte){
  lcd.clear();
  switch(select){
    case 0:
    {
      lcd.setCursor(0, 0);
      lcd.print("Afstandsmeter"); 
      lcd.setCursor(0, 1);
      lcd.print("Druk op select"); 
      break;
    }
    case 1:
    {
      lcd.setCursor(0, 0);
      lcd.print("Afstand:");  
      break;
    }
    case 2:
    {
      lcd.setCursor(0, 0);
      lcd.print("Waterniveau:");
      break;
    }
    case 3:
    {
      lcd.setCursor(0, 0);
      lcd.print("Hoogte in cm:");
      lcd.setCursor(0, 1);
      lcd.print(max(diepte, 0));
      break;
    }
    case 4:
    {
      lcd.setCursor(0, 0);
      lcd.print("Breedte in cm:"); 
      lcd.setCursor(0, 1);
      lcd.print(max(breedte, 0));  
      break;
    }
    default:
      break;
  }
}