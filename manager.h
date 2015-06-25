#ifndef RPU_MANAGER_H
#define RPU_MANAGER_H 1

// Third party:
// - Boost:
#include <boost/program_options.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>

namespace rpu {

  class manager {

  public:

    struct parameters {
      datatools::logger::priority log_priority;      /// Logging priority flag
      bool interactive;                              /// ROOT interactive cint
      bool ls;                                       /// ROOT ls
      std::vector<std::string> root_files;           /// ROOT files
      std::vector<std::string> unrecognized_options; /// Unrecognized options
      //
      std::string histogram_name;
      std::string graph_name;
      // Rendering options
      std::string reference_root_file;
      bool show_ratio;
      bool logx;
      bool logy;
      double xmin;
      double xmax;
      double ymin;
      double ymax;
      bool fill_reference;
      std::vector<std::string> colors;
    };

    /// Command line dialog
    void cldialog(int argc_, char ** argv_);

    /// Print usage
    void usage(const boost::program_options::options_description & od_, std::ostream & out_ = std::clog);

    /// Get a non-mutable reference to parameters
    const parameters & get_parameters() const;

  private:

    parameters _parameters_;

  };

} // end of rpu namespace
#endif
