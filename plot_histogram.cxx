// -*- mode: c++ ; -*-
// plot_histogram.cxx

// Standard libraries:
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdio>

// Third party:
// - ROOT:
#include <TRint.h>

// This project:
#include <base_container.h>
#include <manager.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    rpu::manager mgr;
    mgr.cldialog(argc_, argv_);
    const rpu::manager::parameters & params = mgr.get_parameters();

    TRint * a_rint = 0;
    if (params.interactive) a_rint = new TRint("CINT ROOT", 0, 0);
    rpu::base_container * BC = new rpu::histogram_container;
    BC->initialize(params);
    BC->process();
    if (params.interactive) a_rint->Run();

  } catch (std::exception & x) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what ());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of plot_histogram.cxx
