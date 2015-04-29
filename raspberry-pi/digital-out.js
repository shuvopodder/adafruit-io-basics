var GpioStream = require('gpio-stream'),
    light = GpioStream.writable(17),
    AIO = require('adafruit-io');

// replace xxxxxxxxxxx with your Adafruit IO key
var AIO_KEY = 'xxxxxxxxxxx';

// aio init
var aio = AIO(AIO_KEY);

// pipe light data to the powerswitch tail
aio.feeds('Light').pipe(light);

console.log('listening for light data...');