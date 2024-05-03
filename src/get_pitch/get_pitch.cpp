/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    -1, --thau0=FLOAT         Umbral th0 [default: 41]
    -2, --thau1=FLOAT         Umbral th1 [default: 0.94]
    -3, --thau2=FLOAT         Umbral th2 [default: 0.4]
    -4, --thauz=FLOAT         Umbral thz [default: 1500]
    -5, --thauc=FLOAT         Umbral thc [default: 0.0061]

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  
  float t0 = stof(args["--thau0"].asString());
  float t1 = stof(args["--thau1"].asString());
  float t2 = stof(args["--thau2"].asString());
  float tz = stof(args["--thauz"].asString());
  float tc = stof(args["--thauc"].asString());

  float aux = -t0;

  //std::cout << "Th0: " << aux << "\nTh1: " << t1 << "\nTh2: " << t2 << endl;

  // float t0 = -40;
  // float t1 = 0.9;
  // float t2 = 0.5;

  // Read input sound file
  unsigned int rate;
  
  vector<float> x; //vector no diezmado

  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int N = 10; //factior diezmado

  // rate = rate/N;

  
  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;
  
  // int size2 = (int) y.size()/N;
  // vector<float> x(size2); //vector diezmado

  // for (unsigned int i = 0 ; i < y.size(); i++){
  //   if(i%N==0){
  //     x[i/N] = y[i];
  //   }
  // }
  // Define analyzer
  PitchAnalyzer analyzer(aux,t1,t2,tz,tc,n_len, rate, PitchAnalyzer::RECT, 50, 500);
  // analyzer.th0 = th0;
  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
  //CENTRAL CLIPPING

  


  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
