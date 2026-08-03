#include "systemc.h"
#include "digit.h"
#include "dh_hw.h"

void dh_hw::process_hw()
{
  NN_DIGIT t[2], c;
  NN_HALF_DIGIT aHigh, cLow, cHigh;

  for (;;) {
    wait();

    if (!load_output_internal.read()) {
      continue;
    }

    // Read the registered results from the structural datapath.
    t[0] = dp_t0_out.read();
    t[1] = dp_t1_out.read();
    c = dp_c_out.read();
    aHigh = dp_ah_out.read();

    /*** Begin: Temporary behavioral bonus part ***/
    cHigh = (NN_HALF_DIGIT)HIGH_HALF(c);
    cLow = (NN_HALF_DIGIT)LOW_HALF(c);

    while ((t[1] > cHigh) ||
           ((t[1] == cHigh) &&
            (t[0] >= TO_HIGH_HALF(cLow)))) {
      if ((t[0] -= TO_HIGH_HALF(cLow)) >
          MAX_NN_DIGIT - TO_HIGH_HALF(cLow)) {
        t[1]--;
      }

      t[1] -= cHigh;
      aHigh++;
    }
    /*** End: Temporary behavioral bonus part ***/

    // Write the final results to the software output signals.
    to_sw0.write(t[0]);
    to_sw1.write(t[1]);
    to_sw2.write(aHigh);
  }
}