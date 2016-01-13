#ifndef UTILFUNC_CXX
#define UTILFUNC_CXX

#include "UtilFunc.h"
#include <unistd.h>
#include "TString.h"
#include <iostream>
namespace ubdaq {

  unsigned int RollOver(unsigned int ref,
			unsigned int subject,
			unsigned int nbits) 
  {
    // Return "ref" which lower "nbits" are replaced by that of "subject"                                      
    // Takes care of roll over effect.                                                                         
    // For speed purpose we only accept pre-defined nbits values.                                              
  
    unsigned int diff=0; // max diff should be (2^(nbits)-2)/2                                                       
    unsigned int mask=0; // mask to extract lower nbits from subject ... should be 2^(nbits)-1                       
    if      (nbits==3) {diff = 3; mask = 0x7;}
    else if (nbits==4) {diff = 0x7; mask = 0xf;}
    //else if (nbits==4) {diff = 0x7; mask = 0xf;}                                                             
    //    else if (nbits==4) {nbits=3; diff = 0x7; mask = 0x7;}                                                
    else {
      std::cerr<<"\033[93m<<ERROR>>\033[00m"<<" Only supported for nbits = [3,4]!"<<std::endl;
      throw std::exception();
    }
  
    subject = ( (ref>>nbits) << nbits) + (subject & mask);
    //std::cout<<ref<<" : "<<subject<<" : "<<nbits<< " : "<<diff<<std::endl;                                   
    // If exactly same, return                                                                                 
    if(subject == ref) return subject;
  
    // If subject is bigger than ref by a predefined diff value, inspect difference                            
    else if ( subject > ref && (subject - ref) > diff) {
    
      // Throw an exception if difference is exactly diff+1                                                    
      if ( (subject - ref) == diff+1 ) {
	std::cerr<<"\033[93m<<ERROR>>\033[00m"<<Form(" Unexpected diff: ref=%d, subject=%d",ref,subject)<<std::endl;
	throw std::exception();
      }

      // Else we have to subtract (mask+1)                                                                     
      else{
	//std::cout<<Form("Correcting %d to %d",subject,(subject-(mask+1)))<<std::endl;                        
	subject = subject - (mask + 1);
      }
    
    }
    // If subject is smaller than ref by a predefined diff value, inspect difference                           
    else if ( subject < ref && (ref - subject) > diff) {
    
      // Throw an exception if difference is exactly diff+1                                                    
      if ( (ref - subject) == diff+1 ) {
	std::cerr<<"\033[93m<<ERROR>>\033[00m"<<Form(" Unexpected diff: ref=%d, subject=%d",ref,subject)<<std::endl;
	throw std::exception();
      }
      else{
	//std::cout<<Form("Correcting %d to %d",subject,(subject + (mask+1)))<<std::endl;                      
	subject = subject + (mask + 1);
      }
    }
    return subject;
  }
}
#endif
