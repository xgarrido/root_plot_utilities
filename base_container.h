#ifndef BASE_CONTAINER_H_
#define BASE_CONTAINER_H_ 1

#include <vector>
#include <string>
#include <map>

#include <manager.h>

static const int get_color (const std::string & color_name_);

class base_container {

public:

  base_container ();

  void set_logging_priority (const datatools::logger::priority priority_);

  datatools::logger::priority get_logging_priority () const;

  virtual void grab (const std::vector<std::string> & files_, const std::string & name_) = 0;

  virtual void show (const manager::parameters & options_) = 0;

protected:

  datatools::logger::priority _logging_priority;
};

class TH1;

class histogram_container : public base_container {

public:

  typedef std::map<std::string, std::vector<TH1*> > histogram_dict_type;

  void grab (const std::vector<std::string> & files_, const std::string & name_);

  void show (const manager::parameters & options_);

private:
  histogram_dict_type _histos1d_;
};

class graph_container : public base_container {
};
#endif
