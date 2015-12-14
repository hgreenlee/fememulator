import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMemulator.so")

fin = sys.argv[1]

tree=rt.TChain("opdigit_pmtreadout_tree")
tree.AddFile(fin)

entry = 0
bytes = tree.GetEntry(entry)

config = rt.fememu.FEMBeamTriggerConfig()
config.fVerbose = False
config.fSetTriggerWindow = False
config.fDiscr0delay = 3
config.fDiscr3delay = 3
config.fDiscr0threshold = 5
config.fDiscr3threshold = 10
config.fDiscr0precount = 3
config.fDiscr0deadtime = 6
config.fDiscr3deadtime = 6
config.fDiscr0width = 6
config.fDiscr3width = 6
config.fMinReadoutTicks = 500
config.fTriggerWinStartTick = 0
config.fWindowSize = 103


fememu = rt.fememu.FEMBeamTriggerAlgo()
fememu.Reset( config )



while bytes>0:
    digit_array=tree.opdigit_pmtreadout_branch
    vec = rt.vector("vector<short>")()
    for iwfm in range(0,digit_array.size()):
        #wfm = rt.vector("short")()
        wfm = digit_array.at(iwfm)
        if wfm.ChannelNumber()<32 and wfm.size()>500:
            vec.push_back( wfm )
    out = fememu.Emulate( vec )
    entry += 1
    tree.GetEntry( entry )
    if entry==10:
        break

#print digit_array.size()

