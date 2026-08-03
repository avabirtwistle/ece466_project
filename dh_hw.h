#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "dh_controller.h"
#include "dh_datapath.h"

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
  sc_signal<bool> bonus_condition_internal;
  sc_signal<bool> load_output_internal;

  dh_datapath DP;
  dh_controller controller;
  reg32 t0_output_reg, t1_output_reg;
  reg16 ah_output_reg;


  
SC_CTOR (dh_hw)
      : DP("DATAPATH"), controller("CONTROLLER"),
        t0_output_reg("t0_output_reg"),
        t1_output_reg("t1_output_reg"),
        ah_output_reg("ah_output_reg")
  {
    DP.clock(clock);
    DP.load_inputs(load_inputs_internal);
    DP.load_result(load_result_internal);
    DP.load_bonus(bonus_ready_internal);

    DP.t0_in(from_sw0);
    DP.t1_in(from_sw1);
    DP.c_in(from_sw2);
    DP.ah_in(from_sw3);

    DP.t0_out(dp_t0_out);
    DP.t1_out(dp_t1_out);
    DP.c_out(dp_c_out);
    DP.ah_out(dp_ah_out);
    DP.bonus_condition(bonus_condition_internal);

   // Controller connections
    controller.clock(clock);
    controller.hw_enable(hw_enable);
    controller.bonus_condition(bonus_condition_internal);
    controller.hw_done(hw_done);
    
    controller.load_inputs(load_inputs_internal);
    controller.load_result(load_result_internal);
    controller.bonus_ready(bonus_ready_internal);
    controller.load_output(load_output_internal);
    
    // Final output registers hold stable values while hw_done is asserted.
    t0_output_reg.clock(clock);
    t0_output_reg.load(load_output_internal);
    t0_output_reg.IN(dp_t0_out);
    t0_output_reg.OUT(to_sw0);
  
    t1_output_reg.clock(clock);
    t1_output_reg.load(load_output_internal);
    t1_output_reg.IN(dp_t1_out);
    t1_output_reg.OUT(to_sw1);

    ah_output_reg.clock(clock);
    ah_output_reg.load(load_output_internal);
    ah_output_reg.IN(dp_ah_out);
    ah_output_reg.OUT(to_sw2);
  }
};
#endif