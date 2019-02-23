// A creative (i.e. not required for project)
// implementation of sigma.C for saving data in database
// A lightweight calculator for significance values (final)

#include "TMySQLServer.h"

#include "TFile.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooDataSet.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/HypoTestCalculatorGeneric.h"
#include "RooStats/FrequentistCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestResult.h"

using namespace RooFit;
using namespace RooStats;

void sigma (    const char* infile,
                float_t     mwp,
                const char* workspace_name,
                const char* database,
                const char* user_id,
                const char* password,
                const char* sbmodel_name,
                const char* bmodel_name,
                const char* data_name     );

void sigma_scan (          int repeat,
                   const char* infile[],
                       float_t mwp[],
                   const char* workspace_name[],
                   const char* database,
                   const char* user_id,
                   const char* password,
                   const char* sbmodel_name,
                   const char* bmodel_name,
                   const char* data_name          );



// Frequentist p value calculator
void sigma (    const char* infile,
                float_t     mwp,
                const char* workspace_name = "combined",
                const char* outfile = "sigma.root",
                const char* sbmodel_name = "ModelConfig",
                const char* bmodel_name = "",
                const char* data_name = "asimovData"){

  ///// PART 1: Setup /////
  //----- Get File -----//
  TFile *file = TFile::Open(infile, "READ");
  if(!file){
    cout << "File not found\nBye" << endl;
    return;
  }

  //----- Get Workspace -----//
  RooWorkspace *w = (RooWorkspace*) file->Get(workspace_name);
  if (!w){
    cout << "Workspace not found\nBye" << endl;
    return;
  }

  //----- Get ModelConfig -----//
  // signal + background model
  ModelConfig *sbModel = (ModelConfig*) w->obj(sbmodel_name);

  // Get observed data.
  RooAbsData *data = (RooAbsData*) w->data(data_name);
  if (!data){
    cout << "Data not found\nBye" << endl;
    return;
  }

  // background only model
  ModelConfig *bModel = (ModelConfig*) w->obj(bmodel_name);
  
  if (!bModel){
    bModel = (ModelConfig*) sbModel->Clone();
    bModel->SetName(TString(sbmodel_name)+TString("_bmodel"));
    RooRealVar *poi = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
    double oldval = poi->getVal();
    poi->setVal(0);
    bModel->SetSnapshot(RooArgSet(*poi));
    poi->setVal(oldval);
  }

  if (!sbModel->GetSnapshot()){
    RooRealVar *poi = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
    sbModel->SetSnapshot(RooArgSet(*poi));
  }    

  ///// PART 2: Calculate local p for given mass /////

  //----- Configure Frequentist Method -----//
  // Set Test Statistic
  ProfileLikelihoodTestStat *profll = new ProfileLikelihoodTestStat(*sbModel->GetPdf());
  profll->SetOneSidedDiscovery(true);
  profll->SetVarName( "q_{0}/2" );
  profll->SetPrintLevel(0);

  // Set FrequentistCalculator
  HypoTestCalculatorGeneric *hc = new FrequentistCalculator(*data, *sbModel, *bModel);
  ((FrequentistCalculator*) hc)->SetToys(5000, 1250);

  // Set ToyMCSampler
  ToyMCSampler *sampler = (ToyMCSampler*)hc->GetTestStatSampler();
  sampler->SetGenerateBinned(true);
  sampler->SetTestStatistic(profll);

  //----- Get Results -----//
  HypoTestResult *result = hc->GetHypoTest();
  result->SetPValueIsRightTail(true);
  result->SetBackgroundAsAlt(false);
  result->Print();

  double_t significance = (double_t) result->Significance();
  double_t significance_error = (double_t) result->SignificanceError();
  double_t p_value = (double_t) result->NullPValue();
  double_t p_error = (double_t) result->NullPValueError();

  std::cout << "Expected significance is: " << significance <<
               "\nwith error : " << significance_error << std::endl;

  std::cout << "Expected p_value is: " << p_value <<
               "\nwith error : " << p_error << std::endl;

  delete profll;
  delete hc;

  /*
  result->Print();

  // Plot
  HypoTestPlot *plot = new HypoTestPlot(*result, 100);
  plot->SetLogYaxis(true);
  plot->Draw();
  */

  //----- Save results to database -----//
  // Connect to server
  TMySQLServer *mysqlserver = new TMySQLServer(database, user_id, password);
  const char* query1 = "CREATE TABLE IF NOT EXISTS sigma_result "
                      "(id INT NOT NULL AUTO_INCREMENT,"
                      " cut_id VARCHAR(20) NOT NULL,"
                      " mwp DECIMAL(6,2) NOT NULL,"
                      " signif FLOAT(8,7),"
                      " signif_error FLOAT(8,7),"
                      "p_value FLOAT(8,7),"
                      " p_value_error FLOAT(8,7),"
                      "PRIMARY KEY (id));"
  /////// to do: add stuff into it
  TSQLResult* qresult1 = mysqlserver->Query(query1);
  
  mysqlserver->Close();
}



// Do the same thing for multiple files
void sigma_scan (          int  repeat,  // how many files to process?
                   const char*  infile[],         // arrays
                       float_t  mwp[],            // ...
                   const char*  workspace_name[], // indices should match
                   const char*  outfile = "sigma.root",
                   const char*  sbmodel_name = "ModelConfig",
                   const char*  bmodel_name = "",
                   const char*  data_name = "asimovData"){

for (int i=0; i<repeat; i++){
  sigma(infile[i], mwp[i], workspace_name[i], outfile, sbmodel_name, bmodel_name, data_name);
}
}

