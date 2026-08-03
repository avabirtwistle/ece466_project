#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "dh_controller.h"

SC_MODULE (dh_hw){
  sc_in_clk clock;

  sc_in<NN_DIGIT> from_sw0, from_sw1, from_sw2;
  sc_in<NN_HALF_DIGIT> from_sw3;

  sc_out<NN_DIGIT> to_sw0, to_sw1;
  sc_out<NN_HALF_DIGIT> to_sw2;

  sc_in <bool> hw_enable; 
  sc_out <bool> hw_done;

  // Required-datapath outputs
  sc_signal<NN_DIGIT> dp_t0_out, dp_t1_out, dp_c_out;
  sc_signal<NN_HALF_DIGIT> dp_ah_out;

  // Controller-to-datapath control signals
  sc_signal<bool> load_inputs_internal;
  sc_signal<bool> load_result_internal;
  
  // Reserved for the bonus and final output registers
  sc_signal<bool> bonus_ready_internal;
  sc_signal<bool> load_output_internal;

  dh_datapath DP;
  dh_controller controller;

  void process_hw();
  
  SC_CTOR (dh_hw) : DP("DATAPATH") : controller("CONTROLLER")
  {
    DP.clock(clock);
    DP.load_inputs(load_inputs_internal);
    DP.load_result(load_result_internal);

    DP.t0_in(from_sw0);
    DP.t1_in(from_sw1);
    DP.c_in(from_sw2);
    DP.ah_in(from_sw3);

    DP.t0_out(dp_t0_out);
    DP.t1_out(dp_t1_out);
    DP.c_out(dp_c_out);
    DP.ah_out(dp_ah_out);

    // Controller connections
    controller.clock(clock);
    controller.hw_enable(hw_enable);
    controller.hw_done(hw_done);
    
    controller.load_inputs(load_inputs_internal);
    controller.load_result(load_result_internal);
    controller.bonus_ready(bonus_ready_internal);
    controller.load_output(load_output_internal);
    
    SC_CTHREAD(process_hw, clock.pos());
  }
};
#endif