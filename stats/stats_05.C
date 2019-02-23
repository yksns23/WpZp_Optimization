// Macro for creating sample data and storing it in the same file

#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooRealVar.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/HypoTestCalculatorGeneric.h"
#include "RooStats/FrequentistCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestResult.h"

using namespace RooFit;
using namespace RooStats;

// Frequentist p value calculator
void create_data (const char* infile,
                  std::string cutID,         //020457101143203100
                  std::string run,
                  int         nevents,       // # events to generate
                  double_t    signal_xsec,      
                  std::string bkg1_name,    // "background_ttxZ"
                  double_t    bkg1_xsec,
                  std::string bkg2_name = "",
                  double_t    bkg2_xsec = 0,
                  std::string bkg3_name = "",  
                  double_t    bkg3_xsec = 0     ){

  ///// PART 1: Get signal/background PDFs from file /////
  RooArgList pdf;
  RooArgList xsec;

  //----- Open file -----//
  TFile *file = TFile::Open(infile, "UPDATE");  

  //----- Construct signal PDF -----//
  std::string signal_dir = "/" + cutID + "/signal/run_" + run;
  gDirectory->cd((const char *) signal_dir.c_str());

  // Get histogram from file
  TH1F *mwp_signal = dynamic_cast<TH1F*>(gDirectory->Get("mwp_signal"));

  // RooRealVar mass
  Double_t xmin = mwp_signal->GetXaxis()->GetXmin();
  Double_t xmax = mwp_signal->GetXaxis()->GetXmax();
  RooRealVar m("m", "Reconstructed mass of Wp", xmin, xmax, "GeV");

  // Signal PDF
  RooDataHist sHist("sHist", "signal Hist", m, mwp_signal);
  RooHistPdf sPDF("sPDF", "signal PDF", m, sHist);
  RooRealVar sXsec("sXsec", "signal cross section", signal_xsec, "pb-1");

  pdf.add(sPDF);
  xsec.add(sXsec);

  //----- Construct background PDF -----//
  std::string bkg1_dir = "/" + cutID + "/" + bkg1_name + "/run_1";
  std::string bkg1_hist_name = "mwp_" + bkg1_name;
  gDirectory->cd((const char *) bkg1_dir.c_str());
  TH1F *bkg1_hist = dynamic_cast<TH1F*>(gDirectory->Get((const char *) bkg1_hist_name.c_str()));

  // First PDF
  RooDataHist bHist1("bHist1", "background 1 Hist", m, bkg1_hist);
  RooHistPdf bPDF1("bPDF1", "background 1 PDF", m, bHist1);
  RooRealVar bXsec1("bXsec1", "background 1 cross section", bkg1_xsec, "pb-1");

  pdf.add(bPDF1);   // Recall that pdf is RooArgList
  xsec.add(bXsec1);

  if (!bkg2_name.empty()){
    std::string bkg2_dir = "/" + cutID + "/" + bkg2_name + "/run_1";
    std::string bkg2_hist_name = "mwp_" + bkg2_name;
    gDirectory->cd((const char *) bkg2_dir.c_str());
    TH1F *bkg2_hist = dynamic_cast<TH1F*>(gDirectory->Get((const char *) bkg2_hist_name.c_str()));

    // (Optional) Second PDF
    RooDataHist bHist2("bHist2", "bHist2", m, bkg2_hist);
    RooHistPdf bPDF2("bPDF2", "bPDF2", m, bHist2);
    RooRealVar bXsec2("bXsec2", "background 2 cross section", bkg2_xsec, "pb-1");

    pdf.add(bPDF2);
    xsec.add(bXsec2);

    if (!bkg3_name.empty()){
      std::string bkg3_dir = "/" + cutID + "/" + bkg3_name + "/run_1";
      std::string bkg3_hist_name = "mwp_" + bkg3_name;
      gDirectory->cd((const char *) bkg3_dir.c_str());
      TH1F *bkg3_hist = dynamic_cast<TH1F*>(gDirectory->Get((const char *) bkg3_hist_name.c_str()));

      // (Optional) Third PDF
      RooDataHist bHist3("bHist1", "bHist1", m, bkg3_hist);
      RooHistPdf bPDF3("bPDF3", "bPDF3", m, bHist3);
      RooRealVar bXsec3("bXsec3", "background 3 cross section", bkg3_xsec, "pb-1");

      pdf.add(bPDF3);
      xsec.add(bXsec3);
    }
  }

  RooAddPdf sbmodel("sbmodel", "s+b", pdf, xsec);

  //----- Generate data for analysis -----//
  RooDataSet *data = sbmodel.generate(m, nevents);
  data->SetNameTitle("data", "data");

  std::string data_dir = cutID + "/data/run_" + run;
  file->mkdir((const char *) data_dir.c_str());
  gDirectory->cd((const char *) ("/"+data_dir).c_str());
  data->Write();
  file->Close();
}
