#include "systemc.h"
#include "dh_components.h"

void reg32::reg_process()
{
  while (1) {
    if (load.read()) OUT.write(IN.read());
    wait();
  }
}

void reg16::reg_process()
{
  while (1) {
    if (load.read()) OUT.write(IN.read());
    wait();
  }
}

void mux2_32::mux_process()
{
  OUT.write(sel.read() ? B.read() : A.read());
}

void multiplier::multiplier_process()
{
  OUT.write((NN_DIGIT)A.read() * (NN_DIGIT)B.read());
}

void sub32::sub_process()
{
  OUT.write(A.read() - B.read());
}

void less_than32::comp_process()
{
  LT.write(A.read() < B.read());
}

void split32::split_process()
{
  LOW.write((NN_HALF_DIGIT)LOW_HALF(IN.read()));
  HIGH.write((NN_HALF_DIGIT)HIGH_HALF(IN.read()));
}

void to_high_half::shift_process()
{
  OUT.write(TO_HIGH_HALF(IN.read()));
}

void extend16::extend_process()
{
  OUT.write((NN_DIGIT)IN.read());
}

void bool_to_digit::convert_process()
{
  OUT.write(IN.read() ? 1u : 0u);
}

void or2::or_process()
{
  OUT.write(A.read() || B.read());
}

void buffer32::buffer_process()
{
  OUT.write(IN.read());
}

void buffer16::buffer_process()
{
  OUT.write(IN.read());
}
