/*
 *  RNG.hxx
 *  
 *
 *  Created by Jonathan PASSERAT-PALMBACH on 1/31/11.
 *  Copyright 2011 ISIMA/LIMOS. All rights reserved.
 *
 */

#ifndef RNG_HXX
#define RNG_HXX

#include <boost/concept_check.hpp>
#include "RNGAlgorithm.hxx"

template <
         class T, 
         template <class> class Algo
         >
class RNG : public Algo<T> {
   
public:
	
	__device__
	RNG ( ParameterizedStatus<Algo >* ps)
		:Algo<T>(ps) 
	{}
	
	typedef typename Algo<T>::ParameterizedStatusType ParameterizedStatusType;
	
   // declare that class RNG models the Algorithm concept
	BOOST_CONCEPT_ASSERT((RNGAlgorithm< T, Algo >));
   
};

#endif // RNG_HXX
