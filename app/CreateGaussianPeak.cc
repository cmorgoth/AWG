#include <iostream>
#include <fstream>
#include <TF1.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>

int main( int argc, char** argv )
{ 
  //units will be in ns
  float t_low  = 0.0;
  float t_high = 10000.0;
  TF1* f1 = new TF1("f1", "gaus(0)+gaus(3)", t_low, t_high);
  float first_mean  = 10.;//ns
  float second_mean = 20.;//ns
  float sigma       = 1.0;//ns
  f1->SetParameter(0,1);
  f1->SetParameter(1,first_mean);
  f1->SetParameter(2,sigma);
  f1->SetParameter(3,1);
  f1->SetParameter(4,second_mean);
  f1->SetParameter(5,sigma);

  TCanvas* c = new TCanvas( "c", "c", 800, 600 );
  c->SetHighLightColor(2);
  c->SetFillColor(0);
  c->SetBorderMode(0);
  c->SetBorderSize(2);
  c->SetFrameBorderMode(0);
  c->SetFrameBorderMode(0);

  std::ofstream ofs ("two_gaus.txt", std::ofstream::out);
  
  //int nsamples = 1e3;
  double step = 0.0400;
  int nsamples = (int)(t_high-t_low)/step;
  float alpha = 1;//IM insertion loss
  float Pin = 1;//LaserPower Input
  
  double t[nsamples];
  double input[nsamples];
  double output[nsamples];
  int event = 0;
  // Create a ROOT Tree
  TTree *tree = new TTree ("T", "An example of ROOT tree with a few branches");
  tree->Branch ("event", &event, "event/I");
  tree->Branch ("nsamples", &nsamples, "nsamples/I");
  tree->Branch ("input", input, "input[nsamples]/D");
  tree->Branch ("output", output, "output[nsamples]/D");
  tree->Branch ("t", t, "t1[nsamples]/D");

  
  
  std::cout << "[INFO]: step size = " << step << " [nsec]" << std::endl;
  std::cout << "[INFO]: number of samples = " << nsamples << std::endl;
  for ( int i = 0; i < nsamples; i++ )
    {
      ofs << f1->Eval(t_low+(double)i*step) << ",0,0"<< std::endl;
      input[i] = (3.5/TMath::Pi())*TMath::ACos(2.*f1->Eval(t_low+(double)i*step)/(alpha*Pin) -1.);
      output[i] = f1->Eval(t_low+(double)i*step);
      t[i] =  t_low+(double)i*step;
    };
  ofs.close();
  tree->Fill();

  event++;
  float step_time  = 25;//ns
  float step_width = 5;//ns
  std::ofstream ofs2 ("step_function.txt", std::ofstream::out);
  std::ofstream ofs3 ("step_in.txt", std::ofstream::out);
  double current_time;
  for ( int i = 0; i < nsamples; i++ )
    {
      current_time = t_low+(double)i*step;
      t[i] = current_time;
      if ( (current_time >= step_time && current_time < (step_time+step_width)) || (current_time >= 2*step_time && current_time < (2*step_time+step_width)) )
	{
	  ofs2 << "1,0,0"<< std::endl;
	  ofs3 << (3.5/TMath::Pi())*TMath::ACos(2.*1.0/(alpha*Pin) -1.) <<",0,0"<< std::endl;
	  input[i]  = (3.5/TMath::Pi())*TMath::ACos(2.*1.0/(alpha*Pin) -1.);
	  output[i] = 1.0;
	}
      else
	{
	  ofs2 << "0.0,0,0"<< std::endl;
	  ofs3 << (3.5/TMath::Pi())*TMath::ACos(2.*0.0/(alpha*Pin) -1.) <<",0,0"<< std::endl;
	  input[i]  = (3.5/TMath::Pi())*TMath::ACos(2.*0.0/(alpha*Pin) -1.);
	  output[i] = 0.0;
	}
     
    }
  tree->Fill();
  TFile* fout = new TFile("awg.root", "recreate");
  tree->Write();
  fout->Close();
  ofs2.close();
  ofs3.close();
  
  f1->SetNpx(1e5);
  f1->Draw();
  c->SaveAs("two_gauss.pdf");
  
  
  return 0;
}
