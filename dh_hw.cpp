#include "systemc.h"
#include "digit.h"
#include "dh_hw.h"

void dh_hw::process_hw()
{

  NN_DIGIT t[2], c, u, v;
  NN_HALF_DIGIT aHigh, cLow, cHigh;

  load_inputs.write(false);
  load_result.write(false);
  
  for (;;) {  
  
	    // Read inputs (blocking FIFO access)
	    t[0] = from_sw0.read();
	    t[1] = from_sw1.read();
      c = from_sw2.read();
      aHigh = from_sw3.read();

		
	    // Original code from NN_DigitDivHH():	

      /*** Begin: Required part (to do: Datapath + Control) ***/
      cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
      cLow = (NN_HALF_DIGIT)LOW_HALF (c);

      dp_t0_in.write(t[0]);
      dp_t1_in.write(t[1]);
      dp_c_in.write(c);
      dp_ah_in.write(aHigh);
      load_inputs.write(true);

      wait(clock.posedge_event());
      load_inputs.write(false);
      load_result.write(true);

      wait(clock.posedge_event());
      load_result.write(false);
  
      wait(SC_ZERO_TIME);
      wait(SC_ZERO_TIME);

      t[0] = dp_t0_out.read();
      t[1] = dp_t1_out.read();
      /*** End: Required part ***/

      /*** Begin: Bonus part (optional: Extra Datapath + Extra Control) ***/
      while ((t[1] > cHigh) || ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
          if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow)) t[1]--;
          t[1] -= cHigh;
          aHigh++;
      }
      /*** End: Bonus part ***/
		
      // Write outputs (blocking FIFO access)
      to_sw0.write(t[0]);
      to_sw1.write(t[1]);
	    to_sw2.write(aHigh);

  }
	  	  
}

