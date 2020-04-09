/*
 * glpsol.cpp
 *
 *  Created on: 22 févr. 2011
 *      Author: toky
 */


//#define __COINLIB__
//#define __GUROBILIB__

#include <commons/verbose.h>
#include "glpsol.h"
#include <algorithm>
#ifdef __COINLIB__
#include <coin/CbcModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
#else
//#warning "You don't have Coin installed."
#endif

#ifdef __CPLEXLIB__
#include <ilcplex/ilocplex.h>
#else
//#warning "You don't have Cplex installed."
#endif


#ifdef __GUROBILIB__
#include "gurobi_c++.h"
#else
//#warning "You don't have Gurobi installed."
#endif

namespace commons {


GLPSol::GLPSol (const string s,const Objectif obj) : problemName               (s),
		objectif                  (obj),
		integer                   (false) ,
		bounded                   (false) ,
		coefs_size                (0)     {

	// FIX : schedule open more than one lpsolver
	//VERBOSE_ASSERT(GLPSolCounter == 0,TXT_NEVER_HAPPEND);
	//GLPSolCounter++;

	lp = glp_create_prob();
	glp_set_prob_name(lp,commons::fromString<const char*>(problemName));
	glp_set_obj_dir(lp, objectif);


	/* hide glpk (or verbose it ..) */
	glp_term_hook(GLPSol::hook_printer,NULL);



}

GLPSol::~GLPSol	() {
	if (lp)	glp_delete_prob(lp);
	//GLPSolCounter--;
}
int GLPSol::hook_printer(void * const , const char * const m) {
	std::string mess = m;
	if (mess.find("Warning:") == 0) {
		VERBOSE_WARNING("GLPSol : " << m);
		//VERBOSE_FAILURE();
	}
	replace(mess.begin(), mess.end(), '\n', ' ');
	VERBOSE_ILP(mess);
	return 1;
}

int GLPSol::printer(void * const , const char * const m) {
	std::string mess = m;
	replace(mess.begin(), mess.end(), '\n', ' ');
	VERBOSE_ILP(mess);
	return 1;
}


void GLPSol::callback(glp_tree *tree, void *info){

	VERBOSE_ASSERT((GLPSol *)info,TXT_INVALID_GLPK_USAGE);

	switch(glp_ios_reason(tree)){
	case GLP_IROWGEN: /*printer(info,"request for row generation");*/break;
	case GLP_IBINGO:  printer(info,"better integer solution found");break;
	case GLP_IHEUR:  /* printer(info,"request for heuristic solution");*/break;
	case GLP_ICUTGEN: /*printer(info,"request for cut generation");*/break;
#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 35)
	case GLP_IBRANCH: /*printer(info,"request for branching");*/break;
	case GLP_ISELECT: /*printer(info,"request for subproblem selection");*/break;
	case GLP_IPREPRO: /*printer(info,"request for preprocessing");*/break;
#endif
	default:          printer(info,"unknown request");break;
	}
}

void GLPSol::writeProblem() {
	std::string name = commons::toString<const char*>(glp_get_prob_name(lp)) + ".lp";
#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 29)
	glp_write_lp (lp,0,name.c_str());
#else
	VERBOSE_ERROR(TXT_NO_IMPLEMENTATION);
#endif
}

void GLPSol::writeSolution() {
	std::string name = commons::toString<const char*>(glp_get_prob_name(lp)) + ".sol";
#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 23)
	glp_write_sol (lp,name.c_str());
#else
	VERBOSE_ERROR(TXT_NO_IMPLEMENTATION);
#endif
}



bool GLPSol::isRowExists (const string n) const {
	return rowsMap.find(n) != rowsMap.end();
}
bool GLPSol::isColumnExists (const string n) const {
	return columnsMap.find(n) != columnsMap.end();
}

idx_t             GLPSol::getRowCount         () const {return (idx_t) this->rowsMap.size();}
idx_t             GLPSol::getColumnCount      () const {return (idx_t) this->columnsMap.size();}


bool GLPSol::rowIsReach (const string n) {
	if (bounded) checkBound();
	//VERBOSE_ASSERT (false,TXT_INVALID_GLPK_USAGE << TXT_COLON << n << TXT_SPACE << TXT_NOT_FOUND);
	return rowsMap[n].critical;

}


std::string GLPSol::getColumnName(const idx_t indice)  const {
	for(std::map<std::string,column>::const_iterator it = columnsMap.begin(); it != columnsMap.end() ; it++ )
		if (it->second.indice == indice) return it->second.name;
	throw new std::out_of_range(TXT_NOT_FOUND);
}

bool GLPSol::isInteger() const {
	return this->integer;
}

double 	GLPSol::getValue(const idx_t c) const {
	for(std::map<std::string,column>::const_iterator it = columnsMap.begin(); it != columnsMap.end() ; it++ )
		if (it->second.indice == c) return this->integer?it->second.Ifound:it->second.found;

	throw new std::out_of_range(TXT_NOT_FOUND);
}

bool GLPSol::haveValue (const string c) const {
	return columnsMap.find(c) != columnsMap.end();
}

double GLPSol::getValue	(const string c) const {
	if (haveValue(c)) return columnsMap.at(c).found;
	throw std::out_of_range(TXT_NOT_FOUND);
}
double GLPSol::getRoundedValue (const string c) const {
	return  std::round(getValue(c));
	throw std::out_of_range(TXT_NOT_FOUND);
}
double GLPSol::getIntegerValue	(const string c) const {
	if (haveValue(c)) return columnsMap.at(c).Ifound;
	throw std::out_of_range(TXT_NOT_FOUND);
}


void GLPSol::generateGLPKProblem() {


	VERBOSE_ILP("Solve LP with GLPK...");

	VERBOSE_ILP("Row count: " << this->rowsMap.size());
	VERBOSE_ILP("Col count: " << this->columnsMap.size());



	/* set rows */
	if (this->rowsMap.size() > 0) {
		glp_add_rows(lp, (int) this->rowsMap.size());
		//for (vector<row>::iterator it = rows.begin(); it != rows.end() ; it ++) {
		for(std::map<std::string,row>::iterator it = rowsMap.begin(); it != rowsMap.end() ; it++ ) {
			glp_set_row_name(lp, it->second.indice, commons::fromString<const char*>(it->first));
			glp_set_row_bnds(lp, it->second.indice, it->second.bound.type,  it->second.bound.up, it->second.bound.down);
		}
	}

	/* set columns */
	if (this->columnsMap.size() > 0) {
		glp_add_cols(lp, (int) this->columnsMap.size());
		for(std::map<std::string,column>::iterator it = columnsMap.begin(); it != columnsMap.end() ; it++ ) {
			glp_set_col_name(lp, it->second.indice, commons::fromString<const char*>(it->second.name));
			glp_set_col_bnds(lp, it->second.indice, it->second.bound.type,  it->second.bound.up, it->second.bound.down);
			glp_set_col_kind(lp, it->second.indice, it->second.kind );
			this->integer = this->integer || (it->second.kind != KIND_CONTINUE);
			glp_set_obj_coef(lp, it->second.indice, it->second.coef);
		}
	}

	int matrice_size = this->coefs_size ;

	int* ia = new int[matrice_size + 1] ;
	int* ja = new int[matrice_size + 1] ;
	double* ar = new double[matrice_size + 1] ;

	idx_t cur = 1;
	for (std::map< idx_t , std::map<  idx_t , double >  >::iterator it = this->coefs.begin() ; it != this->coefs.end();it++) {
		for (std::map< idx_t , double >::iterator it2 = it->second.begin() ; it2 != it->second.end();it2++) {
			ia[cur] = it->first;
			ja[cur] = it2->first;
			ar[cur] = it2->second;
			cur++;
		}
	}

	VERBOSE_ASSERT((cur-1) == (idx_t) matrice_size, TXT_ERROR_IN_LP_DEF);
	glp_load_matrix(lp, matrice_size, ia, ja, ar);

	delete[] ia;
	delete[] ja;
	delete[] ar;


	/***
	 *  Methode de resolution d'un programme lineaire
	 */


}

bool GLPSol::solve(const GLPParameters& params) {


	VERBOSE_ILP("Linear solving : Generation.");

	generateGLPKProblem();


	// Ecriture du probleme
	if (VERBOSE_IS_ILP())   writeProblem();


	VERBOSE_ILP("Linear solving : Start.");

#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 33)
	//**** passe SCALE ****
	VERBOSE_ILP("Linear solving : Scaling on.");
	if (params.general_doScale) glp_scale_prob(lp,params.general_scaleMode );
#endif

	bool doInterior = (params.linear_algorithm == IPT);
	bool doSimplex  = (params.linear_algorithm != IPT);
	bool doExact    = (params.linear_algorithm == EXACT);

	//**** passe LINEAR ****

	//parameters
	glp_smcp simplex_parm;
	glp_init_smcp(&simplex_parm);
	simplex_parm.pricing  = params.linear_doSteep?GLP_PT_PSE:GLP_PT_STD;
	simplex_parm.r_test   = params.linear_doRelax?GLP_RT_HAR:GLP_RT_STD;
	simplex_parm.presolve = params.linear_doPresolve;
	simplex_parm.meth     = params.linear_method;
	if (params.limit > 0) simplex_parm.tm_lim = params.limit * 1000;

#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 33)
	// Basis
	if (params.linear_doStdBasis) {
		VERBOSE_ILP("StdBasis");
		glp_std_basis(lp);
	}
	if (params.linear_doAdvBasis) {
		VERBOSE_ILP("advBasis");
		glp_adv_basis(lp,0);
	}

#endif



	if (doInterior) {
		VERBOSE_ILP("IPT");
#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 33)
		const int ret_interior = glp_interior(lp,NULL); //don't use parameter for the moment
		if (ret_interior != 0) {
			VERBOSE_ERROR("InteriorPoint solving failed : Error code " << ret_interior);
			return false;
		}
#else
		return false;
#endif
	} else {
		if (doSimplex) {
			VERBOSE_ILP("Simplex");
			const int ret_simplex = glp_simplex(lp,&simplex_parm);
			if (ret_simplex != 0) {
				VERBOSE_ERROR("simplex solving failed : Error code " << ret_simplex);
				return false;
			}
		}

		/**
		 * last hope !
		 */
		if ((glp_get_status(lp) == GLP_NOFEAS) or (doExact))   {
#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 33)
			const int ret_exact = glp_exact(lp,&simplex_parm);
#else
			const int ret_exact = 1;
			VERBOSE_ERROR("NO EXACT METHOD In THIS VERSION !");
#endif
			if (ret_exact != 0) {
				VERBOSE_ERROR("Exact solving failed: Error code " << ret_exact);
				return false;
			}
		}

	}


	const int status = doInterior?glp_ipt_status(lp):glp_get_status(lp);
	switch (status) {
	case GLP_UNDEF    : printer(NULL,"solution is undefined");printTraces();return false;break;
	case GLP_FEAS     : printer(NULL,"solution is feasible");break;
	case GLP_INFEAS   : printer(NULL,"solution is infeasible");printTraces();return false;break;
	case GLP_NOFEAS   : printer(NULL,"no feasible solution exists");printTraces();return false;break;
	case GLP_OPT      : printer(NULL,"solution is optimal");break;
	case GLP_UNBND    : printer(NULL,"solution is unbounded");break;
	default : printTraces();return false;
	}


	VERBOSE_ILP("Linear solving : Done");


	//***** passe INTEGER ****

	VERBOSE_ILP("Integer solving : glp_init_iocp ...");

	// parameters
	glp_iocp integer_parm;
	glp_init_iocp(&integer_parm);
	integer_parm.cb_func=GLPSol::callback;
	integer_parm.cb_info=this;
#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 33)
	// add branch and bound presolver (in test)
	integer_parm.presolve = params.integer_doPresolve;

	integer_parm.br_tech = params.integer_branchingTechnique; /* GLP_BR_FFV (first fractional variable), GLP_BR_LFV (last fractional variable), GLP_BR_MFV (most fractional variable),
                                                  GLP_BR_DTH (*Driebeck and Tomlin*), GLP_BR_PCH (hybrid pseudocost heuristic) */
	integer_parm.bt_tech = params.integer_backtrackTechnique; /* GLP_BT_DFS, GLP_BT_BFS, *GLP_BT_BLB*, GLP_BT_BPH */
	integer_parm.pp_tech = GLP_PP_ALL;
	integer_parm.gmi_cuts = params.integer_doGMI;
	integer_parm.mir_cuts = params.integer_doMIR;
	integer_parm.cov_cuts = params.integer_doCOV;
	integer_parm.clq_cuts = params.integer_doCLQ;
#endif
	if (params.limit > 0) integer_parm.tm_lim = params.limit * 1000;

	if (this->integer) {

		VERBOSE_ILP("Integer solving : glp_intopt ...");

		const int ret_integer           = glp_intopt(lp,&integer_parm); // solve MIP problem with the branch-and-bound method
		if (ret_integer !=  0) return false; /* Valeurs de retour au nombre de 9 et toujours fruits de problemes */
		int status  = glp_mip_status(lp);
		VERBOSE_ILP("Integer solving : Done.");

		switch (status) {
		case GLP_UNDEF    : printer(NULL,"solution is undefined");printTraces();return false;break;
		case GLP_FEAS     : printer(NULL,"solution is feasible");break;
		case GLP_INFEAS   : printer(NULL,"solution is infeasible");printTraces();return false;break;
		case GLP_NOFEAS   : printer(NULL,"no feasible solution exists");printTraces();return false;break;
		case GLP_OPT      : printer(NULL,"solution is optimal");break;
		case GLP_UNBND    : printer(NULL,"solution is unbounded");break;
		default : printTraces();return false;
		}
	}



	//***** retrieve results ****

	for(int i = 1; i <= glp_get_num_cols(lp);i++) {
		const std::string name = getColumnName(i);
		columnsMap[name].found  = doInterior?glp_ipt_col_prim(lp, i):glp_get_col_prim(lp, i);
		columnsMap[name].Ifound = glp_mip_col_val(lp, i);
	}
	printValues();
	return true;
}
void GLPSol::writeMPSProblem() { // TODO : HUGE BUG FROM GENERRATION TO FIX
	generateGLPKProblem();

	// Ecriture du probleme
	const std::string mpsname = commons::toString<const char*>(glp_get_prob_name(lp)) + ".mps";
	glp_write_mps (lp,GLP_MPS_DECK,0,mpsname.c_str());
}


bool            GLPSol::solveWith      () {
#ifdef __GUROBILIB__
	return solveWithGurobi();
#else
	#ifdef __CPLEXLIB__
		return solveWithCplex();
	#else
		#ifdef __COINLIB__
			return solveWithCoin();
		#else
			return solve();
		#endif
	#endif
#endif
}
bool            GLPSol::solveWithGurobi      () {
#ifdef __GUROBILIB__

	VERBOSE_ILP("Gurobi: Preprocess");
	generateGLPKProblem();


	// Ecriture du probleme
	if (VERBOSE_IS_ILP())   writeProblem();
	const std::string mpsname = commons::toString<const char*>(glp_get_prob_name(lp)) + ".mps";

	glp_write_mps (lp,GLP_MPS_DECK,0,mpsname.c_str());

	VERBOSE_ILP("Gurobi: Read");

	static GRBEnv *env;
	try {
		env = new GRBEnv();
	} catch (GRBException& error) {
		VERBOSE_ERROR("new GRBEnv() failed: " << error.getMessage());
	}
	GRBVar *vars = 0;
	if (not VERBOSE_IS_ILP())  env->set(GRB_IntParam_OutputFlag, 0);
    GRBModel model = GRBModel(*env, mpsname);
    if (not VERBOSE_IS_ILP())  model.set(GRB_IntParam_OutputFlag, 0);

	if (model.get(GRB_IntAttr_IsMIP) == 0) {
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
		return false;
	} else if (optimstatus == GRB_INFEASIBLE) {
		VERBOSE_ILP("Model is infeasible" );
		return false;
	} else if (optimstatus == GRB_UNBOUNDED) {
		VERBOSE_ILP("Model is unbounded" );
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

	delete[] vars;
	return true;
#endif


	VERBOSE_ERROR("Gurobi not found");
	return this->solve();
}
bool            GLPSol::solveWithCplex      () {

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

bool GLPSol::solveWithCoin() {


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

std::pair<double,std::string> GLPSol::compute_row(row c) {
	std::string valueStr = "";
	double value = 0;
	if (this->coefs.find(c.indice) != this->coefs.end()) {
		std::map<   idx_t , double >& ref2map = this->coefs[c.indice];
		for (std::map<  idx_t , double >::iterator it = ref2map.begin() ; it != ref2map.end();it++) {
			value += this->getValue(it->first) * it->second;
			valueStr += " + " + commons::toString<double>(this->getValue(it->first)) + " * " +  commons::toString<double>(it->second);
		}
	}

	return std::pair<double,string>(value,valueStr);
}
void GLPSol::checkBound() {
	VERBOSE_ILP("Compute Bounds ...");
	if (!(this->bounded))
		for(std::map<std::string,row>::iterator it = rowsMap.begin(); it != rowsMap.end() ; it++ ) {
			std::pair<double,std::string> cvals = compute_row(it->second);
			bool bound = false;
			switch (it->second.bound.type) {
			case NO_BOUND 		: VERBOSE_BAD_PROGRAMMING(); break;
			case FIX_BOUND 		: bound = commons::AreSame<double>(cvals.first,it->second.bound.down); break;
			case UP_BOUND 		: bound = commons::AreSame<double>(cvals.first,it->second.bound.up);   break;
			case LOW_BOUND 		: bound = commons::AreSame<double>(cvals.first,it->second.bound.down); break;
			case DOUBLE_BOUND 	: VERBOSE_BAD_PROGRAMMING(); break;
			default				: VERBOSE_BAD_PROGRAMMING(); break;
			}
			it->second.critical = bound;
		}
	VERBOSE_ILP("Compute Bounds is Done");
	bounded = true;
}

void GLPSol::printValues() {
	VERBOSE_DEBUG(" VALUES ");
	for(std::map<std::string,column>::iterator it = columnsMap.begin(); it != columnsMap.end() ; it++ ) {
		VERBOSE_DEBUG(" - " << (it->first) <<  " = " << std::fixed <<  it->second.found << " : " << std::fixed <<  it->second.Ifound);
	}
}
void GLPSol::printBound() {
	checkBound() ;
	for(std::map<std::string,row>::iterator it = rowsMap.begin(); it != rowsMap.end() ; it++ ) {
		std::pair<double,std::string> cvals = compute_row(it->second);
		std::string borne = "";
		switch (it->second.bound.type) {
		case NO_BOUND 		: VERBOSE_BAD_PROGRAMMING(); break;
		case FIX_BOUND 		: borne = " = " + commons::toString<double>(it->second.bound.down); break;
		case UP_BOUND 		: borne = "<= " + commons::toString<double>(it->second.bound.up);   break;
		case LOW_BOUND 		: borne = ">= " + commons::toString<double>(it->second.bound.down); break;
		case DOUBLE_BOUND 	: VERBOSE_BAD_PROGRAMMING(); break;
		default				: VERBOSE_BAD_PROGRAMMING(); break;
		}

		VERBOSE_ILP("(" << (it->second.critical?"*":" ") <<  ")" << glp_get_row_name(lp,it->second.indice) << " : " << cvals.second << " "  << borne);

	}

}
void GLPSol::printTraces() {
	if (!VERBOSE_IS_ILP()) return;

	//don't print trace, it sucks
	return;

#if (GLP_MAJOR_VERSION >= 4) && (GLP_MINOR_VERSION >= 33)
	VERBOSE_ILP("glp get unbnd ray = " << glp_get_unbnd_ray(lp));
#else
	VERBOSE_ILP("glp get unbnd ray = TOO OLD GLPK VERSION");
#endif
	VERBOSE_ILP("Row stats :");
	for (int i = 1 ; i <= glp_get_num_rows(lp) ; i++) {
		VERBOSE_ILP("    *  " << glp_get_row_name(lp,i) << " : "<< glp_get_row_stat(lp,i));
	}
	VERBOSE_ILP("Cols stats :");
	for (int i = 1 ; i <= glp_get_num_cols(lp) ; i++) {
		VERBOSE_ILP("    *  " << glp_get_col_name(lp,i) << " : "<< glp_get_col_stat(lp,i));
	}
}


void   GLPSol::warm_up  () {

	//switch(lpx_warm_up(lp)) {
	//    case LPX_E_OK : VERBOSE_ILP("Warm up OK !"); break;
	//    case LPX_E_EMPTY : VERBOSE_ERROR("the problem has no rows and/or no columns."); break;
	//    case LPX_E_BADB : VERBOSE_ERROR("the LP basis is invalid, because the number of basic variables is not the same as the number of rows.");break;
	//    case  LPX_E_SING: VERBOSE_ERROR("the basis matrix is numerically singular or ill-conditioned.");break;
	//    default : VERBOSE_ERROR("BAD WARMUP RESULT !"); break;
	//}

}



} // end of namespace
