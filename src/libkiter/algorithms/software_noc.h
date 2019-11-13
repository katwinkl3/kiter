/*
 * kperiodic.h
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */

#ifndef SOFTWARE_NOC_H_
#define SOFTWARE_NOC_H_
typedef long int LARGE_INT;
namespace algorithms {
	inline LARGE_INT myabs(LARGE_INT a)
	{
		if (a < 0)
			return -1*a;
		return a;
	} 
	void software_noc (models::Dataflow* const dataflow, parameters_list_t);
	void software_noc_bufferless (models::Dataflow* const dataflow, parameters_list_t);
	void dynamic_noc (models::Dataflow* const dataflow, parameters_list_t);
	//Find co-efficients a and b such that ax + by = gcd(x, y)
	//LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b);
	bool isConflictPresent(LARGE_INT HP, TIME_UNIT si, LARGE_INT ni, TIME_UNIT sj, LARGE_INT nj);
	void generate_lte_sdf(models::Dataflow* const dataflow, parameters_list_t param_list);
}
#endif // SOFTWARE_NOC_H_
