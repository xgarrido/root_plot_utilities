#include <base_container.h>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TKey.h>

#include <datatools/utils.h>


// namespace {
struct color {
  typedef std::map<std::string, int> lookup_table;
  typedef std::vector<std::string>   lookup_index;
};

// Construct the color lookup table
color::lookup_table construct_lookup_table ()
{
  color::lookup_table a =
    map_list_of
    ("black",  TColor::GetColor ( 76,  76,  76))
    ("red",    TColor::GetColor (255,  76,  76))
    ("blue",   TColor::GetColor ( 76,  76, 255))
    ("yellow", TColor::GetColor (255, 255,   0))
    ("grey",   TColor::GetColor (179, 179, 179))
    ;
  return a;
}


const int rendering_options::get_color (const std::string & color_name_)
{
  static color::lookup_table a;
  if (a.empty ()) a = construct_lookup_table ();

  static color::lookup_index vcolors;
  static color::lookup_index::const_iterator i;
  if (vcolors.empty ())
    {
      vcolors += "black", "red", "blue", "yellow", "grey";
      i = vcolors.begin ();
    }

  std::string cname = "black";
  if (! color_name_.empty ()) cname = color_name_;
  else { cname = *i; ++i;}
  color::lookup_table::const_iterator p = a.find (cname);
  return (p != a.end () ? p->second : a.begin ()->second);
}

rendering_options::rendering_options ()
{
  logx = false;
  logy = false;
  datatools::invalidate (xmin);
  datatools::invalidate (xmax);
  datatools::invalidate (ymin);
  datatools::invalidate (ymin);
  show_ratio = false;
  fill_reference = false;
  return;
}

void base_container::set_logging_priority (datatools::logger::priority priority_)
{
  _logging_priority = priority_;
  return;
}

datatools::logger::priority base_container::get_logging_priority () const
{
  return _logging_priority;
}

base_container::base_container ()
{
  _logging_priority = datatools::logger::PRIO_WARNING;
  return;
}

void histogram_container::grab (const std::vector<std::string> & files_, const std::string & name_)
{
  for (std::vector<std::string>::const_iterator i = files_.begin ();
       i != files_.end (); ++i)
    {
      const std::string & filename = *i;
      TFile* rootfile = new TFile (filename.c_str ());

      if (name_ == "all")
        {
          TIter nextkey(rootfile->GetListOfKeys());
          TKey *key;
          while (key = (TKey*)nextkey()) {

            const std::string a_class_name = key->GetClassName ();
            const std::string a_name       = key->GetName ();
            if (a_class_name.find ("TH1") != std::string::npos)
              {
                _histos1d_[a_name].push_back (dynamic_cast<TH1*>(key->ReadObj ()));
              }
          }
        }
      else
        {
          TH1 * h = (TH1*)rootfile->Get (name_.c_str ());
          _histos1d_[name_].push_back (h);
        }
    }
}

void histogram_container::show (const rendering_options & options_)
{
  if (_histos1d_.empty ())
    {
      DT_LOG_ERROR (get_logging_priority (), "No histograms have been stored!");
      return;
    }

  for (histogram_dict_type::iterator i = _histos1d_.begin ();
       i != _histos1d_.end (); ++i)
    {
      const std::string & name = i->first;

      DT_LOG_DEBUG (get_logging_priority (), "Plotting histogram " << name);

      TCanvas * a_canvas = new TCanvas;
      a_canvas->SetName (name.c_str ());
      TPad * pads[2];

      if (options_.show_ratio)
        {
          a_canvas->SetWindowSize (600, 700);
          pads[0] = new TPad ("top_pad", "top_pad", 0.0, 0.302, 1.0, 1.0);
          pads[1] = new TPad ("bot_pad", "bot_pad", 0.0, 0.0, 1.0, 0.298);
          pads[0]->SetBottomMargin (0);
          pads[1]->SetTopMargin (0);
          pads[0]->Draw ();
          pads[1]->Draw ();
        }
      else
        {
          a_canvas->SetWindowSize (600, 500);
          pads[0] = a_canvas;
        }

      // Pointer to the reference histogram for ratio mode
      TH1 * ref = 0;
      size_t cnt = 0;
      for (std::vector<TH1*>::iterator j = i->second.begin ();
           j != i->second.end (); ++j, ++cnt)
        {
          TH1 * a_histo = *j;

          pads[0]->cd ();
          if (options_.logx) pads[0]->SetLogx ();
          if (options_.logy) pads[0]->SetLogy ();

          const int icolor
            = rendering_options::get_color (options_.colors.empty () || cnt >= options_.colors.size () ?
                                            "" : options_.colors.at (cnt));
          a_histo->SetMarkerColor (icolor);
          a_histo->SetLineColor   (icolor);
          if (j == i->second.begin ())
            {
              a_histo->Draw ();
              if (options_.fill_reference)
                {
                  a_histo->SetFillColor (icolor);
                  a_histo->SetLineColor (rendering_options::get_color ("black"));
                }
              if (datatools::is_valid (options_.xmax))
                a_histo->GetXaxis ()->SetRangeUser (a_histo->GetXaxis ()->GetXmin (),
                                                    options_.xmax);
              if (datatools::is_valid (options_.xmin))
                a_histo->GetXaxis ()->SetRangeUser (options_.xmin,
                                                    a_histo->GetXaxis ()->GetXmax ());
            }
          else
            {
              a_histo->Draw ("EPX0 && same");
              a_histo->SetMarkerStyle (20);
            }
          if (options_.show_ratio)
            {
              pads[1]->cd ();
              TH1 * clone = (TH1*)a_histo->Clone ();
              clone->Sumw2 ();
              clone->SetStats (0);
              clone->Divide (i->second.front ());
              if (j == i->second.begin ())
                {
                  clone->Draw ("AXIS");
                  ref = clone;
                  ref->GetXaxis ()->SetLabelOffset (0.05);
                  ref->GetXaxis ()->SetTitleOffset (2.0);
                  ref->GetXaxis ()->SetTickLength (0.03);
                  ref->GetYaxis ()->SetTitle ("\\Updelta\\;\\text{ratio}");
                  TF1* f = new TF1 ("", "1", ref->GetXaxis ()->GetXmin (), ref->GetXaxis ()->GetXmax ());
                  f->SetLineStyle (kDashed);
                  f->Draw ("L && same");
                }
              else
                {
                  clone->Draw ("EPX0 && same");
                  ref->GetYaxis ()
                    ->SetRangeUser (0.9*std::min (ref->GetMinimum (), clone->GetMinimum ()),
                                    1.1*std::max (ref->GetMaximum (), clone->GetMaximum ()));
                }
            }
        }

      const std::string latex_name = name + (options_.show_ratio ? "_with_ratio" : "") + ".tex";
      const std::string tikz_name  = name + (options_.show_ratio ? "_with_ratio" : "") + ".tikz";
      a_canvas->Print (latex_name.c_str ());
      rename (latex_name.c_str (), tikz_name.c_str ());
      a_canvas->Update ();
    }
}
