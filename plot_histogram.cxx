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

// void usage (const boost::program_options::options_description & options_,
//             std::ostream & out_)
// {
//   return;
// }

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  try
    {
      rpu::manager mgr;
      mgr.cldialog (argc_, argv_);

      const rpu::manager::parameters & params = mgr.get_parameters ();
      if (params.ls)
        {
          for (std::vector<std::string>::const_iterator
                 i = params.root_files.begin ();
               i != params.root_files.end (); ++i)
            {
              const std::string & filename = *i;
              TFile * rootfile = new TFile (filename.c_str ());
              rootfile->ls ();
            }
          return EXIT_SUCCESS;
        }

      rpu::base_container * BC = 0;
      if (! params.histogram_name.empty ())
        {
          BC = new rpu::histogram_container;
        }
      else
        {
          DT_THROW_IF (true, std::logic_error, "No object name has been set !");
        }
      TRint * a_rint = 0;
      if (params.interactive) a_rint = new TRint ("CINT ROOT", 0, 0);
      BC->grab (params.root_files, params.histogram_name);
      BC->show (params);
      if (params.interactive) a_rint->Run ();
    }
  catch (std::exception & x)
    {
      DT_LOG_ERROR (datatools::logger::PRIO_ERROR, x.what ());
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      DT_LOG_ERROR (datatools::logger::PRIO_ERROR, "Unexpected error!");
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of plot_histogram.cxx
