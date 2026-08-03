#include "systemc.h"
#include "dh_controller.h"

#include <iostream>
#include <sstream>
#include <string>

SC_MODULE(controller_tb)
{
sc_in_clk clock;

sc_out<bool> hw_enable;
sc_in<bool> load_inputs;
sc_in<bool> load_result;
sc_in<bool> bonus_ready;
sc_in<bool> load_output;
sc_in<bool> hw_done;
unsigned int failures = 0;

void expect_outputs(
    const char* phase,
    bool expected_load_inputs,
    bool expected_load_result,
    bool expected_bonus_ready,
    bool expected_load_output,
    bool expected_hw_done)
  {
    bool passed =
        load_inputs.read() == expected_load_inputs &&
        load_result.read() == expected_load_result &&
        bonus_ready.read() == expected_bonus_ready &&
        load_output.read() == expected_load_output &&
        hw_done.read() == expected_hw_done;

    if (passed) {
      std::cout << sc_time_stamp()
                << " PASS: " << phase << '\n';
      return;
    }

    ++failures;

    std::ostringstream message;
    message << phase
            << " expected {load_inputs=" << expected_load_inputs
            << ", load_result=" << expected_load_result
            << ", bonus_ready=" << expected_bonus_ready
            << ", load_output=" << expected_load_output
            << ", hw_done=" << expected_hw_done
            << "} but received {load_inputs=" << load_inputs.read()
            << ", load_result=" << load_result.read()
            << ", bonus_ready=" << bonus_ready.read()
            << ", load_output=" << load_output.read()
            << ", hw_done=" << hw_done.read()
            << "}";

    const std::string text = message.str();
    SC_REPORT_ERROR("controller_tb", text.c_str());
  }

void stimulus()
{
    /*
    * The testbench runs on falling clock edges.
    * The controller changes state on rising edges, so its
    * Moore outputs are stable when they are checked here.
    */

    // Initial state
    expect_outputs(
        "IDLE",
        false, false, false, false, false);

    // Start first transaction
    hw_enable.write(true);

    wait();
    expect_outputs(
        "LOAD",
        true, false, false, false, false);

    wait();
    expect_outputs(
        "CALCULATE",
        false, true, false, false, false);

    wait();
    expect_outputs(
        "BONUS",
        false, false, true, false, false);

    wait();
    expect_outputs(
        "OUTPUT",
        false, false, true, true, false);

    wait();
    expect_outputs(
        "DONE",
        false, false, false, false, true);

    // DONE must remain asserted while hw_enable remains high.
    wait();
    expect_outputs(
        "DONE held",
        false, false, false, false, true);

    // Software acknowledges completion.
    hw_enable.write(false);

    wait();
    expect_outputs(
        "return to IDLE",
        false, false, false, false, false);

    // Start second transaction.
    hw_enable.write(true);

    wait();
    expect_outputs(
        "second LOAD",
        true, false, false, false, false);

    wait();
    expect_outputs(
        "second CALCULATE",
        false, true, false, false, false);

    wait();
    expect_outputs(
        "second BONUS",
        false, false, true, false, false);

    wait();
    expect_outputs(
        "second OUTPUT",
        false, false, true, true, false);

    wait();
    expect_outputs(
        "second DONE",
        false, false, false, false, true);

    // Acknowledge the second transaction.
    hw_enable.write(false);

    wait();
    expect_outputs(
        "final IDLE",
        false, false, false, false, false);

    // Check the final state.
    if (failures == 0) {
    std::cout << "\nAll controller tests passed.\n";
    } else {
    std::cout << "\nController test failed with "
                << failures << " error(s).\n";
    }

    sc_stop();
}

  SC_CTOR(controller_tb)
  {
    // Check and drive signals halfway between controller clock edges.
    SC_CTHREAD(stimulus, clock.neg());
  }
};

int sc_main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    sc_clock clock("clock", 10, SC_NS);

    sc_signal<bool> hw_enable("hw_enable");
    sc_signal<bool> load_inputs("load_inputs");
    sc_signal<bool> load_result("load_result");
    sc_signal<bool> bonus_ready("bonus_ready");
    sc_signal<bool> load_output("load_output");
    sc_signal<bool> hw_done("hw_done");

    dh_controller dut("DUT");

    dut.clock(clock);
    dut.hw_enable(hw_enable);
    dut.load_inputs(load_inputs);
    dut.load_result(load_result);
    dut.bonus_ready(bonus_ready);
    dut.load_output(load_output);
    dut.hw_done(hw_done);

    controller_tb tb("TB");

    tb.clock(clock);
    tb.hw_enable(hw_enable);
    tb.load_inputs(load_inputs);
    tb.load_result(load_result);
    tb.bonus_ready(bonus_ready);
    tb.load_output(load_output);
    tb.hw_done(hw_done);

    //waveform
    sc_trace_file* trace = sc_create_vcd_trace_file("controller_test");

    sc_trace(trace, clock, "clock");
    sc_trace(trace, hw_enable, "hw_enable");
    sc_trace(trace, load_inputs, "load_inputs");
    sc_trace(trace, load_result, "load_result");
    sc_trace(trace, bonus_ready, "bonus_ready");
    sc_trace(trace, load_output, "load_output");
    sc_trace(trace, hw_done, "hw_done");

    sc_start();

    sc_close_vcd_trace_file(trace);

    return tb.failures == 0 ? 0 : 1;
}