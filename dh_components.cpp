#include "systemc.h"
#include "dh_components.h"

/*
 * 32-bit working register with synchronous load
 *
 * On a rising clock edge:
 *   load = true  -> OUT captures IN
 *   load = false -> OUT retains its previous value
 *
 */
void reg32::reg_process()
{
  while (1) {
    if (load.read()) OUT.write(IN.read());
    wait();
  }
}

/*
 * 16-bit working register with synchronous load
 *
 * On a rising clock edge:
 *   load = true  -> OUT captures IN
 *   load = false -> OUT retains its previous value
 *
 */
void reg16::reg_process()
{
  while (1) {
    if (load.read()) OUT.write(IN.read());
    wait();
  }
}

/*
 * Combinational 32-bit 2:1 multiplexer
 *
 * sel = false -> OUT = A, used when loading software inputs
 * sel = true  -> OUT = B, used when loading calculated results
 */
void mux2_32::mux_process()
{
  OUT.write(sel.read() ? B.read() : A.read());
}

/*
 * Unsigned 16-by-16-bit multiplier
 * The full 32-bit product is written to OUT
 */
void multiplier::multiplier_process()
{
  OUT.write((NN_DIGIT)A.read() * (NN_DIGIT)B.read());
}

/*
 * Unsigned 32-bit subtractor.
 * OUT = A - B modulo 2^32.
 *
 * Borrow detection is performed separately by less_than32
 */
void sub32::sub_process()
{
  OUT.write(A.read() - B.read());
}

/*
 * Unsigned comparator used for subtraction borrow detection
 * LT is true when A < B, meaning A - B requires a borrow
 */
void less_than32::comp_process()
{
  LT.write(A.read() < B.read());
}

/*
 * Splits a 32-bit word into two 16-bit halves:
 *   LOW  = IN[15:0]
 *   HIGH = IN[31:16]
 */
void split32::split_process()
{
  LOW.write((NN_HALF_DIGIT)LOW_HALF(IN.read()));
  HIGH.write((NN_HALF_DIGIT)HIGH_HALF(IN.read()));
}

/*
 * Places a 16-bit value in the upper half of a 32-bit word
 * OUT = IN << 16
 */
void to_high_half::shift_process()
{
  OUT.write(TO_HIGH_HALF(IN.read()));
}

/*
 * Zero-extends a 16-bit unsigned value to 32 bits.
 */
void extend16::extend_process()
{
  OUT.write((NN_DIGIT)IN.read());
}

/*
 * Converts the one-bit borrow flag into a 32-bit subtraction operand:
 *   false -> 0
 *   true  -> 1
 */
void bool_to_digit::convert_process()
{
  OUT.write(IN.read() ? 1u : 0u);
}

/*
 * Combines the two load conditions so the t0 and t1 register can be reused 
 * OUT is asserted during either input capture or result capture
 */
void or2::or_process()
{
  OUT.write(A.read() || B.read());
}

/*
 * Combinational 32-bit output connection 
 * connects internal datapath signal to module output port
 */
void buffer32::buffer_process()
{
  OUT.write(IN.read());
}

/*
 * Combinational 16 bit output connection 
 * connects internal datapath signal to module output port
 */
void buffer16::buffer_process()
{
  OUT.write(IN.read());
}
