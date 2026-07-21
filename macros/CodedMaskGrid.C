void CodedMaskGrid()
{
    const int CM_size = 57;
    const int full_pixels[14] = {5, 3, 4, 1, 1, 2, 2, 1, 1, 1, 1, 5, 1, 1};
    const int empty_pixels[13] = {1,5,1,1,1,1,2,2,1,1,4,3,5};

    TH2I *hGrid = new TH2I("hGrid", "Coded Mask Grid", CM_size, -0.5, CM_size - 0.5,1 , 0, 1);

    int i_full = 0;
    int i_empty = 0;
    int i_filled = 0;
    for(int i = 0; i_filled < CM_size;)
    {
        while(i < full_pixels[i_full] + i_filled)
        {
            hGrid->SetBinContent(i + 1, 1, 1);
            i++;
        }
        i_filled = i;
        i_full++;
        while(i < empty_pixels[i_empty] + i_filled)
        {
            hGrid->SetBinContent(i + 1, 1, 0);
            i++;
        }
        i_filled = i;
        i_empty++;
    }
    gStyle->SetOptStat(0);
    auto c1 = new TCanvas("c1", "Coded Mask Grid", 800, 100);
    hGrid->GetYaxis()->SetNdivisions(0);
    hGrid->Draw("col");

    for(int i = 0; i < CM_size; i++)
    {
        TLine *l = new TLine(i - 0.5, 0, i - 0.5, 1);
        l->SetLineStyle(2);
        l->SetLineColor(kBlack);
        l->Draw();
    }
}