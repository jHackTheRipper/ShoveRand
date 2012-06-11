/**
 * @file utils.hxx
 *
 * @brief utility program for CUDA implementation of TinyMT.
 *
 * This is utility program for CUDA implementation of TinyMT32.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 * @author jH@CKtheRipper [generic version]
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see LICENSE.txt
 * Modified by jH@CKtheRipper, 11/3/11
 */

#ifndef UTILS_HXX
#define UTILS_HXX

namespace shoverand {
	namespace utils {
		
		template <class UINT_TYPE>
		struct TinyMT {
		public:
		/**
		* This function reads parameter from file and puts them in an array.
		* The file should be the output of tinymtXXdc.
		*
		* @param filename name of the file generated by tinymtXXdc.
		* @param params output array of this function.
		* number of elements of the array is num_params * 3.
		* @param num_param number of parameters
		* @return 0 if normal end.
		* @author Mutsuo Saito
		*/
		static int tinymt_set_params(const char * filename,
					UINT_TYPE* params,
					int num_param);
		
		private:
		
		static const unsigned int BUFF_SIZE = 500;
			
		/**
		* read line from fp and set parametes to mat1, mat2, tmat.
		* The format of the file should be that of tinymtXXdc's output.
		*
		* @param mat1 output mat1 parameter.
		* @param mat2 output mat2 parameter.
		* @param tmat output tmat parameter.
		* @param fp file pointer.
		* @return 0 if normal end.
		*/
		static int read_line(UINT_TYPE* mat1, UINT_TYPE* mat2, UINT_TYPE* tmat, FILE *fp);
		
		};
	} // end of namespace utils
} // end of namespace shoverand


#include "utils.cxx"

#endif // UTILS_HXX
