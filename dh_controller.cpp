#include "systemc.h"
#include "dh_controller.h"

void dh_controller::state_reg()
{
  while (true) {
    if (!hw_enable.read())
      state.write(DH_IDLE);
    else
      state.write(next_state.read());

    wait();
  }
}

void dh_controller::state_diagram()
{
  // Moore-FSM defaults
  load_inputs.write(false);
  load_result.write(false);
  bonus_ready.write(false);
  load_output.write(false);
  hw_done.write(false);

  next_state.write(state.read());

  switch (state.read()) {
    case DH_IDLE:
      if (hw_enable.read())
        next_state.write(DH_LOAD);
      break;

    case DH_LOAD:
      // Capture inputs at the next rising edge.
      load_inputs.write(true);
      next_state.write(DH_CALCULATE);
      break;

    case DH_CALCULATE:
      // Capture required-part results at the next rising edge.
      load_result.write(true);
      next_state.write(DH_BONUS);
      break;

    case DH_BONUS:
      bonus_ready.write(true);
      next_state.write(DH_OUTPUT);
      break;

    case DH_OUTPUT:
      //keep bonus valid while capturing
      bonus_ready.write(true);
      load_output.write(true);
      next_state.write(DH_DONE);
      break;

    case DH_DONE:
      // Output registers are valid and may be read by software.
      hw_done.write(true);
      break;

    default:
      next_state.write(DH_IDLE);
      break;
  }
}