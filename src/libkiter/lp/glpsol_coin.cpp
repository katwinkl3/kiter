/*
 * glpsol_coin.cpp
 *
 *  Created on: 22 Oct. 2020
 *      Author: toky
 */


#include <lp/glpsol.h>

#ifdef __COINLIB__
#include <coin/CbcModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
#else
//#warning "You don't have Coin installed."
#endif


bool commons::GLPSol::solveWithCoin() {


#ifdef __COINLIB__
	generateGLPKProblem();

	// Ecriture du probleme
	if (VERBOSE_IS_ILP())   writeProblem();
	OsiClpSolverInterface solver1;
	solver1.setLogLevel(0);
	const std::string lpname = commons::toString<const char*>(glp_get_prob_name(lp)) + ".lp";
	const std::string mpsname = commons::toString<const char*>(glp_get_prob_name(lp)) + ".mps";
	// Read in example model in MPS file format
	// and assert that it is a clean model
	//this->writeProblem();
	glp_write_mps (lp,GLP_MPS_DECK,0,mpsname.c_str());
	VERBOSE_ASSERT( solver1.readMps(mpsname.c_str(),"")==0,TXT_NEVER_HAPPEND);

	// Pass the solver with the problem to be solved to CbcModel
	CbcModel model(solver1);

	if (VERBOSE_IS_ILP()) {
		model.setLogLevel(4);
		solver1.setLogLevel(4);
	} else {
		model.setLogLevel(0);
		solver1.setLogLevel(0);
	}


	// Do complete search
	model.initialSolve();
	model.branchAndBound();

	if (!(model.isProvenOptimal())) return false;

	/* Print the solution.  CbcModel clones the solver so we
            need to get current copy from the CbcModel */
	int numberColumns = model.solver()->getNumCols();

	const double * solution = model.getCbcColSolution();


	for (int iColumn=0;iColumn<numberColumns;iColumn++) {
		double value=solution[iColumn];
		//double obj = model.getObjCoefficients()[iColumn];
		const char * colname =  glp_get_col_name(lp,iColumn + 1);
		//if (fabs(value)>1.0e-7&&model.solver()->isInteger(iColumn))
		//printf("%s(%d) has value %f for a obj of %f\n",colname,iColumn,value,obj);
		columnsMap[colname].found  = value;
		columnsMap[colname].Ifound = value;

	}
	return true;
#endif

	VERBOSE_ERROR("Coin not found");
	return this->solve();
}
