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

  //wires between the dh_controller and the dh_datapath
  sc_signal<bool> load_inputs_internal;
  sc_signal<bool> load_result_internal;
  sc_signal<bool> bonus_ready_internal;
  sc_signal<bool> load_output_internal;

  dh_controller controller;
  void process_hw();
  
  SC_CTOR (dh_hw): controller("CONTROLLER"){
    controller.clock(clock);
    controller.hw_enable(hw_enable);
    controller.load_inputs(load_inputs_internal);
    controller.load_result(load_result_internal);
    controller.bonus_ready(bonus_ready_internal);
    controller.load_output(load_output_internal);
    controller.hw_done(hw_done);
    
    SC_THREAD (process_hw, clock.pos());
  }
  
};

#endif /* end _DH_HW_H_ */
