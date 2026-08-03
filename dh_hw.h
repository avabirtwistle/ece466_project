#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "dh_datapath.h"

SC_MODULE (dh_hw)
{
  sc_in_clk clock;
  
  sc_fifo_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
  sc_fifo_in <NN_HALF_DIGIT> from_sw3;

  sc_fifo_out <NN_DIGIT> to_sw0, to_sw1;
  sc_fifo_out <NN_HALF_DIGIT> to_sw2;
  
  sc_in <bool> hw_enable; 
  sc_out <bool> hw_done;

  //signal for datapath
  sc_signal<NN_DIGIT> dp_t0_in, dp_t1_in, dp_c_in;
  sc_signal<NN_HALF_DIGIT> dp_ah_in;

  sc_signal<NN_DIGIT> dp_t0_out, dp_t1_out, dp_c_out;
  sc_signal<NN_HALF_DIGIT> dp_ah_out;

  sc_signal<bool> load_inputs, load_result;

  dh_datapath DP;

  void process_hw();
  
  SC_CTOR (dh_hw) : DP("DATAPATH")
  {
    DP.clock(clock);
    DP.load_inputs(load_inputs);
    DP.load_result(load_result);

    DP.t0_in(dp_t0_in);
    DP.t1_in(dp_t1_in);
    DP.c_in(dp_c_in);
    DP.ah_in(dp_ah_in);

    DP.t0_out(dp_t0_out);
    DP.t1_out(dp_t1_out);
    DP.c_out(dp_c_out);
    DP.ah_out(dp_ah_out);

    SC_THREAD (process_hw);
  }
  
};

#endif /* end _DH_HW_H_ */
