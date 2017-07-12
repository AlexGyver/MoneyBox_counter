/*
  Created 2017
  by AlexGyver
  AlexGyver Home Labs Inc.
*/

//-------НАСТРОЙКИ---------
#define coin_amount 15    // число монет, которые нужно распознать

// стоимость монет, для повышения точности необходимо выбрать по 3 монетки каждого номинала,
// максимально отличающихся друг от друга. Например блестящую, матовую и средней загрязненности
float coin_value[coin_amount] = {0.5, 1.0, 2.0, 5.0, 10.0, 0.5, 1.0, 2.0, 5.0, 10.0, 0.5, 1.0, 2.0, 5.0, 10.0};

String currency = "RUB"; // валюта (английские буквы!!!)
int stb_time = 10000;    // время бездействия, через которое система уйдёт в сон (миллисекунды)
//-------НАСТРОЙКИ---------

int coin_signal[coin_amount];    // тут хранится значение сигнала для каждого размера монет
int coin_quantity[coin_amount];  // количество монет
byte empty_signal;               // храним уровень пустого сигнала
unsigned long standby_timer, reset_timer; // таймеры
float summ_money = 0;            // сумма монет в копилке

//-------БИБЛИОТЕКИ---------
#include "LowPower.h"
#include "EEPROMex.h"
#include "LCD_1602_RUS.h"
//-------БИБЛИОТЕКИ---------

LCD_1602_RUS lcd(0x27, 16, 2);            // создать дисплей
boolean sleep_flag = true;   // флажки
//-------КНОПКИ---------
byte button = 2;         // кнопка "проснуться"
byte calibr_button = 3;  // скрытая кнопка калибровкии сброса
byte disp_power = 12;    // питание дисплея
byte LEDpin = 11;        // питание светодиода
byte IRpin = 17;         // питание фототранзистора
byte IRsens = 14;        // сигнал фототранзистора
//-------КНОПКИ---------
int sens_signal, last_sens_signal;
boolean coin_flag = false;

void setup() {
  Serial.begin(9600);                   // открыть порт для связи с ПК для отладки
  delay(500);

  // подтягиваем кнопки
  pinMode(button, INPUT_PULLUP);
  pinMode(calibr_button, INPUT_PULLUP);

  // пины питания как выходы
  pinMode(disp_power, OUTPUT);
  pinMode(LEDpin, OUTPUT);
  pinMode(IRpin, OUTPUT);

  // подать питание на дисплей и датчик
  digitalWrite(disp_power, 1);
  digitalWrite(LEDpin, 1);
  digitalWrite(IRpin, 1);

  // подключить прерывание
  attachInterrupt(0, wake_up, CHANGE);

  empty_signal = analogRead(IRsens);  // считать пустой (опорный) сигнал

  // инициализация дисплея
  lcd.init();
  lcd.backlight();

  if (!digitalRead(calibr_button)) {  // если при запуске нажата кнопка калибровки
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(L"Сервис");
    delay(500);
    reset_timer = millis();
    while (1) {                                   // бесконечный цикл
      if (millis() - reset_timer > 3000) {        // если кнопка всё ещё удерживается и прошло 3 секунды
        // очистить количество монет
        for (byte i = 0; i < coin_amount; i++) {
          coin_quantity[i] = 0;
          EEPROM.writeInt(20 + i * 2, 0);
        }
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(L"Память очищена");
        delay(100);
      }
      if (digitalRead(calibr_button)) {   // если отпустили кнопку, перейти к калибровке
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(L"Калибровка");
        break;
      }
    }
    while (1) {
      for (byte i = 0; i < coin_amount; i++) {
        lcd.setCursor(0, 1); lcd.print(coin_value[i]);  // отобразить цену монеты, размер которой калибруется
        lcd.setCursor(13, 1); lcd.print(currency);      // отобразить валюту
        last_sens_signal = empty_signal;
        while (1) {
          sens_signal = analogRead(IRsens);                                    // считать датчик
          if (sens_signal > last_sens_signal) last_sens_signal = sens_signal;  // если текущее значение больше предыдущего
          if (sens_signal - empty_signal > 3) coin_flag = true;                // если значение упало почти до "пустого", считать что монета улетела
          if (coin_flag && (abs(sens_signal - empty_signal)) < 2) {            // если монета точно улетела
            coin_signal[i] = last_sens_signal;                                 // записать максимальное значение в память
            EEPROM.writeInt(i * 2, coin_signal[i]);
            coin_flag = false;
            break;
          }
        }
      }
      break;
    }
  }

  // при старте системы считать из памяти сигналы монет для дальнейшей работы, а также их количество в банке
  for (byte i = 0; i < coin_amount; i++) {
    coin_signal[i] = EEPROM.readInt(i * 2);
    coin_quantity[i] = EEPROM.readInt(20 + i * 2);
    summ_money += coin_quantity[i] * coin_value[i];  // ну и сумму сразу посчитать, как произведение цены монеты на количество
  }

  /*
    // для отладки, вывести сигналы монет в порт
    for (byte i = 0; i < coin_amount; i++) {
      Serial.print(coin_value[i]);
      Serial.print(": ");
      Serial.println(coin_signal[i]);
    }
  */

  standby_timer = millis();  // обнулить таймер ухода в сон
}

void loop() {
  if (sleep_flag) {  // если проснулись  после сна, инициализировать дисплей и вывести текст, сумму и валюту
    delay(500);
    lcd.init();
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(L"На мото");
    lcd.setCursor(0, 1); lcd.print(summ_money);
    lcd.setCursor(13, 1); lcd.print(currency);
    empty_signal = analogRead(IRsens);
    sleep_flag = false;
  }

  // далее работаем в бесконечном цикле
  last_sens_signal = empty_signal;
  while (1) {
    sens_signal = analogRead(IRsens);  // далее такой же алгоритм, как при калибровке
    if (sens_signal > last_sens_signal) last_sens_signal = sens_signal;
    if (sens_signal - empty_signal > 3) coin_flag = true;
    if (coin_flag && (abs(sens_signal - empty_signal)) < 2) {


      // алгоритм сравнивает последовательно сигнал каждой монетки с last_sens_signal
      // и находит монетку, наиболее подходящую под сигнал (минимально отличающуюся от last_sens_signal)
      int delta = 1000; // для временной записи минимального значения
      int min_i = 0; // номер минимальной монетки
      for (byte i = 0; i < coin_amount; i++) {
        if (delta > abs(last_sens_signal - coin_signal[i]) ) {
          delta = abs(last_sens_signal - coin_signal[i]) ;
          min_i = i;
        }
      }
      
      // для отладки, вывести номинал найденной монетки
      // Serial.println(coin_value[min_i]);

      summ_money += coin_value[min_i];  // к сумме тупо прибавляем цену монетки (дада, сумма считается двумя разными способами. При старте системы суммой всех монет, а тут прибавление
      lcd.setCursor(0, 1); lcd.print(summ_money);
      coin_quantity[min_i]++;  // для распознанного номера монетки прибавляем количество

      coin_flag = false;
      standby_timer = millis();  // сбросить таймер
      break;
    }

    // если ничего не делали, времят аймера вышло, спим
    if (millis() - standby_timer > stb_time) {
      good_night();
      break;
    }

    // если монетка вставлена (замыкает контакты) и удерживается 2 секунды
    while (!digitalRead(button)) {
      if (millis() - standby_timer > 2000) {
        lcd.clear();

        // отобразить на дисплее: сверху цены монет (округлено до целых!!!!), снизу их количество
        for (byte i = 0; i < coin_amount; i++) {
          lcd.setCursor(i * 3, 0); lcd.print((int)coin_value[i]);
          lcd.setCursor(i * 3, 1); lcd.print(coin_quantity[i]);
        }
      }
    }
  }
}

// функция сна
void good_night() {
  // перед тем как пойти спать, записываем в EEPROM новые полученные количества монет по адресам начиная с 20го (пук кек)
  for (byte i = 0; i < coin_amount; i++) {
    EEPROM.updateInt(20 + i * 2, coin_quantity[i]);
  }
  sleep_flag = true;
  // вырубить питание со всех дисплеев и датчиков
  digitalWrite(disp_power, 0);
  digitalWrite(LEDpin, 0);
  digitalWrite(IRpin, 0);
  delay(100);
  // и вот теперь спать
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

// просыпаемся по ПРЕРЫВАНИЮ (эта функция - обработчик прерывания)
void wake_up() {
  // возвращаем питание на дисплей и датчик
  digitalWrite(disp_power, 1);
  digitalWrite(LEDpin, 1);
  digitalWrite(IRpin, 1);
  standby_timer = millis();  // и обнуляем таймер
}
// Спасибо за внимание, ваш Алекс
