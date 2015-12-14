import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMemulator.so")

fin = sys.argv[1]

tree=rt.TChain("opdigit_pmtreadout_tree")
tree.AddFile(fin)

entry = 0
bytes = tree.GetEntry(entry)

config = rt.fememu.BasicTriggerConfig()
config.fVerbose = True
config.fSetBeamWindow = True
config.fDiscr0delay = 4
config.fDiscr3delay = 4
config.fDiscr0threshold = 5
config.fDiscr3threshold = 5
config.fDiscr0precount = 10
config.fDiscr0deadtime = 6
config.fDiscr3deadtime = 6
config.fDiscr0width = 6
config.fDiscr3width = 6
config.fMinReadoutTicks = 500
config.fFrontBuffer = 0
config.fBeamWinSize = 103
config.fBeamWinStartTick = 300

print "window size, front buffer"
print config.fBeamWinSize, config.fFrontBuffer

while bytes>0:
    digit_array=tree.opdigit_pmtreadout_branch

    wfms = rt.fememu.WaveformStore()
    for iwfm in range(0,digit_array.size()):
        wfm = digit_array.at(iwfm)
        if wfm.ChannelNumber()<32 and wfm.size()>config.fMinReadoutTicks:
            wfms.add( wfm.ChannelNumber(), wfm )
    

    out = rt.fememu.basicTrigger( config, wfms )

    entry += 1
    tree.GetEntry( entry )
    if entry==10:
        break

#print digit_array.size()

