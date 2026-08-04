#ifndef _DH_DATAPATH_H_
#define _DH_DATAPATH_H_ 1

#include "systemc.h"
#include "digit.h"
#include "dh_components.h"
#include "dh_bonus.h"

// Structural datapath for the required and bonus portions of NN_DigitDivHH.
SC_MODULE(dh_datapath)
{
  sc_in_clk clock;

  sc_in<bool> load_inputs; //control loading t0,t1,c,ah 
  sc_in<bool> load_result; //load require result into t0 and t1
  sc_in<bool> load_bonus;// load bonus result into t0, t1, ah

  sc_in<NN_DIGIT> t0_in; //initial t0 (low 32-bit remainder)
  sc_in<NN_DIGIT>t1_in; //initial t1 (high 32-bit remainder)
  sc_in<NN_DIGIT> c_in; //32 bit divisor
  sc_in<NN_HALF_DIGIT> ah_in; //16 bit high quotient estimate

  sc_out<NN_DIGIT> t0_out;
  sc_out<NN_DIGIT> t1_out;
  sc_out<NN_HALF_DIGIT> ah_out;

  sc_out<bool> bonus_condition;// asserted when bonus loop is required

  //
  //working registers
  //
  //current output of working registers
  sc_signal<NN_DIGIT> t0_q; 
  sc_signal<NN_DIGIT> t1_q;
  sc_signal<NN_DIGIT> c_q;
  sc_signal<NN_HALF_DIGIT> ah_q;

  //values entering the working register from software or bonus
  sc_signal<NN_DIGIT> t0_d;
  sc_signal<NN_DIGIT> t1_d;
  sc_signal<NN_DIGIT> t0_selected;
  sc_signal<NN_DIGIT> t1_selected;
  sc_signal<NN_HALF_DIGIT> ah_d;

  //register load enable signals
  sc_signal<bool> load_t_base; //load_inputs or load_result
  sc_signal<bool> load_t; //load_t_base or load_bonus
  sc_signal<bool> load_ah; // load_inputs or load_bonus

  // 2 halfs of registered divisor
  sc_signal<NN_HALF_DIGIT> c_low, c_high;

  //multiplication results
  sc_signal<NN_DIGIT> u, v;

  //two halves of product u
  sc_signal<NN_HALF_DIGIT> u_low, u_high;

  //ulow in the upp half of the 32-bit result
  sc_signal<NN_DIGIT> u_shifted;

  //u_high zero eextended to 32 bits for subtraction
  sc_signal<NN_DIGIT> u_high_extended;

  //
  // Borrow and subtraction chain
  //
  sc_signal<bool> borrow; //asserted when t0_q is smaller than u_shifted
  sc_signal<NN_DIGIT> borrow_digit; 
  sc_signal<NN_DIGIT> t0_result;//Required low-word result: t0_q - u_shifted
  sc_signal<NN_DIGIT> t1_after_borrow;//t1_q - borrow
  sc_signal<NN_DIGIT> t1_after_u;//t1_after_borrow - u_high_extended
  sc_signal<NN_DIGIT> t1_result;//t1_after_u - v

  //
  //results from one structural bonus-loop iteration
  //
  sc_signal<NN_DIGIT> bonus_t0_next;
  sc_signal<NN_DIGIT> bonus_t1_next;
  sc_signal<NN_HALF_DIGIT> bonus_ah_next; //incremented by one bonus

  //
  //structural component instances
  //
  //first stage, select input values or required part results
  mux2_32 t0_mux;
  mux2_32 t1_mux;

  //second stage, select required part results or bonus results
  mux2_32 t0_bonus_mux;
  mux2_32 t1_bonus_mux;
  mux2_16 ah_bonus_mux;


  // Logic used to generate register load enables
  or2 t_load_or;
  or2 t_bonus_load_or;
  or2 ah_load_or;

  // Working registers
  reg32 t0_reg;
  reg32 t1_reg;
  reg32 c_reg;
  reg16 ah_reg;

  //combinational circuit that calculates one bonus-loop iteration
  dh_bonus_step bonus_step;

  //split the divisor and product u into 16-bit halves
  split32 c_split;
  split32 u_split;

  //16-by-16-bit multipliers with 32-bit results
  multiplier u_mult;
  multiplier v_mult;

  //convert u_low into u_low << 16
  to_high_half u_shift;

  //zero-extend u_high from 16 to 32 bits
  extend16 u_high_extend;

  //Detect the unsigned subtraction borrow
  less_than32 borrow_comp;

  //convert the one-bit borrow into a 32-bit subtraction operand.
  bool_to_digit borrow_extend;

  // Required arithmetic subtractors
  sub32 t0_sub;
  sub32 t1_borrow_sub;
  sub32 t1_u_sub;
  sub32 t1_v_sub;

  // Connect registered working values to the datapath output ports
  buffer32 t0_buffer;
  buffer32 t1_buffer;
  buffer16 ah_buffer;

  SC_CTOR(dh_datapath)//name components
      : t0_mux("t0_mux"), t1_mux("t1_mux"),
        t0_bonus_mux("t0_bonus_mux"), t1_bonus_mux("t1_bonus_mux"),
        ah_bonus_mux("ah_bonus_mux"),
        t_load_or("t_load_or"), t_bonus_load_or("t_bonus_load_or"),
        ah_load_or("ah_load_or"),
        t0_reg("t0_reg"), t1_reg("t1_reg"), c_reg("c_reg"),
        ah_reg("ah_reg"),
        bonus_step("bonus_step"),
        c_split("c_split"), u_split("u_split"),
        u_mult("u_mult"), v_mult("v_mult"),
        u_shift("u_shift"), u_high_extend("u_high_extend"),
        borrow_comp("borrow_comp"), borrow_extend("borrow_extend"),
        t0_sub("t0_sub"), t1_borrow_sub("t1_borrow_sub"),
        t1_u_sub("t1_u_sub"), t1_v_sub("t1_v_sub"),
        t0_buffer("t0_buffer"), t1_buffer("t1_buffer"),
        ah_buffer("ah_buffer")
  {
    //
    //Require result MUX
    //
    // Select software inputs or required-part results.
    t0_mux.A(t0_in); //external input from software
    t0_mux.B(t0_result); //require path result from subtractor chain
    t0_mux.sel(load_result);
    t0_mux.OUT(t0_d);

    t1_mux.A(t1_in);//external t1 input from software
    t1_mux.B(t1_result); //require path result from subtractor chain
    t1_mux.sel(load_result);
    t1_mux.OUT(t1_d);

    //
    //Bonus MUX
    //
    // Select one bonus-loop result when load_bonus is asserted.
    t0_bonus_mux.A(t0_d); //normal t0 value from required path
    t0_bonus_mux.B(bonus_t0_next); //bonus-loop result t0 from bonus_step
    t0_bonus_mux.sel(load_bonus);
    t0_bonus_mux.OUT(t0_selected); //sent to t0_reg

    t1_bonus_mux.A(t1_d); //normal t1
    t1_bonus_mux.B(bonus_t1_next); //bonus-loop result t1 from bonus_step
    t1_bonus_mux.sel(load_bonus);
    t1_bonus_mux.OUT(t1_selected); //sent to t1_reg

    ah_bonus_mux.A(ah_in); //initial ah value from software
    ah_bonus_mux.B(bonus_ah_next); //incremented ah from bonus_step
    ah_bonus_mux.sel(load_bonus);
    ah_bonus_mux.OUT(ah_d);//to ah_reg

    // -------------------------------------------------------------------------
    // REGISTER LOAD-ENABLE GENERATION
    // -------------------------------------------------------------------------
    t_load_or.A(load_inputs); //load software input
    t_load_or.B(load_result); //load arithmetic results
    t_load_or.OUT(load_t_base);

    t_bonus_load_or.A(load_t_base); //result from prev (software or arithmetic)
    t_bonus_load_or.B(load_bonus); //load bonus
    t_bonus_load_or.OUT(load_t);

    ah_load_or.A(load_inputs);//load initial software input
    ah_load_or.B(load_bonus); //load bonus_ah_next
    ah_load_or.OUT(load_ah);

    t0_reg.clock(clock);
    t0_reg.load(load_t);
    t0_reg.IN(t0_selected);
    t0_reg.OUT(t0_q);

    t1_reg.clock(clock);
    t1_reg.load(load_t);
    t1_reg.IN(t1_selected);
    t1_reg.OUT(t1_q);

    c_reg.clock(clock);
    c_reg.load(load_inputs);
    c_reg.IN(c_in);
    c_reg.OUT(c_q);

    ah_reg.clock(clock);
    ah_reg.load(load_ah);
    ah_reg.IN(ah_d);
    ah_reg.OUT(ah_q);

    // Split c once for both the required and bonus datapaths
    c_split.IN(c_q);
    c_split.LOW(c_low);
    c_split.HIGH(c_high);

    // Structural bonus block computes one loop iteration at a time
    bonus_step.t0(t0_q);
    bonus_step.t1(t1_q);
    bonus_step.c_low(c_low);
    bonus_step.c_high(c_high);
    bonus_step.a_high(ah_q);
    bonus_step.t0_next(bonus_t0_next);
    bonus_step.t1_next(bonus_t1_next);
    bonus_step.a_high_next(bonus_ah_next);
    bonus_step.loop_condition(bonus_condition);

    // Required-part multiplication.
    u_mult.A(ah_q);
    u_mult.B(c_low);
    u_mult.OUT(u);

    v_mult.A(ah_q);
    v_mult.B(c_high);
    v_mult.OUT(v);

    u_split.IN(u);
    u_split.LOW(u_low);
    u_split.HIGH(u_high);

    u_shift.IN(u_low);
    u_shift.OUT(u_shifted);

    u_high_extend.IN(u_high);
    u_high_extend.OUT(u_high_extended);

    // t0_result = t0 - TO_HIGH_HALF(u), plus unsigned borrow.
    borrow_comp.A(t0_q);
    borrow_comp.B(u_shifted);
    borrow_comp.LT(borrow);

    t0_sub.A(t0_q);
    t0_sub.B(u_shifted);
    t0_sub.OUT(t0_result);

    // t1_result = t1 - borrow - HIGH_HALF(u) - v.
    borrow_extend.IN(borrow);
    borrow_extend.OUT(borrow_digit);

    t1_borrow_sub.A(t1_q);
    t1_borrow_sub.B(borrow_digit);
    t1_borrow_sub.OUT(t1_after_borrow);

    t1_u_sub.A(t1_after_borrow);
    t1_u_sub.B(u_high_extended);
    t1_u_sub.OUT(t1_after_u);

    t1_v_sub.A(t1_after_u);
    t1_v_sub.B(v);
    t1_v_sub.OUT(t1_result);

    // Drive the datapath outputs from the registered working values.
    t0_buffer.IN(t0_q);
    t0_buffer.OUT(t0_out);

    t1_buffer.IN(t1_q);
    t1_buffer.OUT(t1_out);

    ah_buffer.IN(ah_q);
    ah_buffer.OUT(ah_out);
  }
};

#endif /* end _DH_DATAPATH_H_ */
