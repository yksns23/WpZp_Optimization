// Graph significance values on TGraph
// Call multigraph_signif with input files from the /sigma_results directory
// Load this macro in a ROOT session. Compile if necessary by loading it with a + (.L visualize.C+)

// References:
// https://root.cern.ch/doc/master/classTTreeReader.html
// https://root.cern.ch/doc/master/classTGraphPainter.html
// Color option: https://root.cern.ch/doc/master/TColor_8h.html
#include "iostream"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "Canvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TColor.h"

TGraphErrors * signif ( const char* infile,
                        const char* tntuple ); // Return graph pointer

void graph_signif ( const char* infile,
                    const char* tntuple );   // Just draw the graph

void multigraph_signif (const char* infile1,
                        const char* infile2,
                        const char* infile3,
                        const char* infile4,
                        const char* infile5,
                        const char* tntuple); 




// -------------------------------------------------------------
// -------------------------------------------------------------
TGraphErrors * signif ( const char* infile,
                        const char* tntuple="significance"){
  // Open File
  if (!infile) return 0;
  TFile *file = TFile::Open(infile, "READ");

  // Set TTreeReader and variables
  TTreeReader reader(tntuple, file);
  TTreeReaderValue<Float_t> mwp_val(reader, "mwp");
  TTreeReaderValue<Float_t> sig_val(reader, "significance");
  TTreeReaderValue<Float_t> sig_error_val(reader, "sig_error");

  // Set array
  Float_t mwp[20], sig[20], sig_error[20];

  int i=0;
  while (reader.Next()){
    mwp[i] = *mwp_val;
    sig[i] = *sig_val;
    sig_error[i] = *sig_error_val;
    i++;
  }

  // Graph
  TGraphErrors *gr = new TGraphErrors(i, mwp, sig, 0, sig_error);
  //gr->SetMarkerStyle(3);
  //gr->SetLineStyle(3);
  gr->SetLineWidth(2);

  return gr;
}

void graph_signif ( const char* infile,
                    const char* tntuple="significance"){

  // Open File
  TFile *file = TFile::Open(infile, "READ");

  // Set TTreeReader and variables
  TTreeReader reader(tntuple, file);
  TTreeReaderValue<Float_t> mwp_val(reader, "mwp");
  TTreeReaderValue<Float_t> sig_val(reader, "significance");
  TTreeReaderValue<Float_t> sig_error_val(reader, "sig_error");

  // Set array
  Float_t mwp[20], sig[20], sig_error[20];

  int i=0;
  while (reader.Next()){
    mwp[i] = *mwp_val;
    sig[i] = *sig_val;
    sig_error[i] = *sig_error_val;
    i++;
  }

  // Graph
  TGraphErrors *gr = new TGraphErrors(i, mwp, sig, 0, sig_error);
  gr->SetMarkerStyle(3);
  gr->SetLineStyle(3);
  gr->SetLineWidth(1);
  //gr->SetLineColor(20);
  gr->SetTitle("Significance");
  gr->GetXaxis()->SetTitle("Mass of reconstructed W' object");
  gr->GetYaxis()->SetTitle("Significance [sigma]");
  
  // Draw
  TCanvas *c1 = new TCanvas("c1", "c1");
  TPad *p1 = new TPad("p1", "p1", 0, 0, 1, 1);
  p1->Draw();

  p1->cd();
  gr->Draw();
  c1->Update();
}


void multigraph_signif (const char* infile1,
                        const char* infile2=0,
                        const char* infile3=0,
                        const char* infile4=0,
                        const char* infile5=0,
                        const char* tntuple="significance"){
  // New TMultiGraph object
  TMultiGraph *multigraph = new TMultiGraph("gr", "Significance;M_{tb} [GeV];significance [#sigma]");
     // Name, Title; X-axis Title; Y-axis Title

  TGraphErrors *gr1 = signif(infile1, tntuple);
  TGraphErrors *gr2 = signif(infile2, tntuple);
  TGraphErrors *gr3 = signif(infile3, tntuple);
  TGraphErrors *gr4 = signif(infile4, tntuple);
  TGraphErrors *gr5 = signif(infile5, tntuple);

  // Visual options
  gStyle->SetPalette(59); // Automatically choose color from palette
  if (gr1){
    string str;  // User type in legend
    std::cout << "Legend for file 1: \n";
    std::getline(std::cin, str);
    gr1->SetMarkerStyle(4); // Set unique marker style
    gr1->SetTitle(str.c_str()); multigraph->Add(gr1);}
  if (gr2){
    string str;  // User type in legend
    std::cout << "Legend for file 2: \n";
    std::getline(std::cin, str);
    gr2->SetMarkerStyle(5); // Set unique marker style
    gr2->SetTitle(str.c_str()); multigraph->Add(gr2);}
  if (gr3){
    string str;  // User type in legend
    std::cout << "Legend for file 3: \n";
    std::getline(std::cin, str);
    gr3->SetMarkerStyle(25); // Set unique marker style
    gr3->SetTitle(str.c_str()); multigraph->Add(gr3);}
  if (gr4){
    string str;  // User type in legend
    std::cout << "Legend for file 4: \n";
    std::getline(std::cin, str);
    gr4->SetMarkerStyle(36); // Set unique marker style
    gr4->SetTitle(str.c_str()); multigraph->Add(gr4);}
  if (gr5){
    string str;  // User type in legend
    std::cout << "Legend for file 5: \n";
    std::getline(std::cin, str);
    gr5->SetMarkerStyle(35); // Set unique marker style
    gr5->SetTitle(str.c_str()); multigraph->Add(gr5);}

  // Draw
  TCanvas *cg = new TCanvas("cg", "cg");
  TPad *p1 = new TPad("p1", "p1", 0, 0, 1, 1);
  p1->Draw();

  p1->cd();
  multigraph->Draw("a pl x plc pmc"); // Draw options for TMultiGraph
  gPad->BuildLegend();
  cg->Update();
}
