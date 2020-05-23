#pragma once

#include <iostream>
#include <sndfile.hh>
#include <fftw3.h>

// Helper function to find next power of 2 using bit twiddling
int next_pow_2(int x) {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

// Helper function for complex multiplcation into array A
void pointwiseMultiplication(fftwf_complex* a, fftwf_complex* b, int size) {
	for (int i = 0; i < size; i++) {
		fftwf_complex temp;
		temp[0] = a[i][0];
		temp[1] = a[i][1];
		a[i][0] = temp[0] * b[i][0] - temp[1] * b[i][1];
		a[i][1] = temp[0] * b[i][1] + temp[1] * b[i][0];
	}
}
int main() {
	std::cout << "Our input/output filenames" << std::endl;
	std::string input = "chirp_441.wav";
	std::string reverb = "reverb_mono_441.wav";
	std::string output = "output.wav";

	std::cout << "Instantiate the soundfile object and open the input/reverb file" << std::endl;
	SndfileHandle ifile = SndfileHandle(input);
	SndfileHandle rfile = SndfileHandle(reverb);
	if (ifile.channels() != 1 || rfile.channels() != 1) {
		std::cout << "ERROR: Only taking mono files for this example" << std::endl;
		exit(EXIT_FAILURE);
	}
	int padded_length = next_pow_2(ifile.frames() + rfile.frames());
	std::cout << "Create special FFTW3 array to store our audio. Adding 2 to do in-place FFT" << std::endl;
	float* input_buf = fftwf_alloc_real((size_t)padded_length + 2);
	float* reverb_buf = fftwf_alloc_real((size_t)padded_length + 2);

	std::cout << "Filling arrays with input audio and padding with zeros" << std::endl;
	ifile.readf(input_buf, ifile.frames());
	for (int i = ifile.frames(); i < padded_length; i++) {
		input_buf[i] = 0.0f;
	}
	rfile.readf(reverb_buf, rfile.frames());
	for (int i = rfile.frames(); i < padded_length; i++) {
		reverb_buf[i] = 0.0f;
	}

	std::cout << "Creating FFT plans" << std::endl;
	fftwf_plan in_plan = fftwf_plan_dft_r2c_1d(padded_length, input_buf, (fftwf_complex*)input_buf,
		FFTW_ESTIMATE);
	fftwf_plan out_plan = fftwf_plan_dft_c2r_1d(padded_length, (fftwf_complex*)input_buf, input_buf,
		FFTW_ESTIMATE);

	std::cout << "Transforming inputs" << std::endl;
	fftwf_execute(in_plan);
	fftwf_execute_dft_r2c(in_plan, reverb_buf, (fftwf_complex*)reverb_buf);

	std::cout << "Multiplying in the frequency domain" << std::endl;
	pointwiseMultiplication((fftwf_complex*)input_buf, (fftwf_complex*)reverb_buf, padded_length / 2 + 1);

	std::cout << "Transforming back into the time domain" << std::endl;
	fftwf_execute(out_plan);

	std::cout << "Peak normalizing the audio (because this is reverb, not a filter)" << std::endl;
	float max = 0;
	for (int i = 0; i < padded_length; i++) {
		if (fabs(input_buf[i]) > max) {
			max = fabs(input_buf[i]);
		}
	}
	for (int i = 0; i < padded_length; i++) {
		input_buf[i] /= max;
	}

	std::cout << "Open output file" << std::endl;
	SndfileHandle ofile = SndfileHandle(output, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, 1, ifile.samplerate());

	std::cout << "Write to output. For this example, no error checking." << std::endl;
	size_t count = ofile.write(input_buf, ifile.frames() + rfile.frames() - 1);

	std::cout << "Cleanup" << std::endl;
	fftwf_free(input_buf);
	fftwf_free(reverb_buf);
	fftwf_destroy_plan(in_plan);
	fftwf_destroy_plan(out_plan);
}