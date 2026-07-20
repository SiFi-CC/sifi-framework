void FitHistogramsDisplayAutoPeak() {
    TString inputFile = "/scratch3/gccb/data/GSI2025/results/sifi_efficiency_run1224_HISTOS.root";
    TFile *inRoot = TFile::Open(inputFile);
    if(!inRoot || inRoot->IsZombie()) {
        std::cerr << "Cannot open file: " << inputFile << std::endl;
        return;
    }

    TString sides[2] = {"B","T"};

    // One canvas per layer/side
    TCanvas *c[4][2]; // [layer][side]

    for(int L=0; L<4; ++L){
        for(int S=0; S<2; ++S){
            TString side = sides[S];
            c[L][S] = new TCanvas(Form("cL%d%s",L,side.Data()), Form("Layer %d %s",L,side.Data()), 1200,800);
            c[L][S]->Divide(7,4);

            for(int E=0; E<28; ++E){
                TH1 *hist = nullptr;
                TString histName = Form("Module0/Layer%d/Element%d/Q_M0L%dE%d%s",L,E,L,E,side.Data());
                inRoot->GetObject(histName, hist);
                if(!hist) continue;

                c[L][S]->cd(E+1);

                // ----- Use TSpectrum to find peaks -----
                hist->GetXaxis()->SetRangeUser(6.5, 25); // range where we expect the peak
                TSpectrum *spectrum = new TSpectrum(4);
                int nPeaks = spectrum->Search(hist, 1, "", 0.1);
                Double_t *xPeaks = spectrum->GetPositionX();

                Double_t peak = 10.0; // default fallback
                if(nPeaks > 0) peak = xPeaks[0];

                // ----- Fit exponential tail first -----
                TF1* fitExp = new TF1("fitExp","expo",14,25);
                hist->Fit(fitExp,"RQN"); // Fit quietly

                // ----- Fit Gaussian + exponential around peak -----
                TF1 *fitFunc = new TF1(Form("fit_M0L%dE%d%s",L,E,side.Data()),
                                        "[0]*TMath::Gaus(x,[1],[2],1)+[3]*TMath::Exp([4]*x)", 
                                        peak-4, peak+15);
                // Initial parameters: amplitude, mean, sigma, exp amp, exp slope
                fitFunc->SetParameters(0.6*hist->GetBinContent(hist->FindBin(peak)),
                                       peak, 1, fitExp->GetParameter(0), fitExp->GetParameter(1));
                fitFunc->SetParLimits(2, 0.3, 3.0); // sigma limits

                // ----- Draw histogram and fit -----
                hist->GetXaxis()->SetRangeUser(0,40);
                hist->Draw("hist");
                hist->Fit(fitFunc,"QR");  // Quiet fit
                fitFunc->Draw("SAME");    // Draw on top
            }
        }
    }

    // Keep ROOT GUI running if in batch mode
    // gApplication->Run();
}
