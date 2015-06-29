#ifndef BASE_CONTAINER_H_
#define BASE_CONTAINER_H_ 1

#include <vector>
#include <string>
#include <map>

#include <manager.h>

// Forward declaration
class TH1;

namespace rpu {

  /// \brief Base class for container
  class base_container {

  public:

    /// Constructor
    base_container();

    /// Get logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Return initialization flag
    bool is_initialized() const;

    /// Virtual initialization method
    virtual void initialize(const manager::parameters & options_) = 0;

    /// Virtual process method
    virtual void process();

  protected:

    /// Specialized process method
    virtual void _process() = 0;

    /// Basic initialization shared by all inherited modules
    void _common_initialize(const manager::parameters & options_);

  protected:

    bool _initialized;
    const manager::parameters * _params;
  };

  /// \brief A dedicated class for 1D histogram container
  class histogram_container : public base_container {

  public:

    /// 1D histogram dictionnary typedef
    typedef std::map<std::string, std::vector<TH1*> > histogram_dict_type;

    /// Dedicated initialization method
    virtual void initialize(const manager::parameters & options_);

  protected:

    /// Main process method
    void _process();

  private:

    histogram_dict_type _histos1d_;
  };

  class graph_container : public base_container {
  };

} // end of rpu namespace
#endif
