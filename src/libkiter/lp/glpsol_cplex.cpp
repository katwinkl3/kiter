/*
 * glpsol_cplex.cpp
 *
 *  Created on: 22 Oct. 2020
 *      Author: toky
 */


#include <lp/glpsol.h>

#ifdef __CPLEXLIB__
#include <ilcplex/ilocplex.h>
#else
//#warning "You don't have Cplex installed."
#endif


bool            commons::GLPSol::solveWithCplex      () {

#ifdef __CPLEXLIB__

	IloEnv env;
	IloModel model(env);

	IloNumVarArray var(env);
	IloRangeArray con(env);
	populatebyrow (model, var, con);

	IloCplex cplex(model);
	cplex.solve();

	env.out() << "Solution status = " << cplex.getStatus() << endl;
	env.out() << "Solution value  = " << cplex.getObjValue() << endl;





#endif

	VERBOSE_ERROR("CPLEX not found");
	return this->solve();
}
