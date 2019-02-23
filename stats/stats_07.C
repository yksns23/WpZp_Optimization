// Open ROOT file containing background histograms and combine them according to their cross sections. This is to eliminate zero bins.
// Downside: unnecessary error propagation because of bxsec * bxsec'/bxsec

#include "TFile.h"
#include "TH1.h"
#include "TH1F.h"

using namespace std;

void combine_background ( const char* infile,
                              string  cut_id,
                              string  bchannel1="background_tbZ",
                            double_t  bxsec1=0.001167,
                              string  bchannel2="background_ttxZ_4j2b2vl",
                            double_t  bxsec2=0.04719,
                              string  bchannel3="background_ttxZ_lvl2j2b2vl",
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

  gDirectory->cd((const char*) dir2.c_str());
  TH1 *bg2 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist2.c_str()));

  gDirectory->cd((const char*) dir3.c_str());
  TH1 *bg3 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist3.c_str()));

  // Combine histograms together
  TH1 *bcombined = (TH1*) bg1->Clone();
  bcombined->Add(bg2, bxsec2/bxsec1);
  bcombined->Add(bg3, bxsec3/bxsec1);

  // Scale histogram by xsec(pb-1)
  double_t scale_factor = bxsec1;
  bcombined->Scale(scale_factor);

  bcombined->SetNameTitle("bcombined", "bcombined");

  // Draw Histogram
  bcombined->Draw();

  // Write to file
  gDirectory->cd("/");
  gDirectory->pwd();
  bcombined->Write("bcombined", TObject::kOverwrite);
}
