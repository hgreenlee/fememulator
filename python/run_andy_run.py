#!/usr/bin/env python
import sys
from fememu_pycfg import apply_config
from ROOT import trigger
trigger.fememu

#
# Create FEMBeamTrigger algorithm w/ configuration
#

k=trigger.DAQFileInterface()
k.Reset()

for x in xrange(len(sys.argv)-1):
    print 'Input:',sys.argv[x+1]
    k.AddInputFile(sys.argv[x+1])
k.SetTarget(5,0,32,700)
k.Initialize()

#============================================
# Configure Algorithms
# We need a fhl parser
algos = trigger.MultiAlgo()
# trigger_bits
ext = 0x200
bnb = 0x800
# Declare Algos
algos.declareAlgo( ext, "FEMBeamTriggerAlgo", "swtrg_ext" )
algos.declareAlgo( bnb, "FEMBeamTriggerAlgo", "swtrg_bnb" )
# Set parameters
# BNB
algos.GetConfig("swtrg_bnb").Set("Verbosity",3,true)
algos.GetConfig("swtrg_bnb").Set("MinReadoutTick",500,true)
algos.GetConfig("swtrg_bnb").Set("Discr0precount",1,true)
algos.GetConfig("swtrg_bnb").Set("Discr0delay",4,true)
algos.GetConfig("swtrg_bnb").Set("Discr0threshold",5,true)
algos.GetConfig("swtrg_bnb").Set("Discr0deadtime",6,true)
algos.GetConfig("swtrg_bnb").Set("Discr3delay",4,true)
algos.GetConfig("swtrg_bnb").Set("Discr3threshold",10,true)
algos.GetConfig("swtrg_bnb").Set("Discr3deadtime",6,true)
algos.GetConfig("swtrg_bnb").Set("Discr3width",6,true)
algos.GetConfig("swtrg_bnb").Set("Discr3WindowStart",54,true)
algos.GetConfig("swtrg_bnb").Set("Discr3WindowSize",105,true)
algos.GetConfig("swtrg_bnb").Set("TriggerThresMult",1,true)
algos.GetConfig("swtrg_bnb").Set("TriggerThresPHMAX",70,true)
algos.GetConfig("swtrg_bnb").Set("TriggerWindowStart",57,true)
algos.GetConfig("swtrg_bnb").Set("TriggerWindowSize",104,true)
algos.GetConfig("swtrg_bnb").Set("PrescaleFactor",1.0,true)

algos.GetConfig("swtrg_ext").Set("Verbosity",3,true)
algos.GetConfig("swtrg_ext").Set("MinReadoutTick",500,true)
algos.GetConfig("swtrg_ext").Set("Discr0precount",1,true)
algos.GetConfig("swtrg_ext").Set("Discr0delay",4,true)
algos.GetConfig("swtrg_ext").Set("Discr0threshold",5,true)
algos.GetConfig("swtrg_ext").Set("Discr0deadtime",6,true)
algos.GetConfig("swtrg_ext").Set("Discr3delay",4,true)
algos.GetConfig("swtrg_ext").Set("Discr3threshold",10,true)
algos.GetConfig("swtrg_ext").Set("Discr3deadtime",6,true)
algos.GetConfig("swtrg_ext").Set("Discr3width",6,true)
algos.GetConfig("swtrg_ext").Set("Discr3WindowStart",54,true)
algos.GetConfig("swtrg_ext").Set("Discr3WindowSize",105,true)
algos.GetConfig("swtrg_ext").Set("TriggerThresMult",1,true)
algos.GetConfig("swtrg_ext").Set("TriggerThresPHMAX",70,true)
algos.GetConfig("swtrg_ext").Set("TriggerWindowStart",57,true)
algos.GetConfig("swtrg_ext").Set("TriggerWindowSize",104,true)
algos.GetConfig("swtrg_ext").Set("PrescaleFactor",1.0,true)

algos.Configure()

while k.ProcessEvent():
    wf_v=k.WaveformArray()
    out = algos.Process(wf_v)
    print 'Run',k.run(),'SubRun',k.subrun(),'Event',k.event(),'Trigger time:',out.time#,'Process time',algos.AverageProcessTime()

    
