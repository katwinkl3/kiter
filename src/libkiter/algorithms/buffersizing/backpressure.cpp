/*
 * backpressure.cpp
 *
 *  Created on: 25 avr. 2013
 *      Author: toky
 */

#include <vector>
#include <commons/verbose.h>
#include <commons/glpsol.h>
#include <models/Dataflow.h>
#include <algorithms/normalization.h>
#include <algorithms/buffersizing/backpressure.h>
#include <algorithms/repetition_vector.h>

void algorithms::compute_backpressure_memory_sizing (models::Dataflow* const  dataflow, parameters_list_t ) {


	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - Compute normalized period
    TIME_UNIT PERIOD = dataflow->getPeriod()  ;


	// STEP 0 - Need the repetition vector
	VERBOSE_INFO("STEP 0 :  Need the repetition vector");
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");


	std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> s;

	std::map<Vertex,TIME_UNIT> mu;
	{ForEachVertex(dataflow,vi){
		mu[vi] = (PERIOD / dataflow->getNi(vi)) * dataflow->getPhasesQuantity(vi);
		VERBOSE_DEBUG( "Mu_" << dataflow->getVertexName(vi) << " = " << mu[vi]);
	}}

	// STEP 1 - Generate BURST EXECUTION
	VERBOSE_INFO("STEP 1 :  Burst execution and alpha + lambda computation ");
	{ForEachVertex(dataflow,vi){
		std::string verbose_line = dataflow->getVertexName(vi) + std::string(" s = 0 ");
		const  EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);
		s[std::make_pair(vi,1)] = 0;
		for(EXEC_COUNT k = 2 ; k <= maxk;k++){
			s[std::make_pair(vi,k)] = s[std::make_pair(vi,k-1)] + dataflow->getVertexDuration(vi,k-1);
			verbose_line += std::string(",") + commons::toString(s[std::make_pair(vi,k)]);
		}
		VERBOSE_DEBUG(verbose_line);
	}}


	std::map<Edge,TOKEN_UNIT> lambda_prod;
	std::map<Edge,TOKEN_UNIT> lambda_cons;
	std::map<Edge,TIME_UNIT>  alpha;

	{ForEachEdge(dataflow,e) {

		TOKEN_UNIT lambda_p = dataflow->getEdgeIn(e);
		TOKEN_UNIT lambda_c = dataflow->getEdgeOut(e);

		const EXEC_COUNT max_ki = dataflow->getEdgeInPhasesCount(e);
		const EXEC_COUNT max_kj = dataflow->getEdgeOutPhasesCount(e);

		const Vertex vi = dataflow->getEdgeSource(e);
		const TIME_UNIT alpha_e = (TIME_UNIT) dataflow->getEdgeIn(e) / mu[vi] ; // token by sec


		for (EXEC_COUNT kj = 1 ; kj <= max_kj ; kj++) {
			lambda_p = boost::gcd(lambda_p , dataflow->getEdgeOutPhase(e,kj));
		}
		for (EXEC_COUNT ki = 1 ; ki <= max_ki ; ki++) {
			lambda_c = boost::gcd(lambda_c , dataflow->getEdgeInPhase(e,ki));
		}

		lambda_prod[e]      = lambda_p;
		lambda_cons[e]      = lambda_c;
		alpha[e]            = alpha_e;
		VERBOSE_DEBUG(dataflow->getEdgeName(e) << " : Alpha = " <<  alpha_e << " Lambda in/out = " << lambda_p << "/" << lambda_c);
	}}

	// Lambda and alpha  : Done

	// STEP 2 - compute p_l^e(t) = \alpha_e + \Beta_e^p (courbe production)
	VERBOSE_INFO("STEP 2 : production Beta_e^p computation.");
	std::map<Edge,TIME_UNIT> beta_prod;

	{ForEachEdge(dataflow,e) {
		const TIME_UNIT alpha_e = alpha[e];
		const TOKEN_UNIT lambda_p = lambda_prod[e];
		const Vertex vi         = dataflow->getEdgeSource(e);
		const EXEC_COUNT max_ki = dataflow->getEdgeInPhasesCount(e);

		TIME_UNIT  beta_e = std::numeric_limits<TIME_UNIT>::infinity();
		TOKEN_UNIT cumul_production = 0;

		for (EXEC_COUNT ki = 1 ; ki <= max_ki ; ki++) {
			VERBOSE_DEBUG("Check production at " << ki << " of " << cumul_production << "+" << dataflow->getEdgeInPhase(e,ki));
			cumul_production += dataflow->getEdgeInPhase(e,ki);
			TOKEN_UNIT level = commons::ceil(cumul_production - dataflow->getEdgeInPhase(e,ki),lambda_p) + 1;
			if (cumul_production >= lambda_p) {
				VERBOSE_DEBUG(" -> Good point is " << lambda_p << " ; " << s[std::make_pair(vi,ki)] + dataflow->getVertexDuration(vi,ki) );
				beta_e = std::min ( beta_e ,
				                   (TIME_UNIT)  level - alpha_e * (s[std::make_pair(vi,ki)]  + dataflow->getVertexDuration(vi,ki))
				                   );
				VERBOSE_DEBUG(" -> " << dataflow->getEdgeName(e) << " :  Beta prod = " <<  beta_e);
				break;
			} else {
				VERBOSE_DEBUG(" -> Skip it ");
			}
		}
		beta_prod[e]=beta_e;
		VERBOSE_ASSERT(beta_prod[e]==beta_e,TXT_NEVER_HAPPEND);
	}}



	//step 3 - retiming of starting times
	VERBOSE_INFO("STEP 3 : Retiming.");

	std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> g; // new start time
	std::map<std::pair<Vertex,EXEC_COUNT>,TIME_UNIT> h; // new end time


	{ForEachVertex(dataflow,vi){

		VERBOSE_DEBUG("Retiming of " << dataflow->getVertexName(vi));
		const EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);

		h[std::make_pair(vi,maxk)] = mu[vi];
		{ForOutputEdges(dataflow,vi,e) {
			h[std::make_pair(vi,maxk)] = std::min(h[std::make_pair(vi,maxk)], commons::division(commons::ceil(dataflow->getEdgeIn(e) - dataflow->getEdgeInPhase(e,maxk) + 1,lambda_prod[e]) - beta_prod[e],alpha[e]) );
		}}
		g[std::make_pair(vi,maxk)] = h[std::make_pair(vi,maxk)] - dataflow->getVertexDuration(vi,maxk);
		VERBOSE_DEBUG("Task_duration("<< dataflow->getVertexName(vi) << ","<< maxk << ") = "<< dataflow->getVertexDuration(vi,maxk));
		VERBOSE_DEBUG("burst_start(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << s[std::make_pair(vi,maxk)]);
		VERBOSE_DEBUG("burst_end(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << s[std::make_pair(vi,maxk)] + dataflow->getVertexDuration(vi,maxk));
		VERBOSE_DEBUG("g(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << g[std::make_pair(vi,maxk)]);
		VERBOSE_DEBUG("h(" << dataflow->getVertexName(vi) << ","<< maxk << ") = " << h[std::make_pair(vi,maxk)]);

		for(EXEC_COUNT k = (maxk - 1); k >= 1;k--){
			// print ax +b
			// print h(v)
			h[std::make_pair(vi,k)] = g[std::make_pair(vi,k+1)];
			VERBOSE_DEBUG("Task_duration("<< dataflow->getVertexName(vi) << ","<< k << ") = "<< dataflow->getVertexDuration(vi,k));
			VERBOSE_DEBUG("burst_start(" << dataflow->getVertexName(vi) << ","<< k << ") = " << s[std::make_pair(vi,k)]);
	        VERBOSE_DEBUG("burst_end(" << dataflow->getVertexName(vi) << ","<< k << ") = " << s[std::make_pair(vi,k)] + dataflow->getVertexDuration(vi,k));

	        VERBOSE_DEBUG("     compute h(" << dataflow->getVertexName(vi) << ","<< k << ") = " << h[std::make_pair(vi,k)]);

			{ForOutputEdges(dataflow,vi,e) {
				TOKEN_UNIT cumul_production_km1 = 0;
				for (EXEC_COUNT subk = 1 ; subk < k ; subk++) cumul_production_km1+= dataflow->getEdgeInPhase(e,subk);

				const TOKEN_UNIT m_e_k = commons::ceil(cumul_production_km1 + 1,lambda_prod[e]); // relevant token size
				const TIME_UNIT bound = commons::division(m_e_k - beta_prod[e],alpha[e]);
				VERBOSE_DEBUG("   bound computation is : " << m_e_k <<"-"<< beta_prod[e] << "/" << alpha[e]);
	            VERBOSE_DEBUG("     compute h(" << dataflow->getVertexName(vi) << ","<< k << ") = min (h=" << h[std::make_pair(vi,k)] << ",b=" << bound << ")");
				h[std::make_pair(vi,k)] = std::min (h[std::make_pair(vi,k)] , bound);

			}}

			g[std::make_pair(vi,k)] = h[std::make_pair(vi,k)]  - dataflow->getVertexDuration(vi,k);
	        VERBOSE_DEBUG("g(" << dataflow->getVertexName(vi) << ","<< k << ") = " << g[std::make_pair(vi,k)]);
	        VERBOSE_DEBUG("h(" << dataflow->getVertexName(vi) << ","<< k << ") = " << g[std::make_pair(vi,k)]+ dataflow->getVertexDuration(vi,k));

		}
	}}

	// unretiming
	//{ForEachVertex(dataflow,vi){
	//	const EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);
	//	for (EXEC_COUNT k = 1 ; k <= maxk ; k++) {
	//		g[std::make_pair(vi,k)] = s[std::make_pair(vi,k)];
	//		h[std::make_pair(vi,k)] = g[std::make_pair(vi,1)] + dataflow->getVertexDuration(vi,k);
	//	}
	//}}

	// FIX TIMING
	{ForEachVertex(dataflow,vi){
		const EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);
		if (maxk == 1) {
			g[std::make_pair(vi,1)] = 0;
			h[std::make_pair(vi,1)] = g[std::make_pair(vi,1)] + dataflow->getVertexDuration(vi,1);
		}
	}}

	// print result
	{ForEachVertex(dataflow,vi){
		std::string verbose_line = dataflow->getVertexName(vi) + " retimed g:h = " + commons::toString(g[std::make_pair(vi,1)]) +":" + commons::toString(h[std::make_pair(vi,1)]);
		const EXEC_COUNT maxk = dataflow->getPhasesQuantity(vi);
		s[std::make_pair(vi,1)] = 0;
		for(EXEC_COUNT k = 2 ; k <= maxk;k++){
			verbose_line += std::string(",")+ commons::toString(g[std::make_pair(vi,k)]) + std::string(":")+ commons::toString(h[std::make_pair(vi,k)]);
		}
		VERBOSE_DEBUG(verbose_line);


	}}


	// On dispose à ce stade d'une courbe de production correcte,
	// et d'un sequencement d'execution fidele à cette courbe de production.



	// STEP 2 BIS - compute c_u^e(t) = \alpha_e + \Beta_e^c et p_l_b
	VERBOSE_INFO("STEP 2 BIS : consumption slope computation.");
	std::map<Edge,TIME_UNIT> beta_cons;


	{ForEachEdge(dataflow,e) {
		const TIME_UNIT alpha_e   = alpha[e];
		const Vertex vj           = dataflow->getEdgeTarget(e);
		const EXEC_COUNT max_kj   = dataflow->getEdgeOutPhasesCount(e);

		TOKEN_UNIT cumul_consumption = 0;
		TIME_UNIT beta_sup           = - std::numeric_limits<TIME_UNIT>::infinity();

		TOKEN_UNIT lambda_c = lambda_cons[e];
		TIME_UNIT  t = 0;
		cumul_consumption = 0;
		for (EXEC_COUNT kj = 1 ; kj <= max_kj ; kj++) {
			cumul_consumption += dataflow->getEdgeOutPhase(e,kj);
			t = g[std::make_pair(vj,kj)];
			if (cumul_consumption >= lambda_c)
				if (alpha_e * t + beta_sup < cumul_consumption) {
					beta_sup = (TIME_UNIT) cumul_consumption - alpha_e * t;
				}
		}

		beta_cons[e]      = beta_sup;
		VERBOSE_DEBUG(dataflow->getEdgeName(e) << " :  Beta_cons = " <<  beta_sup  );
	}}


	// Step 5 - compute Beta_{ij}
	VERBOSE_INFO("STEP 5 : beta ij computation.");

	std::map<Edge,TIME_UNIT> beta_ij;

	{ForEachEdge(dataflow,e) {
		const TOKEN_UNIT usable_tokens = commons::floor(dataflow->getPreload(e),dataflow->getFineGCD(e));
		VERBOSE_DEBUG("Preload = " << usable_tokens);
		beta_ij[e] = commons::division(   beta_cons[e] - beta_prod[e]     , alpha[e] ) - usable_tokens;
		VERBOSE_DEBUG(dataflow->getEdgeName(e) << " : Beta_ij = " <<  beta_ij[e]);


	}}


	// Step 6 - flow solving !
	VERBOSE_INFO("STEP 6 : flow solving.");


	// Init GLPK
	//******************************************************************
	commons::GLPSol solver = commons::GLPSol("BackPressure_" + dataflow->getName(),commons::MIN_OBJ);

	// Starting times
	//******************************************************************
	{ForEachVertex(dataflow,pVertex) {
		std::string name = dataflow->getVertexName(pVertex);
		solver.addColumn("s_" + name ,commons::KIND_CONTINUE,commons::bound_s(commons::LOW_BOUND,0),1);

	}}

	// Buffer Constraints
	//******************************************************************
	{ForEachEdge(dataflow,e) {

		const Vertex source   = dataflow->getEdgeSource(e);
		const Vertex target   = dataflow->getEdgeTarget(e);
		const std::string  sourceStr = dataflow->getVertexName(source);
		const std::string  targetStr = dataflow->getVertexName(target);


		const std::string rowname = std::string("Edge_") + commons::toString(dataflow->getEdgeId(e)) + std::string("_") + sourceStr + std::string("_") + targetStr;
		solver.addRow(rowname,commons::bound_s(commons::UP_BOUND, (double) - ( beta_ij[e])));


		if ((source != target)) {
			solver.addCoef(rowname ,"s_" + targetStr    , -1        );
			solver.addCoef(rowname ,"s_" + sourceStr    ,  1        );
		}
	}}


	// Solving
	//******************************************************************

	commons::GLPParameters ilp_params = commons::getDefaultParams();

	ilp_params.general_doScale = true;
	ilp_params.linear_doAdvBasis = true;
	ilp_params.linear_method = commons::DUAL_LINEAR_METHOD;

	VERBOSE_INFO(" -> run solver.");

	bool sol = solver.solve(ilp_params);

	VERBOSE_INFO("[CSDF BackPressure]  Solver result is " << sol);

	if (sol) {
		// ** Good value retreiving method **
		{ForEachVertex(dataflow,v) {
			VERBOSE_INFO(dataflow->getVertexName(v) << " : " << solver.getValue("s_" + dataflow->getVertexName(v)));
		}}
	} else {
		VERBOSE_ERROR("Linear solving fail.");
		return;
	}





	// Step 7 - Compute buffer sizes
	VERBOSE_INFO("STEP 7 : buffer sizing.");
	std::map<Edge,TIME_UNIT> beta_feedback_prod;
	std::map<Edge,TIME_UNIT> beta_feedback_cons;
	DATA_UNIT total_buffer_size = 0;

	{ForEachEdge(dataflow,e) {


		const Vertex vi   = dataflow->getEdgeSource(e);
		const Vertex vj   = dataflow->getEdgeTarget(e);
		const std::string  sourceStr = dataflow->getVertexName(vi);
		const std::string  targetStr = dataflow->getVertexName(vj);

		const TIME_UNIT si = solver.getValue("s_" + dataflow->getVertexName(vi));
		const TIME_UNIT sj = solver.getValue("s_" + dataflow->getVertexName(vj));
		const EXEC_COUNT max_ki   = dataflow->getEdgeInPhasesCount(e);
		const EXEC_COUNT max_kj   = dataflow->getEdgeOutPhasesCount(e);
		TOKEN_UNIT lambda_p = lambda_cons[e];
		TOKEN_UNIT lambda_c = lambda_prod[e];
		TOKEN_UNIT cumul_production  = 0;
		TIME_UNIT  t;
		TIME_UNIT  beta_f_prod = std::numeric_limits<TIME_UNIT>::infinity();
		TIME_UNIT alpha_e = alpha[e];
		// compute beta prod (by vj)  in feedback buffer
		for (EXEC_COUNT kj = 1 ; kj <= max_kj ; kj++) {
			t = sj + h[std::make_pair(vj,kj)];
			cumul_production += dataflow->getEdgeOutPhase(e,kj);
			TOKEN_UNIT level = cumul_production+1 - dataflow->getEdgeOutPhase(e,kj) ;
			if (cumul_production >= lambda_p)
				if (alpha_e * t + beta_f_prod > level ) {
					beta_f_prod = (TIME_UNIT) level  - alpha_e * t;
				}
		}

		TIME_UNIT  beta_f_cons = - std::numeric_limits<TIME_UNIT>::infinity();
		TOKEN_UNIT cumul_consumption  = 0;
		// compute beta cons (by vi)  in feedback buffer
		for (EXEC_COUNT ki = 1 ; ki <= max_ki ; ki++) {
			cumul_consumption += dataflow->getEdgeInPhase(e,ki);
			t = si + g[std::make_pair(vi,ki)];
			if (cumul_consumption >= lambda_c)
				if (alpha_e * t + beta_f_cons < cumul_consumption) {
					beta_f_cons = (TIME_UNIT)  cumul_consumption - alpha_e * t;
				}
		}
		beta_feedback_prod[e] =  beta_f_prod;
		beta_feedback_cons[e] = beta_f_cons;

		const TIME_UNIT bsize_beta =   - (beta_f_prod - beta_f_cons);
		const TOKEN_UNIT bsize = commons::ceil(bsize_beta,dataflow->computeFineGCD(e));
		total_buffer_size += bsize;

        VERBOSE_INFO(dataflow->getEdgeName(e) << " :" << bsize_beta << " => " << bsize );
		VERBOSE_DEBUG(dataflow->getEdgeName(e) << " : " << std::setw(10) << beta_f_cons << "   -   "  << std::setw(10)<< beta_f_prod<<"   =   "  << std::setw(10)<< bsize_beta << " ceiled by "<< dataflow->computeFineGCD(e) << " ==> "  << std::setw(10)<< bsize );
	}}

	VERBOSE_INFO("Loopback buffers : " << dataflow->getVerticesCount());
	std::cout << "Total buffer size : " << total_buffer_size
			<< " + 2 * " << dataflow->getVerticesCount() << " = "
			<< total_buffer_size + 2 * dataflow->getVerticesCount() << std::endl ;

	if (VERBOSE_IS_DEBUG()) {
		VERBOSE_INFO("Interlude 1 : print production file for each buffer");

		{ForEachEdge(dataflow,e) {
			const Vertex     vi     = dataflow->getEdgeSource(e);
			const EXEC_COUNT max_ki = dataflow->getEdgeInPhasesCount(e);

			const Vertex     vj     = dataflow->getEdgeTarget(e);
			const EXEC_COUNT max_kj = dataflow->getEdgeOutPhasesCount(e);

			//const TIME_UNIT si = solver.getValue("s_" + dataflow->getVertexName(vi));
			//const TIME_UNIT sj = solver.getValue("s_" + dataflow->getVertexName(vj));

			// write channel file
			std::ofstream myfile;
			myfile.open ( (dataflow->getEdgeName(e) + std::string("_") + dataflow->getVertexName(vi) + ".plot").c_str());
			// write burst
			TIME_UNIT time = 0;
			TOKEN_UNIT token = 0;
			myfile << "set key on outside left box title 'Légende'\nset yrange [0:"  <<  std::max(dataflow->getEdgeIn(e),dataflow->getEdgeOut(e))<< "]\n";
			myfile << "set style line 1 lt 2 lw 1 pt 2 linecolor rgb \"red\"\n";
			myfile << "set style line 2 lt 2 lw 1 pt 2 linecolor rgb \"green\"\n";
			myfile << "set style line 3 lt 1 lw 1 pt 1 linecolor rgb \"green\"\n";
			myfile << "set style line 4 lt 1 lw 2 pt 1 linecolor rgb \"blue\"\n";
			myfile << "set style line 5 lt 2 lw 2 pt 1 linecolor rgb \"orange\"\n";
			myfile << "set style line 6 lt 1 lw 1 pt 2 linecolor rgb \"magenta\"\n";
			myfile << "set style line 7 lt 1 lw 1 pt 1 linecolor rgb \"orange\"\n";
			myfile << "set style line 8 lt 1 lw 4 pt 1 linecolor rgb \"black\"\n";
			myfile      << "plot \"-\" w points  ls 1  title 'Burst prod' ,"
					<< "\"-\" w linespoints  ls 2  title 'Retimed prod', "
					<<  alpha[e]  << " * x + " << beta_prod[e] << " w l ls 3  title 'prod e' , "
					<<  alpha[e]  << " * x + " << beta_cons[e] << " w l ls 5 title 'cons e', "
					<< "\"-\" w linespoints  ls 7  title 'Retimed cons' ,"
					<< "\"-\" w linespoints  ls 8  title 'BETA IJ :" << beta_ij[e]  << "'"
					<< " \n";
			// Courbe de production BURST
			token = 0;
			for(EXEC_COUNT k = 1 ; k <= max_ki;k++){
				time = s[std::make_pair(vi,k)];
				myfile << time << " " << token << "\n";
				time = s[std::make_pair(vi,k)] + dataflow->getVertexDuration(vi,k);
				myfile << time << " " << token << "\n";
				token += dataflow->getEdgeInPhase(e,k);
				myfile << time << " " << token << "\n";
			}
			myfile << "e\n";
			// Courbe de production RETIMING
			token = 0;
			for(EXEC_COUNT k = 1 ; k <= max_ki;k++){
				time = g[std::make_pair(vi,k)];
				myfile << time << " " << token << "\n";
				time = h[std::make_pair(vi,k)];
				myfile << time << " " << token << "\n";
				token += dataflow->getEdgeInPhase(e,k);
				myfile << time << " " << token << "\n";
			}
			myfile << "e\n";
			// Courbe de consommation RETIMING
			token = 0;
			for(EXEC_COUNT k = 1 ; k <= max_kj;k++){
				time = g[std::make_pair(vj,k)];
				myfile << time << " " << token << "\n";
				token += dataflow->getEdgeOutPhase(e,k);
				myfile << time << " " << token << "\n";
				time = h[std::make_pair(vj,k)];
				myfile << time << " " << token << "\n";
			}
			myfile << "e\n";

			// Beta IJ
			myfile <<  -(beta_cons[e]/alpha[e]) <<" 0 \n";
			myfile <<  -(beta_cons[e]/alpha[e]) + beta_ij[e] << " 0 \n";
			myfile.close();

			// write final file

			const TIME_UNIT si = solver.getValue("s_" + dataflow->getVertexName(vi));
			const TIME_UNIT sj = solver.getValue("s_" + dataflow->getVertexName(vj));

			myfile.open ( (dataflow->getEdgeName(e) + std::string("_") + dataflow->getVertexName(vi) + "_final.plot").c_str());

			myfile << "set key on outside left box title 'Légende'\n";
			myfile << "set style line 1 lt 2 lw 1 pt 2 linecolor rgb \"red\"\n";
			myfile << "set style line 2 lt 2 lw 1 pt 2 linecolor rgb \"green\"\n";
			myfile << "set style line 3 lt 1 lw 1 pt 1 linecolor rgb \"green\"\n";
			myfile << "set style line 4 lt 1 lw 2 pt 1 linecolor rgb \"blue\"\n";
			myfile << "set style line 5 lt 2 lw 2 pt 1 linecolor rgb \"orange\"\n";
			myfile << "set style line 6 lt 1 lw 1 pt 2 linecolor rgb \"magenta\"\n";
			myfile << "set style line 7 lt 1 lw 1 pt 1 linecolor rgb \"orange\"\n";
			myfile << "set style line 8 lt 1 lw 4 pt 1 linecolor rgb \"black\"\n";
			myfile      << "plot "
					<< "\"-\" w linespoints  ls 1  title 'Retimed prod', "
					<< "\"-\" w linespoints  ls 2  title 'Retimed cons', "
					<<  alpha[e]  << " * x + " << beta_feedback_prod[e]  << " w l ls 5  title 'prod b' , "
					<<  alpha[e]  << " * x + " << beta_feedback_cons[e]  << " w l ls 3 title 'cons b' ,"
					<< "\"-\" w linespoints  ls 8  title 'Buffer size : " <<  beta_feedback_cons[e] - beta_feedback_prod[e] << "'"
					<< " \n";
			// Courbes de prod / cons
			TIME_UNIT max_time = 3 * std::max(h[std::make_pair(vi,max_ki)],h[std::make_pair(vj,max_kj)]);
			TIME_UNIT mu_i = mu[vi];
			TIME_UNIT mu_j = mu[vj];
			EXEC_COUNT ki = 1;
			EXEC_COUNT kj = 1;
			time = 0;
			token = 0;
			while (time < max_time) {
				EXEC_COUNT k =  ((kj -1) % max_kj) + 1;
				time = sj + g[std::make_pair(vj,k)] + mu_j * ((kj - 1) / max_kj);
				myfile << time << " " << token << "\n";
				myfile << time + dataflow->getVertexDuration(vj,k) << " " << token << "\n";
				token += dataflow->getEdgeOutPhase(e,k);
				myfile << time + dataflow->getVertexDuration(vj,k) << " " << token << "\n";
				kj++;
			}
			myfile << "e\n";
			time = 0;
			token = 0;
			while (time < max_time) {
				EXEC_COUNT k =  ((ki -1) % max_ki) + 1;
				time = si + g[std::make_pair(vi,k)] + mu_i * ((ki - 1) / max_ki);
				myfile << time << " " << token << "\n";
				token += dataflow->getEdgeInPhase(e,k);
				myfile << time << " " << token << "\n";
				myfile << time + dataflow->getVertexDuration(vi,k) << " " << token << "\n";
				ki++;
			}
			myfile << "e\n";
			// Beta IJ
			myfile << "0 " << beta_feedback_prod[e] <<"  \n";
			myfile << "0 " <<  beta_feedback_cons[e] << "  \n";
			myfile.close();




			// write production file


			myfile.open ( (dataflow->getEdgeName(e) + std::string("_") + dataflow->getVertexName(vi) + "_production.plot").c_str());

			myfile << "set key on outside left box title 'Légende'\n";
			myfile << "set style line 1 lt 2 lw 1 pt 2 linecolor rgb \"red\"\n";
			myfile << "set style line 2 lt 2 lw 1 pt 2 linecolor rgb \"green\"\n";
			myfile << "set style line 3 lt 1 lw 1 pt 1 linecolor rgb \"green\"\n";
			myfile      << "plot "
					<< "\"-\" w linespoints  ls 1  title 'Burst prod', "
					<< "\"-\" w linespoints  ls 2  title 'Retimed prod', "
					<<  alpha[e]  << " * x + " << beta_prod[e]  << " w l ls 5  title 'prod b'"
					<< " \n";
			// Courbes de prod / cons
			max_time = 3 * h[std::make_pair(vi,max_ki)];
			ki = 1;
			kj = 1;
			time = 0;
			token = 0;
			while (time < max_time) {
				EXEC_COUNT k =  ((ki -1) % max_ki) + 1;
				time = s[std::make_pair(vi,k)] + mu_i * ((ki - 1) / max_ki);
				myfile << time << " " << token << "\n";
				myfile << time + dataflow->getVertexDuration(vi,k) << " " << token << "\n";
				token += dataflow->getEdgeInPhase(e,k);
				myfile << time + dataflow->getVertexDuration(vi,k) << " " << token << "\n";
				ki++;
			}
			myfile << "e\n";
			time = 0;
			token = 0;
			ki = 1;
			kj = 1;
			while (time < max_time) {
				EXEC_COUNT k =  ((ki -1) % max_ki) + 1;
				time = g[std::make_pair(vi,k)] + mu_i * ((ki - 1) / max_ki);
				myfile << time << " " << token << "\n";
				myfile << time + dataflow->getVertexDuration(vi,k) << " " << token << "\n";
				token += dataflow->getEdgeInPhase(e,k);
				myfile << time + dataflow->getVertexDuration(vi,k) << " " << token << "\n";
				ki++;
			}
			myfile.close();



			// write consumption file


			myfile.open ( (dataflow->getEdgeName(e) + std::string("_") + dataflow->getVertexName(vi) + "_consumption.plot").c_str());

			myfile << "set key on outside left box title 'Légende'\n";
			myfile << "set style line 1 lt 2 lw 1 pt 2 linecolor rgb \"red\"\n";
			myfile << "set style line 2 lt 2 lw 1 pt 2 linecolor rgb \"green\"\n";
			myfile << "set style line 3 lt 1 lw 1 pt 1 linecolor rgb \"green\"\n";
			myfile      << "plot "
					<< "\"-\" w linespoints  ls 2  title 'Retimed cons', "
					<<  alpha[e]  << " * x + " << beta_cons[e]  << " w l ls 5  title 'cons b'"
					<< " \n";
			// Courbes de prod / cons
			max_time = 3 * h[std::make_pair(vj,max_kj)];
			time = 0;
			token = 0;
			ki = 1;
			kj = 1;
			while (time < max_time) {
				EXEC_COUNT k =  ((kj -1) % max_kj) + 1;
				time = g[std::make_pair(vj,k)] + mu_j * ((kj - 1) / max_kj);
				myfile << time << " " << token << "\n";
				token += dataflow->getEdgeOutPhase(e,k);
				myfile << time << " " << token << "\n";
				myfile << time + dataflow->getVertexDuration(vj,k) << " " << token << "\n";
				kj++;
			}
			myfile.close();

		}}
	}




}
