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

//local_p("data3.root", "020457101143203100", "3", 500, signal xsec, "background_ttxZ", background xsec);

// Frequentist p value calculator
void local_p (  const char* filename,
                std::string cutID,         //02045710114320325
                std::string run,       
                   double_t mass,
                   double_t signal_xsec,      
                std::string bkg1_name,    // "background_ttxZ"
                   double_t bkg1_xsec,
                std::string bkg2_name = "",
                   double_t bkg2_xsec = 0,
                std::string bkg3_name = "",  
                   double_t bkg3_xsec = 0     ){

  ///// PART 1: Get signal/background PDFs from file /////
  RooArgList pdf;
  RooArgList xsec;
  //----- Open file -----//
  TFile *file = TFile::Open(filename);  

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


  ///// PART 2: Construct ModelConfig /////
  RooAddPdf sbmodel("sbmodel", "s+b", pdf, xsec);

  RooWorkspace w("w"); // Everything is imported into the workspace
  w.import(sbmodel);

  w.defineSet("obs", "m");
  w.defineSet("poi", "sXsec");
  //w.defineSet("nuis", uncertainties); will work on this later

  //----- Signal + background Hypothesis -----//
  ModelConfig sbHypo("sbHypo", &w);
  sbHypo.SetPdf(sbmodel);
  sbHypo.SetObservables(*w.set("obs"));
  sbHypo.SetParametersOfInterest(*w.set("poi"));
  //sbHypo.SetNuisanceParameters(*w.set("nuis"));
  w.var("sXsec")->setVal(signal_xsec);
  sbHypo.SetSnapshot(*w.set("poi"));

  ModelConfig bHypo("bHypo", &w);
  bHypo.SetPdf(sbmodel);
  bHypo.SetObservables(*w.set("obs"));
  bHypo.SetParametersOfInterest(*w.set("poi"));
  //bHypo.SetNuisanceParameters(*w.set("nuis"));
  w.var("sXsec")->setVal(0);
  bHypo.SetSnapshot(*w.set("poi"));

  w.import(sbHypo);
  w.import(bHypo);

  ///// PART 3: Calculate local p for given mass /////
  //----- Generate data for analysis -----//
  RooDataSet *data = sbmodel.generate(*w.set("obs"), 100000);
  w.import(*data);
  w.writeToFile("workspace.root");

  //----- Configure Frequentist Method -----//
  // Set Test Statistic
  ProfileLikelihoodTestStat *profll = new ProfileLikelihoodTestStat(*sbHypo.GetPdf());
  profll->SetOneSidedDiscovery(true);
  profll->SetVarName( "q_{0}/2" );

  // Set FrequentistCalculator
  HypoTestCalculatorGeneric *hc = new FrequentistCalculator(*data, sbHypo, bHypo);
  ((FrequentistCalculator*) hc)->SetToys(1000, 1000);

  // Set ToyMCSampler
  ToyMCSampler *sampler = (ToyMCSampler*)hc->GetTestStatSampler();
  int nEvents = data->numEntries();
  sampler->SetNEventsPerToy(nEvents);
  sampler->SetGenerateBinned(true);
  sampler->SetTestStatistic(profll);

  //----- Get Results -----//
  HypoTestResult *result = hc->GetHypoTest();
  result->SetPValueIsRightTail(true);
  result->SetBackgroundAsAlt(false);
  result->Print();
}
