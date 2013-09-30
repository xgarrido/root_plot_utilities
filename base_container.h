#ifndef BASE_CONTAINER_H_
#define BASE_CONTAINER_H_

#include <vector>
#include <string>
#include <map>

#include <datatools/logger.h>

struct rendering_options {
  rendering_options ();

  static const int get_color (const std::string & color_name_);
  std::vector<std::string> colors;

  bool show_ratio;
  bool logx;
  bool logy;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  bool fill_reference;
};

class base_container {

public:

  base_container ();

  void set_logging_priority (const datatools::logger::priority priority_);

  datatools::logger::priority get_logging_priority () const;

  virtual void grab (const std::vector<std::string> & files_, const std::string & name_) = 0;

  virtual void show (const rendering_options & options_) = 0;

protected:

  datatools::logger::priority _logging_priority;
};

class TH1;

class histogram_container : public base_container {

public:

  typedef std::map<std::string, std::vector<TH1*> > histogram_dict_type;

  void grab (const std::vector<std::string> & files_, const std::string & name_);

  void show (const rendering_options & options_);

private:
  histogram_dict_type _histos1d_;
};

class graph_container : public base_container {
};
#endif
