#ifndef _DH_DATAPATH_H_
#define _DH_DATAPATH_H_ 1

#include "systemc.h"
#include "digit.h"
#include "dh_components.h"

// Structural datapath for the required portion of NN_DigitDivHH.
SC_MODULE(dh_datapath)
{
  sc_in_clk clock;

  sc_in<bool> load_inputs;
  sc_in<bool> load_result;

  sc_in<NN_DIGIT> t0_in, t1_in, c_in;
  sc_in<NN_HALF_DIGIT> ah_in;

  sc_out<NN_DIGIT> t0_out, t1_out, c_out;
  sc_out<NN_HALF_DIGIT> ah_out;

  // Register and datapath signals.
  sc_signal<NN_DIGIT> t0_q, t1_q, c_q;
  sc_signal<NN_HALF_DIGIT> ah_q;
  sc_signal<NN_DIGIT> t0_d, t1_d;
  sc_signal<bool> load_t;

  // C and product decomposition.
  sc_signal<NN_HALF_DIGIT> c_low, c_high;
  sc_signal<NN_DIGIT> u, v;
  sc_signal<NN_HALF_DIGIT> u_low, u_high;
  sc_signal<NN_DIGIT> u_shifted, u_high_extended;

  // Borrow and subtraction chain.
  sc_signal<bool> borrow;
  sc_signal<NN_DIGIT> borrow_digit;
  sc_signal<NN_DIGIT> t0_result;
  sc_signal<NN_DIGIT> t1_after_borrow;
  sc_signal<NN_DIGIT> t1_after_u;
  sc_signal<NN_DIGIT> t1_result;

  // Structural component instances.
  mux2_32 t0_mux, t1_mux;
  or2 t_load_or;
  reg32 t0_reg, t1_reg, c_reg;
  reg16 ah_reg;

  split32 c_split, u_split;
  multiplier u_mult, v_mult;
  to_high_half u_shift;
  extend16 u_high_extend;

  less_than32 borrow_comp;
  bool_to_digit borrow_extend;
  sub32 t0_sub, t1_borrow_sub, t1_u_sub, t1_v_sub;

  buffer32 t0_buffer, t1_buffer, c_buffer;
  buffer16 ah_buffer;

  SC_CTOR(dh_datapath)
      : t0_mux("t0_mux"), t1_mux("t1_mux"),
        t_load_or("t_load_or"),
        t0_reg("t0_reg"), t1_reg("t1_reg"), c_reg("c_reg"),
        ah_reg("ah_reg"),
        c_split("c_split"), u_split("u_split"),
        u_mult("u_mult"), v_mult("v_mult"),
        u_shift("u_shift"), u_high_extend("u_high_extend"),
        borrow_comp("borrow_comp"), borrow_extend("borrow_extend"),
        t0_sub("t0_sub"), t1_borrow_sub("t1_borrow_sub"),
        t1_u_sub("t1_u_sub"), t1_v_sub("t1_v_sub"),
        t0_buffer("t0_buffer"), t1_buffer("t1_buffer"),
        c_buffer("c_buffer"), ah_buffer("ah_buffer")
  {
    // Input/result selection for the two updated registers.
    t0_mux.A(t0_in);
    t0_mux.B(t0_result);
    t0_mux.sel(load_result);
    t0_mux.OUT(t0_d);

    t1_mux.A(t1_in);
    t1_mux.B(t1_result);
    t1_mux.sel(load_result);
    t1_mux.OUT(t1_d);

    t_load_or.A(load_inputs);
    t_load_or.B(load_result);
    t_load_or.OUT(load_t);

    // Clocked input/result registers.
    t0_reg.clock(clock);
    t0_reg.load(load_t);
    t0_reg.IN(t0_d);
    t0_reg.OUT(t0_q);

    t1_reg.clock(clock);
    t1_reg.load(load_t);
    t1_reg.IN(t1_d);
    t1_reg.OUT(t1_q);

    c_reg.clock(clock);
    c_reg.load(load_inputs);
    c_reg.IN(c_in);
    c_reg.OUT(c_q);

    ah_reg.clock(clock);
    ah_reg.load(load_inputs);
    ah_reg.IN(ah_in);
    ah_reg.OUT(ah_q);

    // cLow/cHigh and the two combinational multipliers.
    c_split.IN(c_q);
    c_split.LOW(c_low);
    c_split.HIGH(c_high);

    u_mult.A(ah_q);
    u_mult.B(c_low);
    u_mult.OUT(u);

    v_mult.A(ah_q);
    v_mult.B(c_high);
    v_mult.OUT(v);

    // Form TO_HIGH_HALF(u) and HIGH_HALF(u).
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

    // Drive the required-part outputs from the registered values.
    t0_buffer.IN(t0_q);
    t0_buffer.OUT(t0_out);

    t1_buffer.IN(t1_q);
    t1_buffer.OUT(t1_out);

    c_buffer.IN(c_q);
    c_buffer.OUT(c_out);

    ah_buffer.IN(ah_q);
    ah_buffer.OUT(ah_out);
  }
};

#endif /* end _DH_DATAPATH_H_ */
