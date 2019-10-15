/*
 * repetition_vector.cpp
 *
 *  Created on: 22 avr. 2013
 *      Author: toky
 */



#include <boost/math/common_factor_rt.hpp>  // for boost::math::gcd, lcm
#include <algorithms/repetition_vector.h>
#include <models/Dataflow.h>




bool calcFractionsConnectedActors(models::Dataflow *from, std::map<Vertex,EXEC_COUNT_FRACT> &fractions, Vertex a, TOKEN_UNIT ratePeriod) {

    EXEC_COUNT_FRACT fractionA = fractions[a];

    // Inconsistent graph?
    if (fractionA == EXEC_COUNT_FRACT(0,1)) {
    	VERBOSE_ERROR("fractionA == EXEC_COUNT_FRACT(0,1)");
        return false;
    }

    // Calculate the rate for each actor 'b' connected to actor 'a'
    {ForConnectedEdges(from,a,c) {
    	Vertex src  = from->getEdgeSource(c);
    	Vertex dest = from->getEdgeTarget(c);

    	Vertex b = (src==a) ? dest : src;

    	// Tokens produced or consumed by actor A in one rate period
    	TOKEN_UNIT rateA = from->getEdgeIn(c) ;
    	rateA = rateA * (ratePeriod / from->getEdgeInPhasesCount(c));

    	// Tokens produced or consumed by actor B in one rate period
    	TOKEN_UNIT rateB =  from->getEdgeOut(c);
    	rateB = rateB * (ratePeriod / from->getEdgeOutPhasesCount(c));

    	if (dest==a) {
    		TOKEN_UNIT tmp = rateB;
    		rateB = rateA;
    		rateA = tmp;
    	}

    	// Graph inconsistent?
    	if (rateA == 0 || rateB == 0)
    	{
    		fractions.clear();
    		VERBOSE_ERROR("rateA == 0 || rateB == 0");
    		   return false;
    	}

    	// Calculate firing rate 'b'
    	EXEC_COUNT_FRACT ratioAB = EXEC_COUNT_FRACT(rateA, rateB);
    	EXEC_COUNT_FRACT fractionB = fractionA * ratioAB;

    	// Known firing rate for 'b'
    	EXEC_COUNT_FRACT knownFractionB = fractions[b];

    	// Compare known and calculated firing rate of 'b'
    	if (knownFractionB != EXEC_COUNT_FRACT(0,1)
    			&& fractionB != knownFractionB)  {
    		// Inconsistent graph, set all fractions to 0
    		fractions.clear();
    		VERBOSE_ERROR("(knownFractionB(" << knownFractionB << ") != EXEC_COUNT_FRACT(0,1) && fractionB(" << fractionB << ") != knownFractionB)");
    		return false;
    	}
    	else if (knownFractionB == EXEC_COUNT_FRACT(0,1))
    	{
    		// Set the firing rate of actor 'b'
    		fractions[b] = fractionB;

    		// Calculate firing rate for all actors connnected to 'b'
    		calcFractionsConnectedActors(from, fractions, b, ratePeriod);

    		// Is graph inconsistent?
    		if (fractions[b] == EXEC_COUNT_FRACT(0,1))
    		{
    		    	VERBOSE_ERROR("(fractions[b] == EXEC_COUNT_FRACT(0,1))");
    		    	return false;
    		}
    	}
    }}
    return true;
}


bool calcRepetitionVector(models::Dataflow *from,std::map<Vertex,EXEC_COUNT_FRACT>& fractions, EXEC_COUNT ratePeriod) {

	std::map<Vertex,EXEC_COUNT> repetitionVector;
	EXEC_COUNT l = 1;

    // Find lowest common multiple (lcm) of all denominators
    {ForEachVertex(from,v) {
    	l = boost::math::lcm(l,fractions[v].denominator());
    }}

    // Zero vector?
    if (l == 0) {
    	VERBOSE_ERROR("Zero vector ?");
    	return false;
    }


    // Calculate non-zero repetition vector
    {ForEachVertex(from,v) {
    	repetitionVector[v] = (fractions[v].numerator() * l) /  fractions[v].denominator();
    }}


    // Find greatest common divisor (gcd)
    EXEC_COUNT g = repetitionVector.begin()->second;

    {ForEachVertex(from,v) {
    	g = boost::math::gcd(g, repetitionVector[v]);
    }}

    VERBOSE_ASSERT(g > 0, TXT_NEVER_HAPPEND);

    // Minimize the repetition vector using the gcd
    {ForEachVertex(from,v) {
    	 repetitionVector[v] = repetitionVector[v] / g;
    }}

    {ForEachVertex(from,v) {
    	 repetitionVector[v] = repetitionVector[v] * ratePeriod;
    }}

    // Workaround for repetition vector issues
    EXEC_COUNT subrate = ratePeriod;
    {ForEachVertex(from,v) {
    	subrate =  boost::math::gcd(subrate, repetitionVector[v] / from->getPhasesQuantity(v));
    }}
    VERBOSE_DEBUG("SubRate = " << subrate);

    {ForEachVertex(from,v) {
    	 repetitionVector[v] = repetitionVector[v] / subrate;
    }}

    VERBOSE_DEBUG("Repetition Vector :");
    {ForEachVertex(from,v) {
    	 from->setNi(v,repetitionVector[v]);
    	 VERBOSE_DEBUG(from->getVertexName(v) << " \t: " << repetitionVector[v] / from->getPhasesQuantity(v) << "\tx " << from->getPhasesQuantity(v) << "\t = " << repetitionVector[v] << " \t(" << ((from->getReentrancyFactor(v) > 0)?"lb":"") << ")");
    }}

    return true;
}


/**
 * getRepetitionVector ()
 * The function calculates and returns the repetition vector of the graph.
 *
 */
bool computeRepetitionVector(models::Dataflow *from) {
	if (from->is_repetition_vector()) return true;
	from->set_read_only();
    std::map<Vertex,EXEC_COUNT_FRACT> fractions;
    {ForEachVertex(from,v) {
    	fractions[v] =  EXEC_COUNT_FRACT(0,1);
    }}
    EXEC_COUNT ratePeriod = 1;

    // Compute period of repetition for rate vectors (dangerous way ...)
    {ForEachEdge(from,c) {
    	ratePeriod = boost::math::lcm(ratePeriod,from->getEdgeInPhasesCount(c));
    	ratePeriod = boost::math::lcm(ratePeriod,from->getEdgeOutPhasesCount(c));
    }}
    VERBOSE_ASSERT(ratePeriod > 0 , TXT_NEVER_HAPPEND);
    VERBOSE_INFO("Rate Period = " << ratePeriod);


    // Calculate firing ratio (as fraction) for each actor
    {ForEachVertex(from,v) {
    	if (fractions[v] == EXEC_COUNT_FRACT(0,1)) {
    		 fractions[v] = EXEC_COUNT_FRACT(1,1);
        	 if (!calcFractionsConnectedActors(from, fractions, v, ratePeriod)) return false;
    	}
    }}

    // Calculate repetition vector based on firing ratios
    bool res =  calcRepetitionVector(from,fractions, ratePeriod);
    if (res) from->set_repetition_vector();
    return res;
}
