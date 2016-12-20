#pragma once
/*
 *
 * Copyright (c) 2014, Laurens van der Maaten (Delft University of Technology)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Delft University of Technology.
 * 4. Neither the name of the Delft University of Technology nor the names of
 *    its contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LAURENS VAN DER MAATEN ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL LAURENS VAN DER MAATEN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#define ARMA_64BIT_WORD
#include <armadillo>

#include "helpers.hpp"

struct Mat {
	const size_t n_rows;
	const size_t n_cols;
	const size_t num_elements_;
	std::shared_ptr<double> data_;
	double* ptr_;

	Mat(const size_t n_rows, const size_t n_cols) :
			n_rows(n_rows), n_cols(n_cols), num_elements_(n_rows * n_cols) {
		data_ = MallocPtr<double>::NumElements(num_elements_, true);
		ptr_ = data_.get();
	}

	Mat(const std::vector<std::vector<double>>& m) :
			n_rows(m.size()), n_cols(m.front().size()), num_elements_(n_rows * n_cols) {
		data_ = MallocPtr<double>::NumElements(num_elements_, false);
		ptr_ = data_.get();

		for (size_t i = 0; i < n_rows; ++i) {
			const auto& p = m[i];
			std::copy(p.begin(), p.end(), &ptr_[i * n_cols]);
		}
	}
};

struct TSNEArgs{

  double theta_ = 0.5;             /**<  gradient accuracy */
  double perplexity_ = 30;         /**<  perplexity, a guess of how many points per groups */
  uint32_t no_dims_ = 2;           /**<  output dimensionality */
  uint32_t initial_dim_ = 50;      /**<  the number of input columns actually used, normally used in conjunction with PCA to choose first components */
  bool skip_random_init_ = false;
  uint32_t max_iter_ = 1000;       /**< maximum number of iterations */
	uint32_t stop_lying_iter_ = 250;
	uint32_t mom_switch_iter_ = 250;
	uint32_t rand_seed_ = -1;

	bool menaCenterCols_ = true;    /**< whether to mean center the data first, this would be done before PCA if it's being done */
	bool doPca_ = true;							/**< whether to perform PCA first or not on input data */


};

static inline double sign(double x) {
	return (x == .0 ? .0 : (x < .0 ? -1.0 : 1.0));
}

class TSNE {
public:
	Mat run(const Mat& X, const TSNEArgs& params);

	void run(double* X, int N, int D, double* Y, int no_dims, double perplexity,
			double theta, int rand_seed, bool skip_random_init, int max_iter = 1000,
			int stop_lying_iter = 250, int mom_switch_iter = 250);
	bool load_data(double** data, int* n, int* d, int* no_dims, double* theta,
			double* perplexity, int* rand_seed, int* max_iter);
	void save_data(double* data, int* landmarks, double* costs, int n, int d);
	void symmetrizeMatrix(unsigned int** row_P, unsigned int** col_P,
			double** val_P, int N); // should be static!

private:
	void computeGradient(double* P, unsigned int* inp_row_P,
			unsigned int* inp_col_P, double* inp_val_P, double* Y, int N, int D,
			double* dC, double theta);
	void computeExactGradient(double* P, double* Y, int N, int D, double* dC);
	double evaluateError(double* P, double* Y, int N, int D);
	double evaluateError(unsigned int* row_P, unsigned int* col_P, double* val_P,
			double* Y, int N, int D, double theta);
	void zeroMean(double* X, int N, int D);
	void computeGaussianPerplexity(double* X, int N, int D, double* P,
			double perplexity);
	void computeGaussianPerplexity(double* X, int N, int D, unsigned int** _row_P,
			unsigned int** _col_P, double** _val_P, double perplexity, int K);
	void computeSquaredEuclideanDistance(double* X, int N, int D, double* DD);
	double randn();
};
