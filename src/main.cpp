#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define PIN A0   // input pin Neopixel is attached to
#define NUMPIXELS 20 // number of neopixels in strip

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN);
SoftwareSerial mySerial(A7, A4);

Servo SERVO_L;
Servo SERVO_R;

static int8_t Send_buf[8] = {0} ;
int L_PS = 0;
int CONT = 0;
int led_bar = 0;
bool PSW = 0;
bool PSW_A = 1;
bool sta = 1;

// MP3
#define CMD_PLAY_W_INDEX 0X03
#define CMD_SET_VOLUME 0X06
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02
#define CMD_PLAY 0X0D
#define CMD_PAUSE 0X0E
#define CMD_SINGLE_CYCLE 0X19
#define SINGLE_CYCLE_ON 0X00
#define SINGLE_CYCLE_OFF 0X01
#define CMD_PLAY_W_VOL 0X22
// SWITCH PACK
#define PEDAL_SWITCH A3
#define LED_A A1
#define LED_R A2

void setup() {
  servo_on;
  pinMode(PEDAL_SWITCH, INPUT_PULLUP);
  pinMode(LED_A, OUTPUT);
  pinMode(LED_R, OUTPUT);

  for (; led_bar <= 11; led_bar++)
  {
    pinMode(led_bar, OUTPUT);
    digitalWrite(led_bar, 0);
  }
  led_bar = 0;

  pixels.begin();
  pixels.clear();

  close();


  mySerial.begin(9600);
  delay(300);//Wait chip initialization is complete
  sendCommand(CMD_SEL_DEV, DEV_TF);
  sendCommand(CMD_PLAY_W_VOL, 0x1F05);
  servo_off();

}

////////////////////////////////////////////////////////// COMANDOS MP3
void sendCommand(int8_t command, int16_t dat)
{
  delay(20);
  Send_buf[0] = 0x7e; //starting byte
  Send_buf[1] = 0xff; //version
  Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
  Send_buf[3] = command; //
  Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
  Send_buf[5] = (int8_t)(dat >> 8);//datah
  Send_buf[6] = (int8_t)(dat); //datal
  Send_buf[7] = 0xef; //ending byte
  for (uint8_t i = 0; i < 8; i++) //
  {
    mySerial.write(Send_buf[i]) ;
  }
}

////////////////////////////////////////////////////////// APERTURA
void open()
{
  servo_on();
  SERVO_R.write(30);  // 150(close) - 30 (open)
  delay(50);
  SERVO_L.write(154); // 150 (open) - 30 (close)

  for (; PSW == 0;)
  {
    for (L_PS = 0; L_PS <= 16; L_PS++)
    {
      pixels.setPixelColor(L_PS, pixels.Color(255, 255, 255));
    }
    pixels.show();
    delay(100);

    for (L_PS = 0; L_PS <= 16; L_PS = L_PS + 2)
    {
      pixels.setPixelColor(L_PS, pixels.Color(50, 0, 255));
    }
    pixels.show();
    delay(60);

    for (L_PS = 0; L_PS <= 16; L_PS++)
    {
      pixels.setPixelColor(L_PS, pixels.Color(255, 255, 255));
    }
    pixels.show();
    delay(100);

    for (L_PS = 1; L_PS <= 16; L_PS = L_PS + 2)
    {
      pixels.setPixelColor(L_PS, pixels.Color(150, 0, 255));
    }
    pixels.show();
    delay(40);

    servo_off();
    PSW = digitalRead(PEDAL_SWITCH);

  }

  for (L_PS = 0; L_PS <= 16; L_PS++)
  {
    pixels.setPixelColor(L_PS, pixels.Color(255, 255, 255));
  }
  pixels.show();

}

////////////////////////////////////////////////////////// ACT SERVOS
void servo_on()
{
  SERVO_L.attach(12);
  SERVO_R.attach(13);
}

////////////////////////////////////////////////////////// DEACT SERVOS
void servo_off()
{
  SERVO_L.detach();
  SERVO_R.detach();
}

////////////////////////////////////////////////////////// CIERRE
void close()
{
  servo_on();
  SERVO_L.write(20); // 150 (open) - 30 (close)
  delay(80);
  SERVO_R.write(150);  // 150(close) - 30 (open)
  for (L_PS = 0; L_PS <= 16; L_PS++)
  {
    pixels.setPixelColor(L_PS, pixels.Color(0, 0, 0));
  }
  pixels.show();
  delay(300);
  servo_off();
}

void loop() {

  PSW = digitalRead(PEDAL_SWITCH);

  if (PSW == 0 && PSW_A == 1 )
  {
    sendCommand(CMD_PLAY_W_VOL, 0x1F01);
    open();
    PSW_A = 0;
  }

  else if (PSW == 1 && PSW_A == 0)
  {
    sendCommand(CMD_PLAY_W_VOL, 0x1F02);
    close();
    delay(3700);
    for (led_bar = 0; led_bar <= 11; led_bar++)
    {
      digitalWrite(led_bar, 1);
      delay(30);
    }

    digitalWrite(LED_A, 1);
    delay(1100);
    for (; PSW == 1;)
    {
      digitalWrite(LED_R, 1);
      delay(70);
      digitalWrite(LED_R, 0);
      delay(250);
      PSW = digitalRead(PEDAL_SWITCH);
    }

    PSW_A = 1;
    sendCommand(CMD_PLAY_W_VOL, 0x1F04);
    digitalWrite(LED_A, 0);
    delay(200);
    digitalWrite(LED_A, 1);
    delay(30);
    digitalWrite(LED_A, 0);
    delay(200);
    digitalWrite(LED_A, 1);
    delay(30);
    digitalWrite(LED_A, 0);

    for (led_bar = 11; led_bar >= 0; led_bar--)
    {
      digitalWrite(led_bar, 0);
      delay(30);
    }


  }

  //  delay(300);

}