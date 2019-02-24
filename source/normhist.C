// Normalize histograms to unity
// to represent PDF
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <iostream>
#include <string>
#include <cstdlib>

void norm_background (   const char*  infile,
                              string  cut_id,
                              string  bchannel1,
                            double_t  bxsec1,
                              string  bchannel2,
                            double_t  bxsec2,
                              string  bchannel3,
                            double_t  bxsec3    );

void normalize_signal (  const char*  infile,
                              string  cut_id,
                                 int  run,
                              string  schannel );





void norm_background (   const char*  infile,
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
    std::cout << "Info in normhist.C: norm_background --> File not found\nBye" << std::endl;
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


void norm_signal   (  const char*  infile,
                           string  cut_id,
                              int  run,
                           string  schannel="signal"){

  //----- Get file in update mode -----//
  TFile *file = TFile::Open(infile, "UPDATE");
  if(!file){
    std::cout << "Info in normhist.C: norm_signal --> File not found\nBye" << std::endl;
    return;
  }

  // Get historgram
  string dir = "/" + cut_id + "/" + schannel + "/run_" + std::to_string(run);
  string shist = "mwp_" + schannel;

  gDirectory->cd((const char*) dir.c_str());
  TH1 *sig = dynamic_cast<TH1*>(gDirectory->Get((const char*)shist.c_str()));
  if (sig->GetSumw2N() == 0) sig->Sumw2(kTRUE);

  TH1 *snormed = (TH1*) sig->Clone();
  snormed->Scale(1/(snormed->Integral())); // Scale
  snormed->Draw();

  // Save
  snormed->SetNameTitle("snormed", "snormed");
  snormed->Write("snormed",TObject::kOverwrite);
  file->Close();
}




int main(int argc, char* argv[]){
  if (argc < 5){
    std::cout << "Usage: " << argv[0] << " [filename] [cutID] [signal_run_start] [signal_run_end]" << std::endl;
  return 1;
  }
  const char* filename = argv[1];
  string cutID = std::string(argv[2]);
  char *p;
  int run_start = strtol(argv[3], &p, 10);
  int run_end = strtol(argv[4], &p, 10);
  norm_background(argv[1], argv[2]);

  for (int run = run_start; run <= run_end; run++){
    norm_signal(filename, cutID, run);
  }
}
