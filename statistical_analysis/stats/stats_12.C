// Graph significance values on TGraph
// References:
// https://root.cern.ch/doc/master/classTTreeReader.html
// https://root.cern.ch/doc/master/classTGraphPainter.html

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TGraphErrors.h"

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
TGraphErrors * signif ( const char* infile,
                        const char* tntuple="significance"){
  // Open File
  TFile *file = TFile::Open(infile, "READ");
  if (!file) return 0;

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
  TMultiGraph *multigraph = new TMultigraph();  // ??????????????

  TGraphErrors *gr1 = signif(infile1, tntuple);
  TGraphErrors *gr2 = signif(infile2, tntuple);
  TGraphErrors *gr3 = signif(infile3, tntuple);
  TGraphErrors *gr4 = signif(infile4, tntuple);
  TGraphErrors *gr5 = signif(infile5, tntuple);

  if (!gr1) multigraph->Add(gr1, //specifications);
