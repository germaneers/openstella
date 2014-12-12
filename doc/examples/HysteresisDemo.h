#include <openstella/generics/Hysteresis.h>

void HysteresisDemo() {
	Hysteresis<int, 10> hyst;

	int a = hyst.calculate(15); // first Value is always applied, a==15
	int b = hyst.calculate(20); // difference to last value==5, threshold not exceeded, b==15
	int c = hyst.calculate(26); // difference to last value==11, threshold exceeded, c==26
	int d = hyst.calculate(20); // difference to last value==6, threshold not exceeded, d==26
	int e = hyst.calculate(35); // difference to last value==9, threshold not exceeded, d==26
	int e = hyst.calculate(40); // difference to last value==14, threshold exceeded, d==40

}
