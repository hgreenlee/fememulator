from ROOT import TEfficiency
import numpy as np
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 16})
fig = plt.figure(figsize=(10,6),facecolor='w')

Thresh = []
YES = []
TOT = []

fin = open('results.txt','r')

for line in fin:
    words = line.split()
    Thresh.append(int(20*float(words[0]))/20.)
    YES.append(int(words[1]))
    TOT.append(int(words[2]))

fin.close()
    
SW = []
SWUpper = []
SWLower = []

k=TEfficiency()
k.SetStatisticOption(TEfficiency.kFFC)

for x in range(len(YES)):
    
    yes  = YES[x]
    tot  = TOT[x]

    eff_mean  = float(yes)/float(tot)
    eff_upper = k.FeldmanCousins(tot,yes,0.6827,True)
    eff_lower = k.FeldmanCousins(tot,yes,0.6827,False)

    SW.append(eff_mean)
    SWUpper.append(eff_upper - eff_mean)
    SWLower.append(eff_mean - eff_lower)


    print('iteration %i'%x)

plt.errorbar(Thresh,SW,yerr=[SWUpper,SWLower],fmt='o-',color='b',markersize=7,label='Software Trig. Offline Analysis')
plt.grid()
plt.legend(numpoints=1)
plt.ylim([-0.1,1.1])
#plt.xlim([0,11])
#plt.yscale('log')
plt.xlabel('Effective PE Threshold [ADC/20]')
plt.ylabel('PMT Trigger Firing Frac.')
plt.title('PMT Trig. Emulator Validation [ Multiplicity = 1 ]')
#plt.savefig('SWTrig_Eff_Jan18.png')
plt.show()
