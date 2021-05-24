/*
 * glpsol_gurobi.cpp
 *
 *  Created on: 22 Oct. 2020
 *      Author: toky
 */


#include <lp/glpsol.h>

#ifdef __GUROBILIB__
#include <cstdio>
#include "gurobi_c++.h"
#else
//#warning "You don't have Gurobi installed."
#endif

// TODO : Terrible to write the modle with gurobi, need to do differently !
bool            commons::GLPSol::solveWithGurobi      () {
#ifdef __GUROBILIB__

	VERBOSE_ILP("Gurobi: Preprocess");
	generateGLPKProblem();


	// Ecriture du probleme
	if (VERBOSE_IS_ILP())   {writeProblem();}

	const std::string mpsname = commons::toString<const char*>(glp_get_prob_name(lp)) + ".mps";

	glp_write_mps (lp,GLP_MPS_DECK,0,mpsname.c_str());

	VERBOSE_ILP("Gurobi: Read");

	static GRBEnv *env;
	try {
		env = new GRBEnv();
	} catch (GRBException& error) {
		std::remove(mpsname.c_str());
		VERBOSE_ERROR("new GRBEnv() failed: " << error.getMessage());
	}
	GRBVar *vars = 0;
	if (not VERBOSE_IS_ILP())  {env->set(GRB_IntParam_OutputFlag, 0);}
    GRBModel model = GRBModel(*env, mpsname);
    if (not VERBOSE_IS_ILP())  {model.set(GRB_IntParam_OutputFlag, 0);}

	if (model.get(GRB_IntAttr_IsMIP) == 0) {
		std::remove(mpsname.c_str());
		VERBOSE_ERROR("Model is not a MIP");
	}

	model.set(GRB_IntParam_Presolve, 1);
	VERBOSE_ILP("Gurobi: Optimize");

	model.optimize();

	int optimstatus = model.get(GRB_IntAttr_Status);

	VERBOSE_ILP("Gurobi: Optimization complete");

	double objval = 0;
	if (optimstatus == GRB_OPTIMAL) {
		objval = model.get(GRB_DoubleAttr_ObjVal);
		VERBOSE_ILP("Optimal objective: " << objval );
	} else if (optimstatus == GRB_INF_OR_UNBD) {
		VERBOSE_ILP("Model is infeasible or unbounded");
		std::remove(mpsname.c_str());
		return false;
	} else if (optimstatus == GRB_INFEASIBLE) {
		VERBOSE_ILP("Model is infeasible" );
		std::remove(mpsname.c_str());
		return false;
	} else if (optimstatus == GRB_UNBOUNDED) {
		VERBOSE_ILP("Model is unbounded" );
		std::remove(mpsname.c_str());
		return false;
	} else {
		VERBOSE_ILP("Optimization was stopped with status = " << optimstatus );
	}

	int numvars = model.get(GRB_IntAttr_NumVars);
	vars = model.getVars();


	VERBOSE_ILP("Gurobi: feed columns");
	for (int iColumn=0;iColumn<numvars;iColumn++) {
	    	  GRBVar v = vars[iColumn];
	      	  const char * colname =  glp_get_col_name(lp,iColumn + 1);
	      	  columnsMap[colname].found  = v.get(GRB_DoubleAttr_X);
	      	  columnsMap[colname].Ifound = v.get(GRB_DoubleAttr_X);

	}

	std::remove(mpsname.c_str());
	delete[] vars;
	return true;
#endif


	VERBOSE_ERROR("Gurobi not found");
	return this->solve();
}
