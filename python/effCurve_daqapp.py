#!/usr/bin/env python
import sys
from fememu_pycfg import apply_config
from ROOT import trigger

#
# Get configuration
#
cfg_file = sys.argv[1]
config = trigger.fememu.FEMBeamTriggerConfig()
if not apply_config(config,cfg_file):
    print('\033[91m[ERROR]\033[00m exiting...')
    sys.exit(1)

# test over a series of PE values
PEs = [1.5,1.75,2.0,2.25,2.5,2.75,3.0,3.25,3.5,3.75,4.0,4.25,4.5,4.75,5.0,5.25,5.5,5.75,6.0,6.25,6.5,6.75,7.0,7.25,7.5]

# file where to store output
fout = open('results.txt','w')

for pe in PEs:

    adcthresh = int(pe*20)
    print('ADC threshold for this run is ',adcthresh)

    config.fTriggerThresPHMAX=adcthresh

    #
    # Create FEMBeamTrigger algorithm w/ configuration
    #
    fememu = trigger.fememu.FEMBeamTriggerAlgo(config)

    k=trigger.DAQFileInterface()
    k.Reset()
    
    for x in range(len(sys.argv)-2):
        print('Input:',sys.argv[x+2])
        k.AddInputFile(sys.argv[x+2])
    k.SetTarget(5,0,32,700)
    k.Initialize()
        
    passed = 0
    total  = 0
        
    while k.ProcessEvent():
        wf_v=k.WaveformArray()
        out = fememu.Process(wf_v)
        total += 1
        if (out.time >= 0):
            passed += 1
        print('Run',k.run(),'SubRun',k.subrun(),'Event',k.event(),'Trigger time:',out.time,'Process time',fememu.AverageProcessTime())


    fout.write('%f %i %i\n'%(pe,passed,total))

fout.close()
