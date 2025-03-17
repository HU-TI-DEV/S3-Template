# http_led_control

Derived from the ESP-IDF example in `esp-idf\examples\protocols\http_server\simple`.

The Example consists of HTTPD server demo with demonstration of URI handling :
    1. URI \hello for GET command returns "Hello World!" message
    2. URI \echo for POST command echoes back the POSTed message

### Configure the project

```bash
idf.py set-target esp32
idf.py menuconfig
```

* Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi credentials.
* You can also select the GPIO pin where the LED is connected to. Default is GPIO 2 (ESP32 built-in LED pin).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

* Replace PORT with the name of the serial port to use.
  (If you omit `-p PORT`, the ESP-IDF will try to autodetect it.)
* To exit the serial monitor, type ``Ctrl-]``.

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Test http_led_control -- GET

**GET request example URL**

If the IP address is `10.12.66.111`, you can control the LED with these URIs:

* turn **off** LED with ```http://10.12.66.111/led?value=0```
* turn **on** LED with ```http://10.12.66.111/led?value=1```

**GET request example session**

```text
I (4634) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 10.12.66.111
I (4640) example_common: Connected to example_netif_sta
I (4645) example_common: - IPv4 address: 10.12.66.111,
I (4651) gpio: GPIO[2]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (4658) http_led_control: init_led_pin()
I (5662) http_led_control: set_led_pin(1)
I (6662) http_led_control: set_led_pin(0)
I (7662) http_led_control: set_led_pin(1)
I (8662) http_led_control: set_led_pin(0)
I (9662) http_led_control: set_led_pin(1)
I (10662) http_led_control: set_led_pin(0)
I (10662) http_led_control: Starting server on port: '80'
I (10666) http_led_control: Registering URI handlers
I (28516) http_led_control: Found header => Host: 10.12.66.111
I (28516) http_led_control: Found URL query => value=1
I (28516) http_led_control: Found URL query parameter => value=1
I (28521) http_led_control: Decoded query parameter => 1
I (28526) http_led_control: set_led_pin(1)
I (34143) http_led_control: Found header => Host: 10.12.66.111
I (34144) http_led_control: Found URL query => value=0
I (34144) http_led_control: Found URL query parameter => value=0
I (34149) http_led_control: Decoded query parameter => 0
I (34154) http_led_control: set_led_pin(0)
```

### Test http_led_control -- POST

POST requests are not so easy to make as GET requests (which you can type in the URL line of your browser).

If you don't have a Python script to make one you might consider installing a browser extension like [Bruno](https://www.usebruno.com/).

Another way is to use one of the methods available for [Javascript in a simple HTML file](https://www.freecodecamp.org/news/javascript-post-request-how-to-send-an-http-post-request-in-js/).

We will use the tool `curl` (which is anyway good to know, as it is available for all operating systems).
[Website with some curl POST examples](https://gist.github.com/subfuzion/08c5d85437d5d4f00e58).

**POST request example**

```bash
curl -d "value=1" -H "Content-Type: application/x-www-form-urlencoded" -X POST http://10.12.66.111:80/led
```

Parameters:

* `-d` is the data to be sent. You can also send files.
* `-H` set a HTTP 'header', in this case the Content-Type header.
* `-X` HTTP request type (usually GET or POST when querying data from a site).

The last part of the command line is the URL to the server (here I included the standard HTTP port number).

As you might notice, the URL is the same for GET and POST - the web server can distinguish the 'route' it needs to process by the request type.

POST is much more flexible than GET - you can also POST JSON data, for example.

**POST request example session**

```text
I (1047981) http_led_control: =========== RECEIVED DATA ==========
I (1047981) http_led_control: value=1
I (1047981) http_led_control: ====================================
I (1047985) http_led_control: set_led_pin(1)
I (1054422) http_led_control: =========== RECEIVED DATA ==========
I (1054423) http_led_control: value=0
I (1054423) http_led_control: ====================================
I (1054427) http_led_control: set_led_pin(0)
I (1061078) http_led_control: =========== RECEIVED DATA ==========
I (1061079) http_led_control: value=1
I (1061079) http_led_control: ====================================
I (1061083) http_led_control: set_led_pin(1)
```
