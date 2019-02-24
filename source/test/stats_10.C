// Normalize histograms to unity
// to represent PDF
#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"

using namespace std;

void norm_background (   const char* infile,
                              string  cut_id,
                              string  bchannel1="background_tbZ",
                            double_t  bxsec1=0.001167,
                              string  bchannel2="background_ttxZ_4j2b2vl",
                            double_t  bxsec2=0.04719,
                              string  bchannel3=
                                      "background_ttxZ_lvl2j2b2vl",
                            double_t  bxsec3=0.0316  ){

  //----- Get file in update mode -----//
  TFile *file = TFile::Open(infile, "UPDATE");
  if(!file){
    cout << "File not found\nBye" << endl;
    return;
  }
  
  // Get background historgrams
  string dir1 = "/" + cut_id + "/" + bchannel1 + "/run_1";
  string bhist1 = "mwp_" + bchannel1;

  string dir2 = "/" + cut_id + "/" + bchannel2 + "/run_1";
  string bhist2 = "mwp_" + bchannel2;

  string dir3 = "/" + cut_id + "/" + bchannel3 + "/run_1";
  string bhist3 = "mwp_" + bchannel3;

  gDirectory->cd((const char*) dir1.c_str());
  TH1 *bg1 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist1.c_str()));
  if (bg1->GetSumw2N() == 0) bg1->Sumw2(kTRUE);

  gDirectory->cd((const char*) dir2.c_str());
  TH1 *bg2 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist2.c_str()));
  if (bg2->GetSumw2N() == 0) bg2->Sumw2(kTRUE);

  gDirectory->cd((const char*) dir3.c_str());
  TH1 *bg3 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist3.c_str()));
  if (bg3->GetSumw2N() == 0) bg3->Sumw2(kTRUE);

  // Combine histograms together
  int nbins = bg1->GetNbinsX();
  double_t low = bg1->GetXaxis()->GetBinLowEdge(1);
  double_t high = bg1->GetXaxis()->GetBinUpEdge(nbins);
  TH1 *bcombined = new TH1D("bcombined", "bcombined", nbins, low, high);

  bcombined->Add(bg1, bxsec1*1000);
  bcombined->Add(bg2, bxsec2*1000);
  bcombined->Add(bg3, bxsec3*1000);

  bcombined->Scale(1/(bcombined->Integral()));

  bcombined->SetNameTitle("bcombined", "bcombined");

  // Get Visual
  bcombined->Draw();

  // Write to file
  gDirectory->cd("/");
  gDirectory->pwd();
  bcombined->Write("bcombined", TObject::kOverwrite);
  file->Close();
}

// As for signal, we only need to normalize by eff
// eff = Nevents/NGenEvents
// count = xsec(poi) * Lumi * eff
//       = xsec(poi) * Lumi * bincontent/NGenEvents
// So, here we only need to scale by 1/NGenEvents

void normalize_signal   ( const char* infile,
                              string  cut_id,
                                 int  run,
                              string  schannel="signal"){

  //----- Get file in update mode -----//
  TFile *file = TFile::Open(infile, "UPDATE");
  if(!file){
    cout << "File not found\nBye" << endl;
    return;
  }

  // Get historgram
  string dir = "/" + cut_id + "/" + schannel + "/run_" + to_string(run);
  string shist = "mwp_" + schannel;

  gDirectory->cd((const char*) dir.c_str());
  TH1 *sig = dynamic_cast<TH1*>(gDirectory->Get((const char*)shist.c_str()));
  if (sig->GetSumw2N() == 0) sig->Sumw2(kTRUE);

  /*int nbins = sig->GetNbinsX();
  double_t low = sig->GetXaxis()->GetBinLowEdge(1);
  double_t high = sig->GetXaxis()->GetBinUpEdge(nbins);
  TH1 *snormed = new TH1D("snormed", "snormed", nbins, low, high);

  snormed->Add(sig, 1/NGenEvents);*/

  TH1 *snormed = (TH1*) sig->Clone();
  snormed->Scale(1/(snormed->Integral())); // Scale
  snormed->Draw();

  // Save
  snormed->SetNameTitle("snormed", "snormed");
  snormed->Write("snormed",TObject::kOverwrite);
  file->Close();
}
