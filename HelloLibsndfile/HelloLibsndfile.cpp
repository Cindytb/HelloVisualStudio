#include <iostream>
#include <sndfile.hh>

int main()
{
	std::cout << "Specify our file strings" << std::endl;
	std::string input_file = "chirp_441.wav";
	std::string output_file = "output.wav";

	std::cout << "Instantiate the soundfile object and open the file" << std::endl;
	SndfileHandle ifile = SndfileHandle(input_file);

	std::cout << "Create array to store our audio" << std::endl;
	float* input_buf = new float[ifile.frames() * ifile.channels()];

	std::cout << "Read the file into the array" << std::endl;
	ifile.readf(input_buf, ifile.frames());

	std::cout << "Dividing each sample by 2, or decreasing by 6 dB" << std::endl;
	for (int i = 0; i < ifile.frames() * ifile.channels(); i++) {
		input_buf[i] /= 2.0;
	}

	std::cout << "Open output file" << std::endl;
	SndfileHandle ofile = SndfileHandle(output_file, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, ifile.channels(), ifile.samplerate());

	std::cout << "Write to output. For this example, no error checking." << std::endl;
	size_t count = ofile.write(input_buf, ifile.frames());

	std::cout << "Delete our allocated memory" << std::endl;
	delete[] input_buf;
}