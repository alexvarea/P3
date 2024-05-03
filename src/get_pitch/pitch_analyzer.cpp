/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {

    for (unsigned int l = 0; l < r.size(); ++l) {
  		/// \TODO Compute the autocorrelation r[l]
      /// \FET Autocorrelació ***calculada***
      r[l] = 0;
      for (unsigned int n = l; n < x.size(); n++){
        r[l]+=x[n]*x[n-l];
      }
      r[l] /= x.size();
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero 
      r[0] = 1e-10; 
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      /// \TODO Implement the Hamming window
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }


  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float zcr) const {
    /// \TODO Implement a rule to decide whether the sound is voiced or not.
    /// * You can use the standard features (pot, r1norm, rmaxnorm),
    ///   or compute and use other ones.
    /// \FET Detector de sonoridad **Implementado**
    // if((zcr>thz)&&(rmaxnorm<th2)){
    //   return true;
    // }else if((r1norm > th1)||(pot > th0)){
    //   return false;
    // }
    if((pot > th0)&&(((r1norm>th1)||(rmaxnorm > th2))&&zcr<thz)){
      return false;
    } else{
      return true;
    }

    // if (rmaxnorm > th2){
    //   return false;
    // }else if((r1norm > th1)&&(pot > th0)){
    //   return false;
    // }
    //return true;
  }

  float PitchAnalyzer::computezcr(const vector<float> &x, unsigned int fm) const{
     //COMPUTE ZCR
    float sum=0;
    float zcr = 0;
    float N = x.size();
    for(int i=1;i<N;i++){
        if((x[i]<0 && x[i-1]>0) || (x[i]>0 && x[i-1]<0)){//sgn(x[N]) = (x[N] > 0) - (x[N] < 0)
        sum++;
      //  printf("%f\n",sum);
        }
    }
    zcr = fm/(2*(N-1))*sum;
    return zcr;
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;

   
    /// \FET Aplicamos preprocesado center clipping sin offset 

    for (unsigned int i=0; i<x.size() ; ++i){
      
      // if(x[i] > thc){
      //   x[i] = x[i] - thc;
      // }else if(x[i] < -thc){
      //   x[i] = x[i] + thc;
      // }else{
      //   x[i] = 0;
      // }
      if((x[i]>-thc && x[i]<thc)){
        x[i] = 0;
      }

    }

    
    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin(), iRMax = iR;

    /// \TODO IMPORTANTE
	  /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	  /// Choices to set the minimum value of the lag are:
	  ///    - The first negative value of the autocorrelation.
	  ///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
	  /// In either case, the lag should not exceed that of the minimum value of the pitch.
    /// \FET Maxim autocorrelacio **implementat**
    for (iRMax = iR = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++){
      if(*iR>*iRMax){
        iRMax = iR;
      }
    }


    unsigned int lag = iRMax - r.begin();
    float pot = 10 * log10(r[0]);
    float zcr;

    zcr = computezcr(x,samplingFreq);
    
    
    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 0
    if (r[0] > 0.0F)
      //cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << zcr <<endl;
      //cout << zcr << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0],zcr))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
}
