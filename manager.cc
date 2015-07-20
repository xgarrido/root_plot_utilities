// -*- mode: c++ ; -*-
// manager.cc

// Ourselves:
#include <manager.h>

// - Boost:
#include <boost/tokenizer.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

namespace rpu {

  void manager::usage(const boost::program_options::options_description & od_,
                      std::ostream & out_)
  {
    out_ << "Usage:" << std::endl
         << "  rpu [options]" << std::endl
         << "Options" << std::endl
         << od_ << std::endl;
    return;
  }

  void manager::cldialog(int argc_, char ** argv_)
  {
    namespace bpo = boost::program_options;
    bpo::options_description opts;
    opts.add_options()
      ("help,h",
       "print this help message")
      ("logging-priority,g",
       bpo::value<std::string>()
       ->default_value("notice"),
       "set the logging priority threshold")
      ("interactive,I",
       bpo::value<bool>(&_parameters_.interactive)
       ->zero_tokens()
       ->default_value(false),
       "run in interactive mode")
      ("ls",
       bpo::value<bool>(&_parameters_.ls)
       ->zero_tokens()
       ->default_value(false),
       "list ROOT file content")
      ("reference-root-file",
       bpo::value<std::string>(&_parameters_.reference_root_file),
       "set the ROOT reference file (from which the comparison will be done)")
      ("root-files,i",
       bpo::value<std::vector<std::string> >(&_parameters_.root_files),
       "set input ROOT file(s)")
      ("save-directory",
       bpo::value<std::string>(&_parameters_.save_directory)
       ->default_value("/tmp"),
       "set the storage directory")
      ("histogram-name",
       bpo::value<std::string>(&_parameters_.histogram_name)
       ->default_value("all"),
       "set the histogram name to be catch from ROOT archive (can be regular expression)")
      // ("graph-name",
      //  bpo::value<std::string>(&_parameters_.graph_name),
      //  "set the graph name to be catch from ROOT archive")
      ("logx",
       bpo::value<bool>(&_parameters_.logx)
       ->zero_tokens()
       ->default_value(false),
       "enable logarithmic X scale")
      ("logy",
       bpo::value<bool>(&_parameters_.logy)
       ->zero_tokens()
       ->default_value(false),
       "enable logarithmic Y scale")
      ("xmin",
       bpo::value<double>(&_parameters_.xmin)
       ->default_value(datatools::invalid_real()),
       "set minimal value for X axis")
      ("xmax",
       bpo::value<double>(&_parameters_.xmax)
      ->default_value(datatools::invalid_real()),
        "set maximal value for X axis")
      ("ymin",
       bpo::value<double>(&_parameters_.ymin)
       ->default_value(datatools::invalid_real()),
       "set minimal value for Y axis")
      ("ymax",
       bpo::value<double>(&_parameters_.ymax)
       ->default_value(datatools::invalid_real()),
       "set maximal value for Y axis")
      ("show-ratio",
       bpo::value<bool>(&_parameters_.show_ratio)
       ->zero_tokens()
       ->default_value (false),
       "show ratio between histograms/graphs")
      ("fill-reference",
       bpo::value<bool>(&_parameters_.fill_reference)
       ->zero_tokens()
       ->default_value(false),
       "fill reference histogram")
      ("colors",
       bpo::value<std::string>(),
       "set color(s)")
      ; // end of options description
    bpo::positional_options_description args;
    args.add("root-files", -1);

    bpo::variables_map vm;
    bpo::parsed_options parsed = bpo::command_line_parser(argc_, argv_)
      .options(opts)
      .positional(args)
      .allow_unregistered()
      .run();
    _parameters_.unrecognized_options
      = bpo::collect_unrecognized(parsed.options, bpo::include_positional);
    bpo::store(parsed, vm);
    bpo::notify(vm);

    // Fetch the opts/args :
    if (vm.count("help")) {
      usage(opts);
      return;
    }

    if (vm.count("logging-priority")) {
      const std::string logging_label = vm["logging-priority"].as<std::string>();
      _parameters_.log_priority = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(_parameters_.log_priority == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    if (vm.count ("colors")) {
      boost::char_separator<char> sep(",");
      boost::tokenizer<boost::char_separator<char> >
        tokens(vm["colors"].as<std::string>(), sep);
      for (auto t: tokens) {
        _parameters_.colors.push_back(t);
      }
    }

    if (!_parameters_.reference_root_file.empty()) {
      DT_LOG_NOTICE (_parameters_.log_priority,
                     "Using '" << _parameters_.reference_root_file << "' file as reference");
      _parameters_.root_files.insert(_parameters_.root_files.begin (),
                                     _parameters_.reference_root_file);
    }

    return;
  }

  const manager::parameters & manager::get_parameters() const
  {
    return _parameters_;
  }

} // end of rpu namespace
