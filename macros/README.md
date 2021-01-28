# SignalsViewer.C
This macro allows to plot signals recorded with the CAEN Desktop Digitzer .  It is possible to view subsequent signals, compare signals from different channels as well as filter signals of desired parameters.

## Viewing subsequent signals
Use function
```c++
Bool_t SignalsViewer(TSring path, Int_t ch, Int_t thr, Bool_t BL_flag)
```
where arguments are:
- `path` - path to the directory containing experimental data; directory must contain `wave_x.dat` files recorded by the Desktop Digitizer
- `ch` - channel number
- `thr `- threshold value [ADC channels]
- `BL_flag` - if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This function allows to view subsequent signals recorded from one, chosen channel `ch`. Threshold level `thr` will be plotted along with the associated T0 value. To view next signal double click on the canvas. When done exit ROOT by choosing `File` and ` Quit ROOT` on the canvas toolbar.

Usage example:
```
$ root
[ ] .L SignalsViewer.C+
[ ] SignalsViewer("/home/path/to/data/2021_01_01_01_01",  0,  -164,  kTRUE)
```
## Comparing signals from different channels
Use function
```c++
Bool_t SignalsViewer(TString path, std::vector <Int_t> ch_list, Bool_t BL_flag)
```
where argumets are:
- `path` - path to the directory containing experimental data; directory must contain `wave_x.dat` files recorded by the Desktop Digitizer
- `ch_list` - list of channels given as standard vector;  up to 16 channels can be viewed at the same time
- if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This function allows to view signals from many channels simultaneously. Maximum of 16 channels can be viewed at the same time. To view next set of signals double click on the canvas.  When done exit ROOT by choosing `File` and ` Quit ROOT` on the canvas toolbar. To examine threshold levels and T0 more carefully use `SignalsViewer()` function for a single channel.

Usage example:
```
$ root
[ ] .L SignalsViewer.C+
[ ] SignalsViewer("/home/path/to/data/2021_01_01_01_01/",  {0,1},  1)
```

## Plotting signals of chosen parameters
Use function
```c++
Bool_t CutAndView(TString path, Int_t ch, Int_t thr, CutType cut, std::vector <Float_t> range, Int_t first, Int_t last)
```
where arguments are:
- `path` - path to the directory containing experimental data; the directory must contain `wave_x.dat` files recorded by the Desktop Digitizer as well as ROOT files produced by the `sifi-framework` and associated params.txt file
- `ch` - channel number
- `thr` - threshold value [ADC channels]
- `cut` - type of cut given as enumeration `CutType` (see below)
- `range` - ranges for imposed cut given as standard vector; ranges should be given in order from min to max; when cut concerns more than one parameter provide ranges in order given in the enumeration name, i.e. for fAmpPE order is: min amplitude,  max amplitude, min PE, max PE
- `first` - number of the first signal fulfilling the cut to be plotted
- `last` - number of the last signal fulfilling the cut to be plotted

This function allows to plot signals of chosen parameters.  Signals are plotted on one canvas, but separate pads. Threshold  level as well as T0 are marked. Base line is always subtracted.

In order to select desired signals enumeration `CutType` representing different  types of cuts  is provided. The following enumerators are defined:
- `fAmp` - cut on amplitude; provide amplitude min and max
- `fCharge` - cut on uncalibrated charge; provide charge min and max
- `fPE` - cut on calibrated charge, i.e. PE; provide PE min and max
- `fT0` - cut on time T0; provide T0 min and max
- `fTOT`- cut on time over threshold; provide TOT min and max
- `fBLSig` - cut on base line sigma; provide BL sigma min and max
- `fBL` - cut on base line, provide BL min and max
- `fVeto` - cut on veto flag; provide flag value 0 or 1
- `fPileUp `- cut on pile up flag; provide flag value 0 or 1
- `fAmpPE `- cut on amplitude and PE;  provide amplitude min and max as well as PE min and max
- `fAmpPESig `- cut on amplitude, PE and base line sigma; provide amplitude min and max, PE min and max and BL sigma max
- `fAmpPESigVeto` - cut on amplitude, PE, base line sigma and veto flag; provide amplitude min and max, PE min and max and BL sigma max; veto flag is assumed to be 0.


 Usage example:
```c++
$ root
[ ] .L SignalsViewer.C+
[ ] CutAndView("/home/path/to/data/2021_01_01_01_01", 0, -164, CutType::fAmp, {100,200}, 1, 10)
```

## Internal functions

```c++
Float_t GetT0(TH1F *h, Float_t thr)
```
where arguments are:
- `h` - signal histogram
- `thr` - threshold value [mV]

This function finds and returns T0 value for given histogram `h` and threshold `thr`.  This function was not meant to be used independently and was written as a tool for `SignalsViewer()` function.

```c++
TH1F* ReadOneSignal(std::ifstream &input, Int_t infile, SDDSignal *sptr )
```
where arguments are:
- `input`- input file stream from binary file containging experimental data
-  `infile` - position in the binary file where chosen signal begins
- `sptr` - pointer to SDDSignal object corresponding to the chosen signal

This function allows to read single, chosen signal from the binary file. Chosen signal is returned in a form of histogram. Base line is subtracted. This function is not meant to be used independently, it was written as a tool for `CutAndView()` function. 
