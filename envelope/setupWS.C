{

  // open ws file
  TFile *tf = TFile::Open("envelopews_wsignal_toy1_110to150.root");
  RooWorkspace *myWS = (RooWorkspace*)tf->Get("multipdf");
  RooWorkspace *newWS = new RooWorkspace("NewWS","NewWS");

  newWS->import( *myWS->var("CMS_hgg_mass") );
  newWS->import( *myWS->var("mean") );
  newWS->import( *myWS->var("sigma") );
  newWS->import( *myWS->var("nbkg") );
  newWS->import( *myWS->function("nsig") );

  // construct signal
  newWS->factory( "RooGaussian::signal( CMS_hgg_mass, mean, sigma )" );

  // construct s+b
  std::vector< TString > bkg_pdfs;
  bkg_pdfs.push_back( "exp1" );
  bkg_pdfs.push_back( "bern1" );
  bkg_pdfs.push_back( "lau1" );
  bkg_pdfs.push_back( "pow1" );

  // canvas
  TCanvas *c = new TCanvas("c","c", 1600,1600);
  c->Divide(2,2);

  for (int b=0; b<bkg_pdfs.size(); b++) {
    newWS->import( *myWS->pdf(Form("env_pdf_1_8TeV_%s",bkg_pdfs[b].Data())));
    newWS->factory( Form("SUM::sb_%s(  nbkg * env_pdf_1_8TeV_%s, nsig * signal )", bkg_pdfs[b].Data(), bkg_pdfs[b].Data()) );
    RooAbsPdf *bkg_pdf = (RooAbsPdf*)newWS->pdf(Form("env_pdf_1_8TeV_%s",bkg_pdfs[b].Data()));
    RooArgSet *nuis_params = bkg_pdf->getParameters( RooArgSet( *newWS->var("CMS_hgg_mass") ) );
    newWS->defineSet( Form("nuis_params_sb_%s",bkg_pdfs[b].Data()), *nuis_params );
    nuis_params->add( *newWS->var("nbkg") );
    newWS->factory( Form("RooUniform::prior_nuis_%s( nuis_params_sb_%s )",bkg_pdfs[b].Data(), bkg_pdfs[b].Data()) );


    //RooAbsPdf *pdf = (RooAddPdf*)myWS->pdf(Form("sb_%s",bkg_pdfs[b].Data()));
    //myWS->defineSet( Form("nuis_params_sb_%s",bkg_pdfs[b].Data()), *nuis_params );

    // fit and draw
    //c->cd(b+1);
    //RooAbsData *data = (RooAbsData*)myWS->data("roohist_data_mass_cat1_toy1_cutrange__CMS_hgg_mass");
    //pdf->fitTo(*data,RooFit::Extended());
    //RooPlot *plot = myWS->var("CMS_hgg_mass")->frame();
    //data->plotOn(plot);
    //pdf->plotOn(plot);
    //plot->Draw();
    //c->Update();
    //c->Modified();

  }
  //c->Print("fits.pdf");

  // construct dummy prior
  //myWS->factory( "RooPolynomial::prior( r, 0.)" );
  //RooAbsPdf *prior = (RooPolynomial*)myWS->pdf("prior");

  // construc POI prior
  newWS->factory( "RooUniform::prior_r( r ) " );
  // set POIs
  RooArgSet *pois = new RooArgSet( *newWS->var("r") );
  newWS->defineSet( "pois", *pois );

  newWS->Print();
  newWS->writeToFile("EnvelopeBatWS.root");
  tf->Close();

}
