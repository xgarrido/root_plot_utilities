#ifndef MANAGER_H_
#define MANAGER_H_ 1

#include <datatools/logger.h>

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

    void cldialog (int argc_, char ** argv_);

    const parameters & get_parameters () const;

  private:

    parameters _parameters_;

  };

} // end of rpu namespace
#endif
