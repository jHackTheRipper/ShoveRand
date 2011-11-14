/**
 *  @file TinyMT.hxx
 *  @author jH@CKtheRipper
 *
 *  Created by Jonathan PASSERAT-PALMBACH on 1/31/11.
 *  Copyright 2011 ISIMA/LIMOS. All rights reserved.
 *
 */

#ifndef TINYMT_HXX
#define TINYMT_HXX

#include <shoverand/core/ParameterizedStatus.hxx>
#include <shoverand/core/SeedStatus.hxx>

#include "ParameterizedStatus.h"
#include "SeedStatus.h"
#include "utils.hxx"

namespace shoverand {
	namespace prng {	
		namespace TinyMT {

			/** Class statically checking the RNGAlgorithm policy's interface.
			Most of this class' methods are marked as __host__ so that BCCL
			can check their presence. 
			You should use them in device code only, when nothing precised.
			*/
			// TODO force people to use it through RNG<>
			template < class T >
			class TinyMT {
				
				
			public:
				
				// TODO insert in core namespace
				typedef SeedStatusTinyMT 			 SeedStatusType;
				typedef ParameterizedStatusTinyMT ParameterizedStatusType;

				
				// TODO add to Concept Checking
				typedef T result_type;
				typedef T input_type;
				
				
			private:

				ParameterizedStatusType* ps_;
				SeedStatusType* ss_;
									
				long k;
				double p1, p2, u;
				
			public:
				
				/** For test purpose only */
				__host__
				TinyMT(ParameterizedStatusType& ps)
					:ps_(&ps)
				{
				}
				
				__device__
				TinyMT(ParameterizedStatusType* ps)
				:ps_(ps)
				{}
				
				
				/** Init is divided in two parts:
				* 	- first, initialize the different streams for every threadblock
				* 	- second, allocate a SubStream per thread
				* 
				* @deprecated
				* TODO remove this method
				*/
				__host__ __device__
				void init() {

					// create the independent random sequence associated to current thread
					ss_ = new SeedStatusType(ps_);
				}
				
				__host__ __device__
				T next() {
					
					// Component 1
					p1 = a12 * ss_->Cg_[1] - a13n * ss_->Cg_[0];
					k = static_cast<long> (p1 / m1);
					p1 -= k * m1;
					
					if (p1 < 0.0)  p1 += m1;
					
					ss_->Cg_[0] = ss_->Cg_[1]; ss_->Cg_[1] = ss_->Cg_[2]; ss_->Cg_[2] = p1;
					
					// Component 2
					p2 = a21 * ss_->Cg_[5] - a23n * ss_->Cg_[3];
					k = static_cast<long> (p2 / m2);
					p2 -= k * m2;
					
					if (p2 < 0.0) p2 += m2;
					
					ss_->Cg_[3] = ss_->Cg_[4]; ss_->Cg_[4] = ss_->Cg_[5]; ss_->Cg_[5] = p2;
					
					// Combination
					u = ((p1 > p2) ? (p1 - p2) * norm : (p1 - p2 + m1) * norm);
					
					return u;
				}
				
				//TODO The two following methods need to be tuned according to TinyMT's characteristics
				// 	and also to its result type.
				__host__ __device__ inline
				result_type min() const { return result_type(0); }


				// TODO specialize according to internal data type
				__host__ __device__ inline
				result_type max() const { return result_type(1); }
				
				// TODO factor upwards
				__host__ __device__ inline
				result_type operator() () { return this->next(); }

				// concept checking class 
				// (friend allows to expose private methods to this class only)
				template <class U, template <typename> class Algo>
				friend class RNGAlgorithm;

			};

		} // end of namespace TinyMT
	} // end of namespace prng
	
	// shortcut to TinyMT
	using prng::TinyMT::TinyMT;

	
} // end of namespace shoverand

#endif // TINYMT_HXX