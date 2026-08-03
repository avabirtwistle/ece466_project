#ifndef _DH_COMPONENTS_H_
#define _DH_COMPONENTS_H_ 1

#include "systemc.h"
#include "digit.h"

// 32-bit register with synchronous load.
SC_MODULE(reg32)
{
  sc_in<bool> load;
  sc_in_clk clock;
  sc_in<NN_DIGIT> IN;
  sc_out<NN_DIGIT> OUT;

  void reg_process();

  SC_CTOR(reg32)
  {
    SC_CTHREAD(reg_process, clock.pos());
  }
};

// 16-bit register with synchronous load.
SC_MODULE(reg16)
{
  sc_in<bool> load;
  sc_in_clk clock;
  sc_in<NN_HALF_DIGIT> IN;
  sc_out<NN_HALF_DIGIT> OUT;

  void reg_process();

  SC_CTOR(reg16)
  {
    SC_CTHREAD(reg_process, clock.pos());
  }
};

// 32-bit 2:1 multiplexer. sel=false selects A; sel=true selects B.
SC_MODULE(mux2_32)
{
  sc_in<bool> sel;
  sc_in<NN_DIGIT> A, B;
  sc_out<NN_DIGIT> OUT;

  void mux_process();

  SC_CTOR(mux2_32)
  {
    SC_METHOD(mux_process);
    sensitive << sel << A << B;
  }
};

// Combinational 16-bit by 16-bit multiplier with a 32-bit result.
SC_MODULE(multiplier)
{
  sc_in<NN_HALF_DIGIT> A, B;
  sc_out<NN_DIGIT> OUT;

  void multiplier_process();

  SC_CTOR(multiplier)
  {
    SC_METHOD(multiplier_process);
    sensitive << A << B;
  }
};

// Combinational 32-bit subtractor: OUT = A - B.
SC_MODULE(sub32)
{
  sc_in<NN_DIGIT> A, B;
  sc_out<NN_DIGIT> OUT;

  void sub_process();

  SC_CTOR(sub32)
  {
    SC_METHOD(sub_process);
    sensitive << A << B;
  }
};

// Unsigned comparator used to detect subtraction borrow.
SC_MODULE(less_than32)
{
  sc_in<NN_DIGIT> A, B;
  sc_out<bool> LT;

  void comp_process();

  SC_CTOR(less_than32)
  {
    SC_METHOD(comp_process);
    sensitive << A << B;
  }
};

// Split a 32-bit value into its low and high 16-bit halves.
SC_MODULE(split32)
{
  sc_in<NN_DIGIT> IN;
  sc_out<NN_HALF_DIGIT> LOW, HIGH;

  void split_process();

  SC_CTOR(split32)
  {
    SC_METHOD(split_process);
    sensitive << IN;
  }
};

// Place a 16-bit value in the high half of a 32-bit word.
SC_MODULE(to_high_half)
{
  sc_in<NN_HALF_DIGIT> IN;
  sc_out<NN_DIGIT> OUT;

  void shift_process();

  SC_CTOR(to_high_half)
  {
    SC_METHOD(shift_process);
    sensitive << IN;
  }
};

// Zero-extend a 16-bit value to 32 bits.
SC_MODULE(extend16)
{
  sc_in<NN_HALF_DIGIT> IN;
  sc_out<NN_DIGIT> OUT;

  void extend_process();

  SC_CTOR(extend16)
  {
    SC_METHOD(extend_process);
    sensitive << IN;
  }
};

// Convert the one-bit borrow flag to a 32-bit subtraction operand.
SC_MODULE(bool_to_digit)
{
  sc_in<bool> IN;
  sc_out<NN_DIGIT> OUT;

  void convert_process();

  SC_CTOR(bool_to_digit)
  {
    SC_METHOD(convert_process);
    sensitive << IN;
  }
};

// Combinational OR for register load controls.
SC_MODULE(or2)
{
  sc_in<bool> A, B;
  sc_out<bool> OUT;

  void or_process();

  SC_CTOR(or2)
  {
    SC_METHOD(or_process);
    sensitive << A << B;
  }
};

// Output buffers keep the datapath itself purely structural.
SC_MODULE(buffer32)
{
  sc_in<NN_DIGIT> IN;
  sc_out<NN_DIGIT> OUT;

  void buffer_process();

  SC_CTOR(buffer32)
  {
    SC_METHOD(buffer_process);
    sensitive << IN;
  }
};

SC_MODULE(buffer16)
{
  sc_in<NN_HALF_DIGIT> IN;
  sc_out<NN_HALF_DIGIT> OUT;

  void buffer_process();

  SC_CTOR(buffer16)
  {
    SC_METHOD(buffer_process);
    sensitive << IN;
  }
};

#endif /* end _DH_COMPONENTS_H_ */
