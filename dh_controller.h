#ifndef DH_CONTROLLER_H
#define DH_CONTROLLER_H

#include "systemc.h"

enum dh_ctrl_state {
  DH_IDLE,
  DH_LOAD,
  DH_CALCULATE,
  DH_BONUS,
  DH_BONUS_UPDATE,
  DH_OUTPUT,
  DH_DONE
};

SC_MODULE(dh_controller)
{
  sc_in_clk clock;
  sc_in<bool> hw_enable;
  sc_in<bool> bonus_condition;

  sc_out<bool> load_inputs; //load input from software
  sc_out<bool> load_result; //capture result to t0 and t1 register (required part)
  sc_out<bool> bonus_ready; //indicate when bonus input is ready
  sc_out<bool> load_output; //capture result to output registers (bonus part)
  sc_out<bool> hw_done; //tell software hardware is done and output registers are valid

  sc_signal<dh_ctrl_state> state;
  sc_signal<dh_ctrl_state> next_state;

  void state_reg();
  void state_diagram();

  SC_CTOR(dh_controller)
  {
    SC_CTHREAD(state_reg, clock.pos());

    SC_METHOD(state_diagram);
    sensitive << state << hw_enable << bonus_condition;
  }
};

#endif