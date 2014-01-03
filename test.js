var Max6675 = require('./build/Release/Max6675.node');

var sensor = new Max6675({
   miso: 0,
   ss: 2,
   clk: 3
});

setInterval(function () {
	var value = sensor.read();
	console.log(value)
}, 1000);

