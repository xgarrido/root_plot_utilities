// -*- mode: c++ ; -*-
// plot_histogram.cxx

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdio>

#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include <TRint.h>
#include <TFile.h>
#include <TStyle.h>

#include <datatools/exception.h>

#include <base_container.h>

struct params
{
  datatools::logger::priority log_priority; /// Logging priority flag
  bool interactive;                              /// ROOT interactive cint
  std::string reference_root_file;
  std::vector<std::string> root_files;           /// ROOT files
  std::vector<std::string> unrecognized_options; /// Unrecognized options

  // TFile options
  bool ls;

  std::string histogram_name;
  std::string graph_name;
};

void usage (const boost::program_options::options_description & options_,
            std::ostream & out_)
{
  return;
}

void cldialog (int argc_, char ** argv_, params & parameters_)
{
  namespace bpo = boost::program_options;
  bpo::options_description opts;
  opts.add_options ()
    ("logging-priority,g",
     boost::program_options::value<std::string>()
     ->default_value("notice"),
     "set the logging priority threshold")
    ("interactive,I",
     boost::program_options::value<bool>(&parameters_.interactive)
     ->zero_tokens ()
     ->default_value (true),
     "run in interactive mode")
    ("ls",
     boost::program_options::value<bool>(&parameters_.ls)
     ->zero_tokens ()
     ->default_value (false),
     "list ROOT file content")
    ("reference-root-file",
     boost::program_options::value<std::string>(&parameters_.reference_root_file),
     "set the ROOT reference file (from which the comparison will be done)")
    ("root-files,i",
     boost::program_options::value<std::vector<std::string> >(&parameters_.root_files),
     "set input ROOT file(s)")
    ("histogram-name",
     boost::program_options::value<std::string>(&parameters_.histogram_name),
     "set the histogram name to be catch from ROOT archive")
    ("graph-name",
     boost::program_options::value<std::string>(&parameters_.graph_name),
     "set the graph name to be catch from ROOT archive")
    ; // end of options description
  bpo::positional_options_description args;
  args.add ("root-files", -1);

  bpo::variables_map vm;
  bpo::parsed_options parsed =
    bpo::command_line_parser (argc_, argv_)
    .options (opts)
    .positional (args)
    .allow_unregistered ()
    .run ();
  parameters_.unrecognized_options
    = bpo::collect_unrecognized (parsed.options, bpo::include_positional);
  bpo::store (parsed, vm);
  bpo::notify (vm);

  // Fetch the opts/args :
  if (vm.count ("help")) {
    usage(opts, std::cout);
    return;
  }

  if (vm.count("logging-priority"))
    {
      const std::string logging_label = vm["logging-priority"].as<std::string>();
      parameters_.log_priority = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(parameters_.log_priority == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  params parameters;
  rendering_options options;


  try
    {
      cldialog (argc_, argv_, parameters);
      // ("logx",
      //  boost::program_options::value<bool>(&options.logx)
      //  ->zero_tokens ()
      //  ->default_value (false),
      //  "enable logarithmic X scale"
      //  )

      //   ("logy",
      //    boost::program_options::value<bool>(&options.logy)
      //    ->zero_tokens ()
      //    ->default_value (false),
      //    "enable logarithmic Y scale"
      //    )

      //   ("xmin",
      //    boost::program_options::value<double>(&options.xmin),
      //    "set minimal value for X axis"
      //    )

      //   ("xmax",
      //    boost::program_options::value<double>(&options.xmax),
      //    "set maximal value for X axis"
      //    )

      //   ("ymin",
      //    boost::program_options::value<double>(&options.ymin),
      //    "set minimal value for Y axis"
      //    )

      //   ("ymax",
      //    boost::program_options::value<double>(&options.ymax),
      //    "set maximal value for Y axis"
      //    )

      //   ("show-ratio",
      //    boost::program_options::value<bool>(&options.show_ratio)
      //    ->zero_tokens ()
      //    ->default_value (false),
      //    "show ratio between histograms/graphs"
      //    )

      //   ("fill-reference",
      //    boost::program_options::value<bool>(&options.fill_reference)
      //    ->zero_tokens ()
      //    ->default_value (false),
      //    "fill reference histogram"
      //    )

      //   ("colors",
      //    boost::program_options::value<std::string>(),
      //    "set color(s)"
      //    )




      // if (vm.count ("colors"))
      //   {
      //     boost::char_separator<char> sep(",");
      //     boost::tokenizer<boost::char_separator<char> > tokens(vm["colors"].as<std::string>(),
      //                                                            sep);
      //     BOOST_FOREACH (const std::string& t, tokens) {
      //       options.colors.push_back (t);
      //     }
      //   }

      if (!parameters.reference_root_file.empty ())
        {
          DT_LOG_NOTICE (parameters.log_priority,
                         "Using '" << parameters.reference_root_file << "' file as reference");
          parameters.root_files.insert (parameters.root_files.begin (),
                                        parameters.reference_root_file);
        }

      if (parameters.ls)
        {
          for (std::vector<std::string>::const_iterator
                 i = parameters.root_files.begin ();
               i != parameters.root_files.end (); ++i)
            {
              const std::string & filename = *i;
              TFile * rootfile = new TFile (filename.c_str ());
              rootfile->ls ();
            }
          return EXIT_SUCCESS;
        }

      base_container * BC = 0;
      if (! parameters.histogram_name.empty ())
        {
          BC = new histogram_container;
        }
      else
        {
          DT_THROW_IF (true, std::logic_error, "No object name has been set !");
        }
      TRint * a_rint = 0;
      if (parameters.interactive) a_rint = new TRint ("CINT ROOT", 0, 0);
      BC->grab (parameters.root_files, parameters.histogram_name);
      BC->show (options);
      if (parameters.interactive) a_rint->Run ();
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
