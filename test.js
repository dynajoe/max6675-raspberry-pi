var Max6675 = require('max6675-raspberry-pi');

var sensor = new Max6675({
   miso: 0,
   ss: 2,
   clk: 3
});

setTimeout(function () {
	console.log(sensor.read());
});