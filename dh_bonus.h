#ifndef _DH_BONUS_H_
#define _DH_BONUS_H_ 1

#include "systemc.h"
#include "digit.h"
#include "dh_components.h"

// Structural combinational datapath for one bonus-loop iteration.
SC_MODULE(dh_bonus_step)
{
  sc_in<NN_DIGIT> t0, t1;
  sc_in<NN_HALF_DIGIT> c_low, c_high, a_high;

  sc_out<NN_DIGIT> t0_next, t1_next;
  sc_out<NN_HALF_DIGIT> a_high_next;
  sc_out<bool> loop_condition;

  sc_signal<NN_DIGIT> c_low_shifted, c_high_extended;
  sc_signal<NN_DIGIT> borrow_digit, t1_after_borrow;
  sc_signal<bool> borrow;
  sc_signal<bool> t1_greater, t1_equal, t0_greater_equal;
  sc_signal<bool> equal_and_greater_equal;

  to_high_half c_low_shift;
  extend16 c_high_extend;

  less_than32 borrow_comp, t1_greater_comp;
  equal32 t1_equal_comp;
  not1 t0_greater_equal_not;
  and2 equal_condition_and;
  or2 loop_condition_or;

  bool_to_digit borrow_extend;
  sub32 t0_sub, t1_borrow_sub, t1_c_high_sub;
  increment16 a_high_increment;

  SC_CTOR(dh_bonus_step)
      : c_low_shift("c_low_shift"),
        c_high_extend("c_high_extend"),
        borrow_comp("borrow_comp"),
        t1_greater_comp("t1_greater_comp"),
        t1_equal_comp("t1_equal_comp"),
        t0_greater_equal_not("t0_greater_equal_not"),
        equal_condition_and("equal_condition_and"),
        loop_condition_or("loop_condition_or"),
        borrow_extend("borrow_extend"),
        t0_sub("t0_sub"),
        t1_borrow_sub("t1_borrow_sub"),
        t1_c_high_sub("t1_c_high_sub"),
        a_high_increment("a_high_increment")
  {
    c_low_shift.IN(c_low);
    c_low_shift.OUT(c_low_shifted);

    c_high_extend.IN(c_high);
    c_high_extend.OUT(c_high_extended);

    // The same comparison supplies the subtraction borrow and t0 >= limit.
    borrow_comp.A(t0);
    borrow_comp.B(c_low_shifted);
    borrow_comp.LT(borrow);

    t0_greater_equal_not.IN(borrow);
    t0_greater_equal_not.OUT(t0_greater_equal);

    // t1 > cHigh is equivalent to cHigh < t1.
    t1_greater_comp.A(c_high_extended);
    t1_greater_comp.B(t1);
    t1_greater_comp.LT(t1_greater);

    t1_equal_comp.A(t1);
    t1_equal_comp.B(c_high_extended);
    t1_equal_comp.EQ(t1_equal);

    equal_condition_and.A(t1_equal);
    equal_condition_and.B(t0_greater_equal);
    equal_condition_and.OUT(equal_and_greater_equal);

    loop_condition_or.A(t1_greater);
    loop_condition_or.B(equal_and_greater_equal);
    loop_condition_or.OUT(loop_condition);

    // One iteration of the bonus arithmetic.
    t0_sub.A(t0);
    t0_sub.B(c_low_shifted);
    t0_sub.OUT(t0_next);

    borrow_extend.IN(borrow);
    borrow_extend.OUT(borrow_digit);

    t1_borrow_sub.A(t1);
    t1_borrow_sub.B(borrow_digit);
    t1_borrow_sub.OUT(t1_after_borrow);

    t1_c_high_sub.A(t1_after_borrow);
    t1_c_high_sub.B(c_high_extended);
    t1_c_high_sub.OUT(t1_next);

    a_high_increment.IN(a_high);
    a_high_increment.OUT(a_high_next);
  }
};

#endif /* end _DH_BONUS_H_ */
