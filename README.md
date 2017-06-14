# Копилка со счётчиком монет

## Папки

**LCD_1602_RUS-master** - библиотека для дисплея, установить в C:\Program Files\Arduino\libraries
  
**money_box_counter** - прошивка для Arduino

## Схема питания от USB
![СХЕМА](https://github.com/AlexGyver/MoneyBox_counter/blob/master/scheme1.jpg)

## Схема питания от аккумулятора через мосфет
![СХЕМА](https://github.com/AlexGyver/MoneyBox_counter/blob/master/scheme2.jpg)

##  Материалы и компоненты
Всё указанное ниже можно найти здесь
http://alexgyver.ru/arduino_shop/

* Arduino NANO http://ali.pub/uxbqf
* Дисплей http://ali.pub/oitu5
* Датчик http://ali.pub/1kamf3
* Повышайка http://ali.pub/1ingxt
* Кнопки и прочее http://alexgyver.ru/electronics/
* Мосфеты
* IRF3704ZPBF
* IRF3205PBF
* IRLB8743PBF
* IRL2203NPBF
* IRLB8748PBF
* IRF3704PBF
* IRL8113PBF
* IRL3803PBF
* IRLB3813PBF
* IRL3502PBF
* IRL2505PBF
* IRF3711PBF
* IRL3713PBF
* IRF3709ZPBF
* AUIRL3705N
* IRLB3034PBF
* IRF3711ZPBF

## Вам также пригодится 
* Всё для пайки http://alexgyver.ru/all-for-soldering/
* Электронные компоненты http://alexgyver.ru/electronics/

## HOW TO
* Нажать и удерживать кнопку калибровки, затем подать питание/перезагрузить Arduino
* Если отпустить кнопку калибровки, система перейдёт в режим калибровки
* Если удерживать ещё 3 секунды - режим очистки памяти (сброс числа монет)
* После окончания калибровки система сама перейдёт в обычный режим работы