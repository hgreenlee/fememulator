import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMemulator.so")

fin = sys.argv[1]

#product_key = "opdigit_pmtreadout"
product_key = "fifo_pmt_xmit"

tree=rt.TChain("%s_tree" % product_key)
tree.AddFile(fin)

nentries = tree.GetEntries();

config = rt.fememu.FEMBeamTriggerConfig()
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
config.fWindowSize = 103

print "window size, front buffer"
print config.fWindowSize, config.fFrontBuffer
print "Looping over",nentries,"entries..."
algo = rt.fememu.LLInterface()
algo.Reset(config)

br = None
for entry in xrange(nentries):

    print "\033[93m[Entry\033[00m",entry
    
    tree.GetEntry(entry)

    exec('br = tree.%s_branch' % product_key)

    if not br:
        sys.stderr.write('\nFailed to fetch a product branch %s\n' % product_key)
        raise Exception

    # If this is fifo type, only use module address 4... (HACK)
    try:
        if br.size() and not br[0].module_address() == 4: continue

    except Exception:
        pass

    result = algo.Emulate(br)
    print

#print digit_array.size()

