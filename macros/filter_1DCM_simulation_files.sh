#!/bin/bash
# # FILES="/scratch2/gccb/monika/SiPMR0003_CodedMaskHIT_Raster_1_467_128.3_101.4_20_170_63_7_AllCoincidence_15_400_SYSTEM_MATRIX/SystemMatrix_CodedMaskHIT_simv5_Pixel*_0to39.root"
# FILES="/scratch2/gccb/monika/SiPMR0003_CodedMaskHIT_Raster_1_467_128.3_101.4_20_170_63_7_AllCoincidence_15_400_SYSTEM_MATRIX_200jobs/SystemMatrix_CodedMaskHIT_simv5_Pixel*_0to199.root"
# 
# for f in $FILES
# do
#     echo "Processing $f file..."
#     g++ `root-config --cflags` DeadChannelsFilter.C `root-config --glibs` -I/scratch/gccb/software/include/ -L/scratch/gccb/software/lib/ -lSiPMRLib -o DeadChannelsFilter
#     ARG1_PATH="$f" LD_PRELOAD=/scratch/gccb/software/lib/libSiPMRLib.so ./DeadChannelsFilter
# done

FILES="/scratch2/gccb/monika/HIT_simulations/OptimisedGeometry_CodedMaskHIT_Spot*_1e10_protons_MK.root"

for f in $FILES
do
    echo "Processing $f file..."
    g++ `root-config --cflags` DeadChannelsFilter.C `root-config --glibs` -I/scratch/gccb/software/include/ -L/scratch/gccb/software/lib/ -lSiPMRLib -o DeadChannelsFilter
    ARG1_PATH="$f" LD_PRELOAD=/scratch/gccb/software/lib/libSiPMRLib.so ./DeadChannelsFilter
done


# g++ `root-config --cflags` DeadChannelsFilter.C `root-config --glibs` -I/scratch/gccb/software/include/ -L/scratch/gccb/software/lib/ -lSiPMRLib -o DeadChannelsFilter
# ARG1_PATH="/scratch/gccb/monika/HIT_simulations/OptimisedGeometry_CodedMaskHIT_Spot1_1e10_protons_MK.root" LD_PRELOAD=/scratch/gccb/software/lib/libSiPMRLib.so ./DeadChannelsFilter


# g++ `root-config --cflags` DeadChannelsFilter.C `root-config --glibs` -I/scratch/gccb/software/include/ -L/scratch/gccb/software/lib/ -lSiPMRLib -o DeadChannelsFilter 
# ARG1_PATH="/scratch2/gccb/monika/SiPMR0003_CodedMaskHIT_Raster_1_467_128.3_101.4_20_170_63_7_AllCoincidence_15_400_SYSTEM_MATRIX/SystemMatrix_CodedMaskHIT_simv5_Pixel50_0to39.root" LD_PRELOAD=/scratch/gccb/software/lib/libSiPMRLib.so ./DeadChannelsFilter
