// Standard library:
#include <regex>

// Ourselves:
#include <base_container.h>

// Third party:
// - Boost:
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

// - ROOT:
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TKey.h>
#include <TLatex.h>

// - Bayeux/datatools:
#include <bayeux/datatools/utils.h>

// namespace {
struct color {
  typedef std::map<std::string, int> lookup_table;
  typedef std::vector<std::string>   lookup_index;
};

// Construct the color lookup table
color::lookup_table construct_lookup_table()
{
  color::lookup_table a =
    map_list_of
    ("black",  TColor::GetColor( 76,  76,  76))
    ("red",    TColor::GetColor(255,  76,  76))
    ("blue",   TColor::GetColor( 76,  76, 255))
    ("yellow", TColor::GetColor(255, 255,   0))
    ("grey",   TColor::GetColor(179, 179, 179))
    ;
  return a;
}

int get_color(const std::string & color_name_)
{
  static color::lookup_table a;
  if (a.empty()) a = construct_lookup_table();

  static color::lookup_index vcolors;
  static color::lookup_index::const_iterator i;
  if (vcolors.empty()) {
    vcolors += "black", "red", "blue", "yellow", "grey";
    i = vcolors.begin();
  }

  std::string cname;
  if (! color_name_.empty()) {
    cname = color_name_;
  } else {
    cname = *i;
    ++i;
  }

  // Cycle over colors
  if (i == vcolors.end()) i = vcolors.begin();

  color::lookup_table::const_iterator p = a.find(cname);
  return (p != a.end() ? p->second : a.begin()->second);
}

namespace rpu {

  datatools::logger::priority base_container::get_logging_priority() const
  {
    return _params->log_priority;
  }

  bool base_container::is_initialized() const
  {
    return _initialized;
  }

  base_container::base_container()
  {
    _params = 0;
    _initialized = false;
    return;
  }

  void base_container::process()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");

    // Simple ls
    if (_params->ls) {
      for (auto filename : _params->root_files) {
        DT_LOG_NOTICE(get_logging_priority(), "ROOT objects stored in '" << filename << "'");
        TFile rootfile(filename.c_str());
        rootfile.ls();
      }
      return;
    }
    _process();
    return;
  }

  void base_container::_common_initialize(const manager::parameters & params_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _params = &params_;

    gROOT->SetStyle("Plain");

    // Ticks everywhere
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetTickLength(0.01, "xyz");
    gStyle->SetNdivisions(510, "xyz");

    // Stat box
    gStyle->SetOptStat("emrou");
    gStyle->SetOptTitle(0);
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatStyle(0000);
    gStyle->SetStatX(0.85);
    gStyle->SetStatY(0.9);
    gStyle->SetStatH(0.1);
    gStyle->SetStatW(0.2);

    // Turn off all borders
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetCanvasBorderSize(0);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetDrawBorder(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetLegendFont(42);

    // Set fonts
    gStyle->SetLabelFont(42,"xyz");
    gStyle->SetLabelSize(0.04,"xyz");
    gStyle->SetTitleFont(42,"xyz");
    gStyle->SetTitleFont(42);
    gStyle->SetTitleSize(0.05,"xyz");
    gStyle->SetTextFont(42);
    gStyle->SetStatFont(42);
    gStyle->SetStatFontSize(0.05);
    gStyle->SetTitleX(0.3);
    gStyle->SetTitleW(0.4);
    gStyle->SetStatFormat("6.3f");
    gStyle->SetFitFormat("6.3f");
    gStyle->SetTextAlign(22);
    gStyle->SetTextSize(0.05);

    // Set margins
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.12);

    gROOT->ForceStyle();
    return;
  }

  void histogram_container::initialize(const manager::parameters & params_)
  {
    base_container::_common_initialize(params_);

    const std::string name = _params->histogram_name;
    for (auto filename : _params->root_files) {
      TFile * rootfile = new TFile(filename.c_str());
      TIter nextkey(rootfile->GetListOfKeys());
      TKey *key;
      while ((key = (TKey*)nextkey())) {
        const std::string a_class_name = key->GetClassName();
        const std::string a_name       = key->GetName();
        if (a_class_name.find("TH1") != std::string::npos) {
          if (name != "all" && ! std::regex_match(a_name, std::regex(name))) {
            DT_LOG_DEBUG(get_logging_priority(), "Do not add '" << a_name << "' histogram");
            continue;
          }
          DT_LOG_DEBUG(get_logging_priority(), "Adding '" << a_name << "' histogram");
          _histos1d_[a_name].push_back(dynamic_cast<TH1*>(key->ReadObj()));
        }
      }
    }

    if (_histos1d_.empty()) {
      DT_LOG_ERROR(get_logging_priority(), "No histograms have been stored!");
      return;
    }

    _initialized = true;
    return;
  }

  void histogram_container::_process()
  {
    for (auto ientry : _histos1d_) {
      const std::string & name = ientry.first;
      DT_LOG_DEBUG(get_logging_priority(), "Plotting histogram " << name);

      TCanvas * a_canvas = 0;
      TPad * pads[2] = {0, 0};

      if (_params->show_ratio) {
        a_canvas = new TCanvas(name.c_str(), name.c_str(), 600, 700);
        pads[0] = new TPad("top_pad", "top_pad", 0.03, 1.0, 0.97, 0.35);
        pads[0]->SetBottomMargin(0.02);
        pads[0]->Draw();
        pads[1] = new TPad("bot_pad", "bot_pad", 0.03, 0.33, 0.97, 0.0);
        pads[1]->SetTopMargin(0.0);
        pads[1]->SetBottomMargin(0.2);
        pads[1]->Draw();
       } else {
        a_canvas = new TCanvas(name.c_str(), name.c_str(), 600, 500);
        pads[0] = a_canvas;
      }

      // Pointer to the reference histogram for ratio mode
      TH1 * ref = 0;
      size_t cnt = 0;
      std::vector<TH1*> vhistos = ientry.second;
      for (auto & ihisto : vhistos) {
        TH1 * a_histo = ihisto;

        pads[0]->cd();
        if (_params->logx) pads[0]->SetLogx();
        if (_params->logy) pads[0]->SetLogy();

        const int icolor
          = get_color(_params->colors.empty() || cnt >= _params->colors.size() ?
                      "" : _params->colors.at(cnt++));
        a_histo->SetMarkerColor(icolor);
        a_histo->SetLineColor(icolor);
        if (&ihisto == &vhistos.front()) {
          a_histo->Draw();
          if (_params->fill_reference) {
            a_histo->SetFillColor(icolor);
            a_histo->SetLineColor(get_color("black"));
          }
          if (datatools::is_valid(_params->xmin)) {
            a_histo->GetXaxis()->SetRangeUser(_params->xmin,
                                              a_histo->GetXaxis()->GetXmax());
          }
          if (datatools::is_valid(_params->xmax)) {
            a_histo->GetXaxis()->SetRangeUser(a_histo->GetXaxis()->GetXmin(),
                                              _params->xmax);
          }
        } else {
          a_histo->Draw("EPX0 && same");
          a_histo->SetMarkerStyle(20);
        }

        if (_params->show_ratio) {
          pads[1]->cd();
          TH1 * clone = (TH1*)a_histo->Clone();
          clone->Sumw2();
          clone->SetStats(0);
          clone->Divide(vhistos.front());
          if (&ihisto == &vhistos.front()) {
            clone->Draw("AXIS");
            // Store pointer to reference histogram
            ref = clone;
            // Scale label/text size
            a_histo->GetXaxis()->SetLabelSize(0);
            a_histo->GetXaxis()->SetTitleSize(0);
            // Rescale label size
            const double AbsHDNC0 = pads[0]->GetAbsHNDC();
            const double AbsHDNC1 = pads[1]->GetAbsHNDC();
            const double ratio_pad = AbsHDNC0/AbsHDNC1;
            const double label = a_histo->GetYaxis()->GetLabelSize();
            const double title = a_histo->GetYaxis()->GetTitleSize();
            ref->GetYaxis()->SetLabelSize(label*ratio_pad);
            ref->GetYaxis()->SetTitleSize(title*ratio_pad);
            ref->GetXaxis()->SetLabelSize(label*ratio_pad);
            ref->GetXaxis()->SetTitleSize(title*ratio_pad);
            ref->GetYaxis()->SetTitleOffset(1.0/ratio_pad);
            ref->GetYaxis()->SetTitle("\\Updelta\\;\\text{ratio}");
            TF1* f = new TF1("", "1", ref->GetXaxis()->GetXmin(), ref->GetXaxis()->GetXmax());
            f->SetLineStyle(kDashed);
            f->SetLineWidth(1);
            f->SetLineColor(get_color("grey"));
            f->Draw("L && same");
          } else {
            clone->Draw("EPX0 && same");
            ref->GetYaxis()
              ->SetRangeUser(0.9*std::min(ref->GetMinimum(), clone->GetMinimum()),
                             1.1*std::max(ref->GetMaximum(), clone->GetMaximum()));
            // Get chi2 value
            const double chi2 = ref->Chi2Test(clone, "CHI2/NDF");
            TLatex * legend = new TLatex;
            legend->SetNDC();
            legend->SetTextAlign(31);
            legend->SetTextSize(ref->GetYaxis()->GetTitleSize());
            legend->SetTextFont(42);
            legend->SetTextColor(icolor);
            std::ostringstream oss;
            oss.precision(2);
            oss << "\\chi^{2}/\\text{ndf} = " << chi2;
            legend->SetText(0.85, 0.9*(1-0.1*cnt), oss.str().c_str());
            legend->Draw();
          }
        }
      }

      a_canvas->Update();
      const std::string latex_name = name + (_params->show_ratio ? "_with_ratio" : "") + ".tex";
      a_canvas->Print(latex_name.c_str());
    }
    return;
  }
} // end of rpu namespace
