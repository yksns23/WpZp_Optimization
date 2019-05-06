// Normalize histograms to unity
// to represent PDF
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <iostream>
#include <string>
#include <cstdlib>

void norm_background (   const char*  infile,
                              string  cut_id,
                                 int  b_nevents,
                              string  bchannel1,
                            double_t  bxsec1,
                              string  bchannel2,
                            double_t  bxsec2,
                              string  bchannel3,
                            double_t  bxsec3,
                              string  bchannel4,
                            double_t  bxsec4,
                              string  bchannel5,
                            double_t  bxsec5,
                              string  bchannel6,
                            double_t  bxsec6,
                              string  bchannel7,
                            double_t  bxsec7,
                              string  bchannel8,
                            double_t  bxsec8         );

void normalize_signal (  const char*  infile,
                              string  cut_id,
                                 int  s_nevents,
                                 int  run,
                              string  schannel );





void norm_background (   const char*  infile,
                              string  cut_id,
                                 int  b_nevents,
                              string  bchannel1="background_jjbb",
                            double_t  bxsec1=2.402e+06,
                              string  bchannel2="background_jjbblvl",
                            double_t  bxsec2=70.57,
                              string  bchannel3=
                                      "background_jjbbvlvl",
                            double_t  bxsec3=50.94,
                              string  bchannel4=
                                      "background_pp_tb_combinations",
                            double_t  bxsec4=6.597,
                              string  bchannel5=
                                      "background_pp_tbZ_combinations",
                            double_t  bxsec5=0.006977,
                              string  bchannel6=
                                      "background_pp_tt_combinations",
                            double_t  bxsec6=460,
                              string  bchannel7=
                                      "background_pp_ttZ_combinations",
                            double_t  bxsec7=0.4252,      
                              string  bchannel8=
                                      "background_pp_ttW_combinations",
                            double_t  bxsec8=0.5266      ){

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

  string dir4 = "/" + cut_id + "/" + bchannel4 + "/run_1";
  string bhist4 = "mwp_" + bchannel4;

  string dir5 = "/" + cut_id + "/" + bchannel5 + "/run_1";
  string bhist5 = "mwp_" + bchannel5;

  string dir6 = "/" + cut_id + "/" + bchannel6 + "/run_1";
  string bhist6 = "mwp_" + bchannel6;

  string dir7 = "/" + cut_id + "/" + bchannel7 + "/run_1";
  string bhist7 = "mwp_" + bchannel7;

  string dir8 = "/" + cut_id + "/" + bchannel8 + "/run_1";
  string bhist8 = "mwp_" + bchannel8;

  gDirectory->cd((const char*) dir1.c_str());
  TH1 *bg1 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist1.c_str()));
  //if (bg1->GetSumw2N() == 0) bg1->Sumw2(kTRUE);
  //Don't understand this..

  gDirectory->cd((const char*) dir2.c_str());
  TH1 *bg2 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist2.c_str()));

  gDirectory->cd((const char*) dir3.c_str());
  TH1 *bg3 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist3.c_str()));

  gDirectory->cd((const char*) dir4.c_str());
  TH1 *bg4 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist4.c_str()));

  gDirectory->cd((const char*) dir5.c_str());
  TH1 *bg5 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist5.c_str()));

  gDirectory->cd((const char*) dir6.c_str());
  TH1 *bg6 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist6.c_str()));

  gDirectory->cd((const char*) dir7.c_str());
  TH1 *bg7 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist7.c_str()));

  gDirectory->cd((const char*) dir8.c_str());
  TH1 *bg8 = dynamic_cast<TH1*>(gDirectory->Get((const char*)bhist8.c_str()));


  // Individual histograms scaled by bxsec (fb-1) * yield
  bg1->Scale( bxsec1*1000*(bg1->Integral()/b_nevents) );
  bg2->Scale( bxsec2*1000*(bg2->Integral()/b_nevents) );
  bg3->Scale( bxsec3*1000*(bg3->Integral()/b_nevents) );
  bg4->Scale( bxsec4*1000*(bg4->Integral()/b_nevents) );
  bg5->Scale( bxsec5*1000*(bg5->Integral()/b_nevents) );
  bg6->Scale( bxsec6*1000*(bg6->Integral()/b_nevents) );
  bg7->Scale( bxsec7*1000*(bg7->Integral()/b_nevents) );
  bg8->Scale( bxsec8*1000*(bg8->Integral()/b_nevents) );

  // Combine histograms together to see overall shape
  int nbins = bg1->GetNbinsX();
  double_t low = bg1->GetXaxis()->GetBinLowEdge(1);
  double_t high = bg1->GetXaxis()->GetBinUpEdge(nbins);
  TH1 *bcombined = new TH1D("bcombined", "bcombined", nbins, low, high);

  bcombined->Add(bg1);
  bcombined->Add(bg2);
  bcombined->Add(bg3);
  bcombined->Add(bg4);
  bcombined->Add(bg5);
  bcombined->Add(bg6);
  bcombined->Add(bg7);
  bcombined->Add(bg8);

  bcombined->SetNameTitle("bcombined", "bcombined");

  // Write to file
  gDirectory->cd("/");
  gDirectory->pwd();
  bcombined->Write("bcombined", TObject::kOverwrite);
  bg1->Write("bg1", TObject::kOverwrite);
  bg2->Write("bg2", TObject::kOverwrite);
  bg3->Write("bg3", TObject::kOverwrite);
  bg4->Write("bg4", TObject::kOverwrite);
  bg5->Write("bg5", TObject::kOverwrite);
  bg6->Write("bg6", TObject::kOverwrite);  
  bg7->Write("bg7", TObject::kOverwrite);
  bg8->Write("bg8", TObject::kOverwrite);  
  file->Close();
}


void norm_signal   (  const char*  infile,
                           string  cut_id,
                              int  s_nevents,
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
  snormed->Scale(1000*(snormed->Integral()/s_nevents) ); // Scale
  snormed->Draw();

  // Save
  snormed->SetNameTitle("snormed", "snormed");
  snormed->Write("snormed",TObject::kOverwrite);
  file->Close();
}




int main(int argc, char* argv[]){
  if (argc < 7){
    std::cout << "Usage: " << argv[0] << " [filename] [cutID] [signal nevents] [background nevents] [signal_run_start] [signal_run_end]" << std::endl;
  return 1;
  }
  const char* filename = argv[1];
  string cutID = std::string(argv[2]);
  int s_nevents = std::stoi(argv[3]);
  int b_nevents = std::stoi(argv[4]);
  char *p;
  int run_start = strtol(argv[3], &p, 10);
  int run_end = strtol(argv[4], &p, 10);
  norm_background(filename, cutID, b_nevents);

  for (int run = run_start; run <= run_end; run++){
    norm_signal(filename, cutID, s_nevents, run);
  }
}
