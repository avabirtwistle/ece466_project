#include "systemc.h"
#include "digit.h"
#include "dh_sw.h"
#include "dh_hw.h"

int sc_main(int argc , char *argv[]) {

  // Important: All FIFOs are of size 1.
  sc_fifo <NN_DIGIT> ch0(1), ch1(1), ch2(1), ch4(1), ch5(1);
  sc_fifo <NN_HALF_DIGIT> ch3(1), ch6(1);
	
	sc_signal <bool> enable, done;
	enable.write(false);
	done.write(false);
	
	dh_sw DH_SW("DH_Software_Module");
	DH_SW.to_hw0(ch0);                 // output0 to hardware
	DH_SW.to_hw1(ch1);                 // output1 to hardware
	DH_SW.to_hw2(ch2);                 // output2 to hardware
	DH_SW.to_hw3(ch3);                 // output3 to hardware
 	DH_SW.from_hw0(ch4);               // input from hardware
 	DH_SW.from_hw1(ch5);               // input from hardware
 	DH_SW.from_hw2(ch6);               // input from hardware
	DH_SW.hw_enable(enable);           // hardware-enable output
	DH_SW.hw_done(done);               // hardware-done input
		
	dh_hw DH_HW("DH_Hardware_Module");
	DH_HW.from_sw0(ch0);               // input0 from software
	DH_HW.from_sw1(ch1);               // input1 from software
	DH_HW.from_sw2(ch2);               // input2 from software
	DH_HW.from_sw3(ch3);               // input3 from software
 	DH_HW.to_sw0(ch4);                 // output to software
 	DH_HW.to_sw1(ch5);                 // output to software
 	DH_HW.to_sw2(ch6);                 // output to software
	DH_HW.hw_enable(enable);           // hardware-enable input
	DH_HW.hw_done(done);               // hardware-done output

	sc_start();

	return(0);
 
}
