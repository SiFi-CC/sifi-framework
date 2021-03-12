# SignalsViewer.C
This macro allows to plot signals recorded with the CAEN Desktop Digitzer and Keysight oscilloscope.  It is possible to view subsequent signals, compare signals from different channels as well as filter signals of desired parameters.

## Viewing subsequent signals

Use function
```c++
Bool_t SignalsViewer(TString path_name, Int_t ch, Float_t thr, Bool_t BL_flag)
```
where arguments are:
- `path_name` - path to the experimental data: for Desktop Digitizer path to the directory only, for oscilloscope path along with the file name
- `ch` - channel number: for Desktop Digitizer [0-15], for oscilloscope [0-3]
- `thr` - threshold value: for Desktop Digitizer given in [ADC channels], for oscilloscope given in [mV]
- `BL_flag` - flag indicating base line subtraction: if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This is interface function to view subsequent signals recorded from one, chosen channel `ch`. Signals recorded with the Desktop Digitizer as well as oscilloscope can be viewed. Threshold level `thr` will be plotted along with the associated T0. To view next signal double click on the canvas. When done exit ROOT by choosing `File` and `Quit ROOT` on the canvas toolbar.

Usage example (Desktop Digitizer):
```
$root
[] .L SignalsViewer.C+
[] SignalsViewer("/home/path/to/data/2021_01_01_01_01/", 0, -164, 1)
```
Usage example (oscilloscope):
```
$root
[] .L SignalsViewer.C+
[] SignalsViewer("/home/path/to/data/2021_01_01_01_01/file.csv", 0, -1, 1)
```


Alternatively use function (Desktop Digitizer only):
```c++
Bool_t SignalsViewerDD(TSring path, Int_t ch, Int_t thr, Bool_t BL_flag)
```
where arguments are:
- `path` - path to the directory containing experimental data; directory must contain `wave_x.dat` files recorded by the Desktop Digitizer
- `ch` - channel number [0-15]
- `thr `- threshold value [ADC channels]
- `BL_flag` - flag indicating whether base line should be subtracted or not; if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This function allows to view subsequent signals recorded from one, chosen channel `ch` of the Desktop Digitizer. Threshold level `thr` will be plotted along with the associated T0 value. To view next signal double click on the canvas. When done exit ROOT by choosing `File` and `Quit ROOT` on the canvas toolbar.

This function can be used indirectly via `SignalsViewer(path, ch, thr, BL_flag)`.
It can be also used directly as follows:
```
$ root
[] .L SignalsViewer.C+
[] SignalsViewerDD("/home/path/to/data/2021_01_01_01_01",  0,  -164,  kTRUE)
```

Alternatively use function (oscilloscope only):
```c++
Bool_t SignalsViewerOsc(TString path_name, Int_t ch, Float_t thr, Bool_t BL_flag)
```
where arguments are:
- `path_name` - full path and name of the `*.csv` file containing recorded data
- `ch` - channel number [0-3]
- `thr` - threshold value given in [mV]
- `BL_flag` - flag indicating whether base line should be subtracted or not; if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

Function to view subsequent signals recorded from one, chosen channel `ch` of the oscilloscope. Threshold level `thr` will be plotted along with the associated T0. To view next signal double click on the canvas. When done exit ROOT by choosing `File` and `Quit ROOT` on the canvas toolbar.

This function can be used indirectly via `SignalsViewer(path, ch, thr, BL_flag)`.
It can be also used directly as follows:
```
$root
[] .L SignalsViewer.C+
[] SignalsViewerOsc("/home/path/to/data/2021_01_01_01_01/file.csv", 0, -1, 1)
```

## Comparing signals from different channels

Use function:
```c++
Bool_t SignalsViewer(TString path_name, Int_t ch,  Bool_t BL_flag)
```
where arguments are:
- `path_name` - path to the experimental data: for Desktop Digitizer path to the directory only, for oscilloscope path along with the name of the file
- `ch_list` - vector containing list of channel numbers: for Desktop Digitizer [0-15], for oscilloscope [0-3]
- `BL_flag` - flag indicating whether base line should be subtracted or not; if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This is interface function to view signals from many channels simultaneously. For the Desktop Digitizer maximum of 16 channels can be viewed at the same time. For the oscilloscope maximum of 4 channels can be viewed at the same time. To view next set of signals double click on the canvas. To examine threshold levels and T0 more carefully use `SignalsViewer()` function for a single channel. When done exit ROOT by choosing `File` and `Quit ROOT` on the canvas toolbar.

Usage example (Desktop Digitizer):
```
$root
[] .L SignalsViewer.C+
[] SignalsViewer("/home/path/to/data/2021_01_01_01_01/", {0,1}, 1)
```
Usage example (oscilloscope):
```
$root
[] .L SignalsViewer.C+
[] SignalsViewer("/home/path/to/data/2021_01_01_01_01/file.csv", {0,1}, 1)
```


Alternatively use function (Desktop Digitizer only):
```c++
Bool_t SignalsViewerDD(TString path, std::vector <Int_t> ch_list, Bool_t BL_flag)
```
where argumets are:
- `path` - path to the directory containing experimental data; directory must contain `wave_x.dat` files recorded by the Desktop Digitizer
- `ch_list` - list of channels given as standard vector; up to 16 channels can be viewed at the same time [0-15]
- `BL_flag` - flag indicating whether base line should be subtracted or not; if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This function allows to view signals from many channels simultaneously. Maximum of 16 channels can be viewed at the same time. To view next set of signals double click on the canvas.  When done exit ROOT by choosing `File` and `Quit ROOT` on the canvas toolbar. To examine threshold levels and T0 more carefully use `SignalsViewer()` function for a single channel.

This function can be used indirectly via `SignalsViewer(path, ch_list, BL_flag)`.
It can be also used directly as follows:
```
$ root
[] .L SignalsViewer.C+
[] SignalsViewerDD("/home/path/to/data/2021_01_01_01_01/",  {0,1},  1)
```


Alternatively use function (oscilloscope only):
```c++
Bool_t SignalsViewerOsc(TString path, std::vector <Int_t> ch_list, Bool_t BL_flag)
```
where arguments are:
- `path` - full path and name of the `*.csv` file containing recorded data
- `ch_list` - vector containing list of channel numbers; up to 4 channels can be viewed at the same time [0-3]
- `BL_flag` - flag indicating whether base line should be subtracted or not; if `kTRUE` base line will be calculated and subtracted, if `kFALSE` base line will be ignored; base line is calculated as average of first 50 samples in the signal

This function allows to view signals recorded by the oscilloscope from many channels simultaneously. Maximum of 4 channels can be viewed at the same time. To view next set of signals double click on the canvas.  When done exit ROOT by choosing `File` and `Quit ROOT` on the canvas toolbar. To examine threshold levels and T0 more carefully use SignalsViewer() function for a single channel.

This function can be used indirectly via `SignalsViewer(path, ch_list, BL_flag)`.
It can be also used directly as follows:
```
$ root
[] .L SignalsViewer.C+
[] SignalsViewerOsc("/home/path/to/data/2021_01_01_01_01/file.csv", {0,1}, 1)
```

## Plotting signals of chosen parameters
Use function
```c++
Bool_t CutAndView(TString path, Int_t ch, Int_t thr, CutType cut, std::vector <Float_t> range, Int_t first, Int_t last)
```
where arguments are:
- `path` - path to the directory containing experimental data; the directory must contain `wave_x.dat` files recorded by the Desktop Digitizer as well as ROOT files produced by the `sifi-framework` and associated params.txt file
- `ch` - channel number [0-16]
- `thr` - threshold value [ADC channels]
- `cut` - type of cut given as enumeration `CutType` (see below)
- `range` - ranges for imposed cut given as standard vector; ranges should be given in order from min to max; when cut concerns more than one parameter provide ranges in order given in the enumeration name, i.e. for fAmpPE order is: min amplitude,  max amplitude, min PE, max PE
- `first` - number of the first signal fulfilling the cut to be plotted
- `last` - number of the last signal fulfilling the cut to be plotted

This function allows to plot signals of chosen parameters. At the moment only Desktop Digitizer file format is supported. Signals are plotted on one canvas, but separate pads. Threshold  level as well as T0 are marked. Base line, calculated from first 50 samples in the signal, is always subtracted.

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
[] .L SignalsViewer.C+
[] CutAndView("/home/path/to/data/2021_01_01_01_01", 0, -164, CutType::fAmp, {100,200}, 1, 10)
```

## Internal functions

```c++
Float_t GetT0(TH1F *h, Float_t thr, Bool_t BL_flag)
```
where arguments are:
- `h` - signal histogram
- `thr` - threshold value [mV]
- `BL_flag` - flag indicating whether base line was subtracted

This function finds and returns T0 value for given histogram `h` and threshold `thr`. The flag `BL_flag` indicating base line subtraction is needed to correctly determine polarity of the signal. This function was not meant to be used independently and was written as a tool for `SignalsViewer()` function.

```c++
TH1F* ReadOneSignal(std::ifstream &input, Int_t infile, SDDSignal *sptr )
```
where arguments are:
- `input`- input file stream from binary file containging experimental data
-  `infile` - position in the binary file where chosen signal begins
- `sptr` - pointer to SDDSignal object corresponding to the chosen signal

This function allows to read single, chosen signal from the binary file. Chosen signal is returned in a form of histogram. Base line is subtracted. This function is not meant to be used independently, it was written as a tool for `CutAndView()` function. 

# profile.sh
This script sets paths necessary for sifi-framework to work:

- `SIFIDIR` - path to the current official installation of the sifi-framework on scratch
- `PATH`  - path to current official executables
- `ROOTLOGON` - path tou your rootlogon.C script

At the end of the file currently used installation of ROOT is loaded. At the moment we are using ROOT v. 6.22.06 installed on scratch.

You should adjust this file according to your needs.

Every time you need to use ROOT or sifi-framework run this script in your terminal:
```
. profile.sh
```
Alternatively add it to your `.bashrc` file, so it will be loaded automatically in every new bash session. Open `.bashrc` file:
```
kate ~/.bashrc &
```
Add path to your `profile.sh` at the end of the file:
```
. /home/user/path/to/profile.sh
```
Save and close.

# rootlogon.C
This script loads all necessary libraries to ROOT session. If it is present in your working directory it will be loaded automatically when you start ROOT. You can also set environment such that `rootlogon.C` will be loaded in every directory. For that you need to have `ROOTLOGON` variable correctly set in your `profile.sh`. Then open `.rootrc` file:
```
kate ~/.rootrc &
```
and add the following line:
```
Rint.Logon: $(ROOTLOGON)
```
Save and close. If `.rootrc` file doesn't exist create it first in your home directory.

# params.txt
This is examplary file containing parameters for sifi_dst. It contains parameters for unpackers, calibrators, lookup tables and detector geometry. Modify this file according to your needs. 

## Syntax

## Structure

### [SFibersGeomPar]
- `fModuleZ: Float_t`
- `nModules: Int_t`
- `nLayers: Int_t`
- `nFibers: Int_t`
- `fLayerRotation: Float_t`
- `fFiberOffsetX: Float_t`
- `fFiberOffsetY: Float_t`
- `fFibersPitch: Float_t`

### [SFibersDDUnpackerPar]
- `fThreshold: Float_t`
- `fPolarity: Int_t`
- `fAnaMode: Int_t`
- `fIntMode: Int_t`
- `fDeadTime: Int_t`
- `fVetoThreshold: Float_t`
- `fBLMode: Int_t`

### [SFibersDDLookupTable]
- FTAB ID channel
- module
- layer
- fiber
- side

### [SFibersDDCalibratorPar]
- address: module, layer, fiber, side
- gain
- offset

### [SFibersCalibratorPar]
- address: module, layer, fiber, side
- lambda
- eta_r
- eta_l
- ksi
- length
- time offset

### [SFibersHitFinderPar]
- `fA0: Float_t`
- `fLambda: Float_t`
- `fAlpha: Float_t`

### [SFibersClusterFinderPar]
- `fClusterMode: Int_t`

### [SFibersDigitizerPar]

### [SFibersHitFinderFiberPar]
