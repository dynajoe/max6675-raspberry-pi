{
	"targets": [
		{
			"target_name":  "Max6675",
			"sources":      [ "src/addon.cc", "src/max6675.cc" ],
			"include_dirs": [ "../lib/wiringPi/wiringPi" ],
			"ldflags":      [ "-lwiringPi" ]
		}
	]
}
