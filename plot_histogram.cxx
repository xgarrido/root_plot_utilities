// -*- mode: c++ ; -*-
// plot_histogram.cxx

#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <TRint.h>
#include <TFile.h>
#include <TStyle.h>

#include <datatools/exception.h>

#include <base_container.h>
#include <manager.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    rpu::manager mgr;
    mgr.cldialog(argc_, argv_);

    const rpu::manager::parameters & params = mgr.get_parameters();
    if (params.ls) {
      for (auto filename : params.root_files) {
        TFile rootfile(filename.c_str());
        rootfile.ls();
      }
      return EXIT_SUCCESS;
    }

    DT_THROW_IF(params.histogram_name.empty(), std::logic_error, "No object name has been set !");
    rpu::base_container * BC = new rpu::histogram_container;
    TRint * a_rint = 0;
    if (params.interactive) a_rint = new TRint("CINT ROOT", 0, 0);
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
