#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"

SC_MODULE (dh_hw)
{
  sc_fifo_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
  sc_fifo_in <NN_HALF_DIGIT> from_sw3;

  sc_fifo_out <NN_DIGIT> to_sw0, to_sw1;
  sc_fifo_out <NN_HALF_DIGIT> to_sw2;
  
  sc_in <bool> hw_enable; 
  sc_out <bool> hw_done;

  void process_hw();
  
  SC_CTOR (dh_hw)
  {
    SC_THREAD (process_hw);
  }
  
};

#endif /* end _DH_HW_H_ */
