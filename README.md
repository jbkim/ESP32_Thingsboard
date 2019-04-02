# ESP32_Thingsboard
ESP32 IoT service test - Thingsboard

## Thingsboard 사용법
### 개요
- Thingsboard는 원도우즈, 맥, 리눅스에 설치가능
- cloud.thingsboard.io 클라우드에 가입해서 사용 가능

## 모니터링
1. 디바이스 그룹 및 디바이스 추가
- 회원 가입
- Device Group -> Add new entity group
- Add Device
- 이 디바이스의 access token을 펌웨어에서 사용해서 연결이 된다. 

2. Dashboard
- DASHBOARD GOUPS -> Add new entity
- Add Dashboard
- Dashboard에 Widget 추가
- 추가된 위젯에 데이터를 연결. 이때 timeseries의 파라미터 값은 소스코드에서 데이터를 전송하는 파라미터 값과 같아야 한다.

참고: [ESP32 Pico Kit GPIO control and DHT22 sensor monitor using ThingsBoard Arduino SDK](https://thingsboard.io/docs/samples/esp32/gpio-control-pico-kit-dht22-sensor/)

## 컨트롤
- 대쉬보드의 위젯을 통해서 원격디바이스를 컨트롤 할 경우 RPC(Remote Procedure Call)를 사용해야 한다.
- 위젯에서 보드의 LED를 on/off 하는 경우 먼저 RPC_Subscribe를 해야 함

~~~cpp
    // Perform a subscription. All consequent data processing will happen in
    // callbacks as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks, callbacks_size)) {
        Serial.println("Failed to subscribe for RPC");
        return;
    }
~~~
- RPC_Callback 함수에서 LED를 on/off하는 함수를 만들어야 함
- 중요한 것은 위젯설정의 Advanced 탭에서 RPC get value method, RPC set value method의 이름과 코드의 RPC_Callback 함수에 등록한 이름이 같아야 한다. 아래 예제의 경우 setVale라는 이름.
 ~~~cpp
    RPC_Response processSetLED(const RPC_Data &data)
    {
    int tmp = data;

    Serial.print("Recv data: ");
    Serial.println(tmp);

    digitalWrite(LEDpin,!(digitalRead(LEDpin)));

    return RPC_Response("toggleGpio", digitalRead(LEDpin));
    }

    RPC_Callback callbacks[callbacks_size] = {
    { "setValue", processSetLED },
    };
 ~~~