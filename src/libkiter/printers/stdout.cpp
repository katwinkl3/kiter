/*
 * stdout.cpp
 *
 *  Created on: 16 avr. 2013
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <algorithms/repetition_vector.h>
#include <printers/stdout.h>


std::string printers::GenerateDOT    (models::Dataflow* const  dataflow ) {

  std::ostringstream returnStream;
  
  
  returnStream << "// Auto-generate by Kiter" << std::endl;
  returnStream << "//   use this dot file with circo for an optimal visu\n" << std::endl;
  returnStream << "digraph G {\n" << std::endl;
  
  returnStream <<  "  graph [label=\"" << "Auto-generate by the Kiter"
	       << "\",overlap=scale,splines=true]\n";
  returnStream << "  edge [labelangle=15,labeldistance=1,len=1.5,fontsize=8,labelsize=4,color=grey]" << std::endl;

  returnStream << std::endl;


  {ForEachVertex(dataflow,t) {

      ARRAY_INDEX tid =  dataflow->getVertexId(t);
      
      returnStream << "  t_" << tid << " [" << std::endl;
      returnStream << "    shape=circle," << std::endl;
      returnStream << "    label = \" " << dataflow->getVertexName(t) << "(id:" << tid << " duration:" << dataflow->getVertexDuration(t) << ")" << "\"" << std::endl;
      
      returnStream  << "  ];" << std::endl;
      returnStream << std::endl;
    }}


  
  {ForEachChannel(dataflow,c){
      ARRAY_INDEX edgeIn  = dataflow->getVertexId(dataflow->getEdgeSource(c));
      ARRAY_INDEX edgeOut = dataflow->getVertexId(dataflow->getEdgeTarget(c));
      returnStream << "  t_" << edgeIn << " -> t_" << edgeOut << " [";
      returnStream << std::endl;
      returnStream << "    headlabel=\"" << dataflow->getEdgeOut(c) << "\"," << std::endl;
      returnStream << "    taillabel=\"" << dataflow->getEdgeIn(c) << "\"," << std::endl;
      returnStream << " ] ;" << std::endl;
    }}
  returnStream << std::endl;
  
  returnStream <<  "}" << std::endl;
  
  return returnStream.str();

}


void printers::printGraph    (models::Dataflow* const  dataflow, parameters_list_t ) {

  std::cout << printers::GenerateDOT    ( dataflow ) ;

}


void printers::printInfos    (models::Dataflow* const  dataflow, parameters_list_t ) {

                VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
		EXEC_COUNT total = 0;
		std::cout << "Graph name             = " <<  dataflow->getName() << std::endl;
		std::cout << "Task count             = " <<  dataflow->getVerticesCount() << std::endl;
		std::cout << "Channels total         = " <<  dataflow->getEdgesCount() << std::endl;

              {ForEachTask(dataflow,t) {
		  total += dataflow->getNi(t) ;
                  std::cout << " - " <<  dataflow->getVertexName(t) << " N=" << dataflow->getNi(t) << std::endl;

              }}

              std::cout << "Complexity         = " <<  total << std::endl;
}


