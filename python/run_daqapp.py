import sys
from fememu_pycfg import apply_config
from ROOT import trigger

#
# Get configuration
#
cfg_file = sys.argv[1]
config = trigger.fememu.FEMBeamTriggerConfig()
if not apply_config(config,cfg_file):
    print '\033[91m[ERROR]\033[00m exiting...'
    sys.exit(1)

#
# Create FEMBeamTrigger algorithm w/ configuration
#
fememu = trigger.fememu.FEMBeamTriggerAlgo(config)

k=trigger.DAQFileInterface()
k.Reset()

for x in xrange(len(sys.argv)-2):
    print 'Input:',sys.argv[x+2]
    k.AddInputFile(sys.argv[x+2])
k.SetTarget(4,0,32)
k.Initialize()

while 1:
    k.ProcessEvent()
    wf_v=k.WaveformArray()

    out = fememu.Process(wf_v)
    print 'Run',k.run(),'SubRun',k.subrun(),'Event',k.event(),'Trigger time:',out.time

    
