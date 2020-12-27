/*
 * glpsol.h
 *
 *  Created on: 22 févr. 2011
 *      Author: toky
 *
 *      inspired by http://www.cs.unb.ca/~bremner//teaching/LP/examples/,
 *      but really different ...
 *
 *      Usage :
 *
 *       DECLARATION : commons::GLPSol g = commons::GLPSol("Test",commons::MAX_OBJ);
 *       ADD ROW :     g.addRow(commons::row("p",commons::bound_s(commons::UP_BOUND,100)));
 *       ADD COLUMN :  g.addColumn(commons::column("x1",commons::CONTINUE,commons::bound_s(commons::LOW_BOUND,0),10));
 *
 *       ADD COEF :    g.addCoef("p","x1", 1 );
 *
 *       SOLVE :       bool sol = g.solve();
 *       GET SOL:      g.getValue("x1")
 *
 *
 *
 */


#ifndef SRC_LIBKITER_COMMONS_GLPSOL_H_
#define SRC_LIBKITER_COMMONS_GLPSOL_H_

#include <glpk.h>
#include <vector>
#include <map>
#include <stdexcept>
#include <commons/verbose.h>

/*** FIX FOR ECLIPSE ****/
#ifndef __DBL_MAX__
#define __DBL_MAX__ std::numeric_limits<double>::max()
#endif


/** Crazy fix **/
#ifndef GLP_BR_PCH
#define GLP_BR_PCH 666
#endif
#ifndef GLP_SF_GM
#define GLP_SF_GM 666
#endif
#ifndef GLP_SF_EQ
#define GLP_SF_EQ 666
#endif
#ifndef GLP_SF_SKIP
#define GLP_SF_SKIP 666
#endif
#ifndef GLP_SF_AUTO
#define GLP_SF_AUTO 666
#endif
#ifndef GLP_DUAL
#define GLP_DUAL 666
#endif
#ifndef GLP_PP_ALL
#define GLP_PP_ALL 666
#endif


#define _DEFAULT_general_scaleMode             commons::AU_SCALE
#define _DEFAULT_linear_method                 commons::PRIMAL_LINEAR_METHOD
#define _DEFAULT_integer_branchingTechnique    commons::DTH_BRANCH_TECHNIQUE
#define _DEFAULT_integer_backtrackTechnique    commons::BLB_BACKTRACK_TECHNIQUE
#define _DEFAULT_general_doScale               false
#define _DEFAULT_linear_doStdBasis             false
#define _DEFAULT_linear_doAdvBasis             false
#define _DEFAULT_linear_doRelax                false
#define _DEFAULT_linear_doSteep                false
#define _DEFAULT_linear_doPresolve             false
#define _DEFAULT_linear_algorithm              commons::SIMPLEX
#define _DEFAULT_integer_doPresolve            true
#define _DEFAULT_integer_doGMI                 false
#define _DEFAULT_integer_doMIR                 false
#define _DEFAULT_integer_doCOV                 false
#define _DEFAULT_integer_doCLQ                 false
#define _DEFAULT_limit                         0

#define TXT_INVALID_GLPK_USAGE "GLPK error"
#define TXT_ROW_NOT_FOUND(n)    std::string((std::string) "Row \"" + (std::string)  n + (std::string)  "\" not found")
#define TXT_ROW_ALREADY_EXISTS(n)    std::string((std::string) "Row \"" + (std::string)  n + (std::string)  "\" already exists")
#define TXT_NOT_FOUND 			"Not found"
#define TXT_COLUMN_NOT_FOUND(n) std::string((std::string) "Column \"" + (std::string)  n + (std::string)  "\" not found")
#define TXT_ERROR_IN_LP_DEF "Error in the LP definition may happend for example if you have defined more than once the same col in a row."

namespace commons {



using std::vector;
using std::string;
using std::map;
using std::pair;

enum Objectif   {MIN_OBJ = GLP_MIN ,MAX_OBJ = GLP_MAX};
enum ValueKind  {KIND_CONTINUE = GLP_CV ,KIND_BOOLEAN = GLP_BV,  KIND_INTEGER = GLP_IV};
enum BoundType  {NO_BOUND=GLP_FR,  FIX_BOUND = GLP_FX, UP_BOUND = GLP_UP,LOW_BOUND = GLP_LO, DOUBLE_BOUND = GLP_DB};
enum ScaleMode  {
                  GM_SCALE = GLP_SF_GM,     //< perform geometric mean scaling;
                  EQ_SCALE = GLP_SF_EQ,     //< perform equilibration scaling;
                  P2_SCALE = GLP_SF_2N,     //< round scale factors to nearest power of two;
                  SK_SCALE = GLP_SF_SKIP, //< skip scaling, if the problem is well scaled
                  AU_SCALE = GLP_SF_AUTO  //< last hope ?
                };

enum LinearAlgorithm {
                 SIMPLEX,
                 EXACT,
                 IPT
};
enum LinearMethod {
                 PRIMAL_LINEAR_METHOD = GLP_PRIMAL,
                 DUAL_LINEAR_METHOD = GLP_DUAL,
                 DANDP_LINEAR_METHOD = GLP_DUALP
};

enum BranchingTechnique {
    FFV_BRANCH_TECHNIQUE = GLP_BR_FFV,  //< first fractional variable
    LFV_BRANCH_TECHNIQUE = GLP_BR_LFV,  //< last fractional variable
    MFV_BRANCH_TECHNIQUE = GLP_BR_MFV,  //< most fractional variable
    DTH_BRANCH_TECHNIQUE = GLP_BR_DTH,  //< Driebeck and Tomlin
    PCH_BRANCH_TECHNIQUE = GLP_BR_PCH   //< hybrid pseudocost heuristic
};

enum BacktrackTechnique {
    DFS_BACKTRACK_TECHNIQUE = GLP_BT_DFS,  //< depth first search
    BFS_BACKTRACK_TECHNIQUE = GLP_BT_BFS,  //< breadth first search
    BLB_BACKTRACK_TECHNIQUE = GLP_BT_BLB,  //< best local bound
    BPH_BACKTRACK_TECHNIQUE = GLP_BT_BPH   //< best projection heuristic
};


	struct bound_s	{
		BoundType       type;
		double			up;
		double			down;
		bound_s() : type(NO_BOUND) , up (0) , down (0) {};
		template<typename type_coef>
		bound_s(BoundType bt,type_coef i) : type(bt), up((double)i), down((double)i) {VERBOSE_DEBUG_ASSERT(bt != DOUBLE_BOUND,TXT_INVALID_GLPK_USAGE)}

        template<typename type_coef>
        bound_s(BoundType bt,type_coef i, type_coef j) : type(bt), up((double)i), down((double)j) {VERBOSE_DEBUG_ASSERT(bt == DOUBLE_BOUND,TXT_INVALID_GLPK_USAGE)}

	};

	typedef int idx_t;

	struct column {
		idx_t		indice;
		std::string name;
		ValueKind   kind;
		bound_s		bound;
		double		found;
		double		Ifound;
		double		coef;
		column() : indice(0), name(), kind(),bound(),found(0.0),Ifound(0.0),coef(0.0) {FAILED("EMPTY COLUMN CREATED");}
		column(string s,ValueKind k, bound_s b,double coef) : indice(0), name(s), kind(k),bound(b),found(0.0),Ifound(0.0),coef(coef) {}
	};

	struct row {
		idx_t			indice;
		std::string name;
		bound_s		bound;
		bool		critical;
		row()   : indice(0), name(), bound(), critical(false)  {FAILED("EMPTY ROW CREATED");}
		row(string s,bound_s b) : indice(0), name(s), bound(b), critical(false) {}
	};





    /**
     *  define parameters used by GLPSol
     */
    struct GLPParameters {
         ScaleMode              general_scaleMode             ;
         LinearAlgorithm        linear_algorithm              ;
         LinearMethod           linear_method                 ;
         BranchingTechnique     integer_branchingTechnique    ;
         BacktrackTechnique     integer_backtrackTechnique    ;
         bool                   general_doScale               ;
         bool                   linear_doStdBasis             ;
         bool                   linear_doAdvBasis             ;
         bool                   linear_doPresolve             ;
         bool                   linear_doRelax                ;
         bool                   linear_doSteep                ;
         bool                   integer_doPresolve            ;
         bool                   integer_doGMI                 ;
         bool                   integer_doMIR                 ;
         bool                   integer_doCOV                 ;
         bool                   integer_doCLQ                 ;
         int                    limit                         ;

             GLPParameters() :
                  general_scaleMode             (_DEFAULT_general_scaleMode         ),
                  linear_algorithm              (_DEFAULT_linear_algorithm),
                  linear_method                 (_DEFAULT_linear_method             ),
                  integer_branchingTechnique    (_DEFAULT_integer_branchingTechnique),
                  integer_backtrackTechnique    (_DEFAULT_integer_backtrackTechnique),
                  general_doScale               (_DEFAULT_general_doScale           ),
                  linear_doStdBasis             (_DEFAULT_linear_doStdBasis         ),
                  linear_doAdvBasis             (_DEFAULT_linear_doAdvBasis         ),
                  linear_doPresolve             (_DEFAULT_linear_doPresolve         ),
                  linear_doRelax                (_DEFAULT_linear_doRelax            ),
                  linear_doSteep                (_DEFAULT_linear_doSteep            ),
                  integer_doPresolve            (_DEFAULT_integer_doPresolve        ),
                  integer_doGMI                 (_DEFAULT_integer_doGMI             ),
                  integer_doMIR                 (_DEFAULT_integer_doMIR             ),
                  integer_doCOV                 (_DEFAULT_integer_doCOV             ),
                  integer_doCLQ                 (_DEFAULT_integer_doCLQ             ),
                  limit                         (_DEFAULT_limit)
         {}

    };

    //void setDefaultParams   (GLPParameters&);
    //void resetDefaultParams ();
    //GLPParameters     getDefaultParams   ()     ;
    //extern GLPParameters default_glpsol_parameters;

class  GLPSol {
public :

  private:

    glp_prob *lp;




    /* columns and rows */

    std::map<std::string,column>     columnsMap;
    std::map<std::string,row>        rowsMap;

    std::string problemName;
    Objectif    objectif;
    bool        integer;
    bool        bounded;

  private :





    /* utilisé un map est bourrin, mais plus simple */
    std::map< int , std::map< int , double >  > coefs;
    int coefs_size;

  public:
    							GLPSol				(const string s,const Objectif obj);
     							~GLPSol				();
     			inline idx_t addRow  (const string s,const bound_s b) {
     								if(rowsMap.find(s) != rowsMap.end()) {
     									VERBOSE_ERROR(TXT_ROW_ALREADY_EXISTS(s));
     									throw std::logic_error(TXT_ROW_ALREADY_EXISTS(s));
     								};
     						        row nRow (s,b);
     						        nRow.indice = (idx_t) this->rowsMap.size() + 1;
     						        this->rowsMap.insert(std::pair<std::string,row>(s,nRow));
     						        return nRow.indice;
     	         }



                inline idx_t addColumn (const string s,const ValueKind k,const bound_s b,const double coef) {
                    column c(s,k,b,coef);
                    c.indice = (idx_t) this->columnsMap.size() + 1;
                    this->columnsMap.insert(std::pair<std::string,column>(s,c));
                    return c.indice;

                }
    		    inline void fastAddCoef (const idx_t r,const idx_t c,const double v) {
    		        if (!commons::AreSame<double>(v , 0.0)) {
    		            this->coefs[r][c] = v;
    		            this->coefs_size++;
    		        }
    		    }

    		    void addCoef (const string r,const string c,const double v) {
    		        fastAddCoef(getRow(r),getColumn(c), v);
    		    }

    			bool            haveValue            (const string c) const ;
                double          getValue             (const string c) const ;
                double          getRoundedValue      (const string c) const ;
                bool            isInteger            () const;
    			double			getIntegerValue		 (const string c) const;
                bool            solve               (const GLPParameters& params = GLPParameters());
                bool            solveWithCoin       ();
                bool            solveWithCplex      ();
                bool            solveWithGurobi     ();
                bool            solveWith           ();
                void            generateGLPKProblem ();
    			bool			rowIsReach			(const string n);
                idx_t             getRowCount          () const;
                idx_t             getColumnCount       () const;
                void            warm_up             ();
                bool            isRowExists          (const string n)const ;
                bool            isColumnExists       (const string n)const ;
                void            writeMPSProblem     ();
                idx_t getRow (const string n) const {
                	std::map<std::string,row>::const_iterator res = rowsMap.find(n);
                	if (res != rowsMap.end()) return res->second.indice;

                    VERBOSE_ERROR(TXT_ROW_NOT_FOUND(n));
                    throw std::out_of_range(TXT_ROW_NOT_FOUND(n));
                    return -1;
                }
                idx_t getColumn(const string n)  const   {

                	std::map<std::string,column>::const_iterator res = columnsMap.find(n);
                	if (res != columnsMap.end()) return res->second.indice;

                    VERBOSE_ERROR(TXT_COLUMN_NOT_FOUND(n));
                    throw  std::out_of_range(TXT_COLUMN_NOT_FOUND(n));
                    return -1;
                }
  private :

    	static  int             printer             (void * const, const char * const m);
        static  int             hook_printer        (void * const, const char * const m);
				void 			writeProblem		();
				void 			writeSolution		();
    	static 	void 			callback			(glp_tree *tree, void *info);
    			void			printTraces			();
                void            printBound          ();
                void            printValues         ();
    			void			checkBound			();
    			std::string     getColumnName       (const idx_t indice) const;



  private :
    			double 									getValue			(const idx_t c) const;
    			std::pair<double,std::string> 			compute_row(row c);






  };

}//end of namespace



#endif /* SRC_LIBKITER_COMMONS_GLPSOL_H_ */
