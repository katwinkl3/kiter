/*
 * EventGraph.cpp
 *
 *  Created on: 17 avr. 2012
 *      Author: toky
 */

#include <commons/verbose.h>
#include <models/EventGraph.h>


namespace models {

    void StrongConnect(models::EventGraph * g,models::EventGraphVertex v, int* index,std::map<models::EventGraphVertex,int> * vindex,std::map<models::EventGraphVertex,int> * lowlink,std::vector<models::EventGraphVertex>* S)  {

               // set deph
                vindex->insert(std::pair<models::EventGraphVertex,int>(v,*index));
                lowlink->insert(std::pair<models::EventGraphVertex,int>(v,*index));
               (*index)++;
               S->push_back(v);

               //successors
               {ForEachOutputs(g,v,c) {
                   models::EventGraphVertex w = g->getTarget(c);
                   if (vindex->find(w) == vindex->end()) {
                       StrongConnect(g,w,index,vindex,lowlink,S);
                       lowlink->at(v) = std::min( lowlink->at(v), lowlink->at(w));
                   } else if (commons::findInVector(S,w)) {
                       lowlink->at(v) = std::min( lowlink->at(v), vindex->at(w));
                   }
               }}

               //if root
               if ( lowlink->at(v) == vindex->at(v)) {
                   models::EventGraphVertex w;
                   do {
                      w =  S->back(); S->pop_back();
                      g->setSCC(w,(unsigned int)v);
                   } while (w != v );
               }

     }
    void freeStrongConnection(std::map<unsigned int,models::EventGraph*>* pointer ) {
    	   for (std::map<unsigned int ,models::EventGraph*>::iterator it = pointer->begin();it != pointer->end();it++) {
    		   delete (it->second);
    	   }
    	   delete pointer;
    }
    std::map<unsigned int,models::EventGraph*>* StrongConnection(models::EventGraph * g)  {

        int index = 0;
        std::map<models::EventGraphVertex,int>  vindex;
        std::map<models::EventGraphVertex,int>  lowlink;
        std::vector<models::EventGraphVertex> S;
        {ForEachEvent(g,v) {
           if (vindex.find(v) == vindex.end()) {
               StrongConnect(g,v,&index,&vindex,&lowlink,&S);
           }
        }}

        VERBOSE_ASSERT(S.size() == 0,TXT_NEVER_HAPPEND);

        // generation des EventGraphs
        std::map<unsigned int,models::EventGraph*>*  graphs = new std::map<unsigned int,models::EventGraph*>();

        // assignation des event
        {ForEachEvent(g,v) {
            if (graphs->find(g->getSCC(v)) == graphs->end()) graphs->insert(std::pair<unsigned int,models::EventGraph*>(g->getSCC(v),new models::EventGraph()));
            graphs->at(g->getSCC(v))->addEvent(g->getEvent(v));
        }}

        // assignation des constraints
        {ForEachConstraint(g,c) {
            models::EventGraphVertex source = g->getSource(c);
            models::EventGraphVertex destination = g->getTarget(c);

            if (g->getSCC(source) == g->getSCC(destination)) {
                graphs->at(g->getSCC(source))->addEventConstraint(g->getConstraint(c));
            }
        }}


        return graphs;
    }

    std::string EventGraph::printDOT()  {
         std::ostringstream returnStream;

             // head file
              returnStream << "// Auto-generate by Kiter" << std::endl;
              returnStream <<  "//   use this dot file with circo for an optimal visu\n" << std::endl;
              returnStream <<  "digraph G {\n" << std::endl;

              returnStream <<  "graph [label=\"" << "Auto-generate by the Kiter" <<
                      "\\n#events="<< this->getEventCount()<<" #constraints=" << this->getConstraintsCount()
                      << "\",overlap=scale,splines=true,mindist=3]\n";
              returnStream << "edge [labelangle=10,labeldistance=1,len=1.5,fontsize=10,labelsize=4,color=grey]" << std::endl;

              ARRAY_INDEX lastid = 0;
              {ForEachEvent(this,e) {
            	    if (lastid != this->getEvent(e).getTaskId())
            	    	if (lastid != 0) returnStream << "}" << std::endl;
            	  if (lastid != this->getEvent(e).getTaskId()) {
            		  lastid = this->getEvent(e).getTaskId();
            		  returnStream << " subgraph cluster_" << lastid << " {" << std::endl;
            		  returnStream << "style=filled;" << std::endl;
            		  returnStream << "color=lightgrey;" << std::endl;
            		  returnStream << "label = \"task " << lastid << "\";" << std::endl;
            	  }
                  returnStream <<  e << " [shape=circle,label=\""
                                << this->getEvent(e).toString()
                                << "\\n("
                                << char('A' + this->getSCC(e))
                                << ")\"," ;

                  if (this->getRoot() == e) {
                      returnStream << "color=red,";
                  }

                  returnStream << "pos=\"" << lastid * 100 << "," << this->getEvent(e).getTaskOc() * 20 << "\",";


                  returnStream  << "];" << std::endl;



                  VERBOSE_DEBUG("Event " <<  this->getEvent(e).toString());

                  VERBOSE_DEBUG("  -  SCC " <<  this->getSCC(e));
                  if (this->getPrevious(e) != NULL_EventGraphVertex) {
                	  VERBOSE_DEBUG("  -  previous is " << this->getEvent(this->getPrevious(e)).toString());
                  } else {
                	  VERBOSE_DEBUG("  -  previous is empty");
                  }

              }}
              if (lastid != 0) returnStream << "}" << std::endl;

              {ForEachConstraint(this,c){
                  returnStream << this->getSource(c) << " ->" << this->getTarget(c);
                                    //labeldistance=\"5\",tail
                  returnStream << " [label=\"w=" << this->getWeight(c) << " d=" << this->getDuration(c) << " \"";
                  if(this->getPrevious(this->getTarget(c)) == this->getSource(c))  returnStream << ",fontsize=20,penwidth=5";
                  returnStream << "];" << std::endl;


              }}

            returnStream <<  "}" << std::endl;

            return returnStream.str();
    }



    TOKEN_UNIT EventGraph::getWeight(SchedulingEvent se1 ,SchedulingEvent se2 )    {

    	/* compute source and target */
    	EventGraphVertex source = NULL_EventGraphVertex;
    	{ForEachEvent(this,v) {
    		if ((this->getTaskId(v) == se1.getTaskId()) && (this->getExecution(v) == se1.getTaskOc())) {
    			VERBOSE_ASSERT(source == NULL_EventGraphVertex,"Duplicated event !");
    			source = v;
    		}
    	}}
    	VERBOSE_ASSERT(source != NULL_EventGraphVertex,"source event not found !");

    	TOKEN_UNIT weight = std::numeric_limits<TOKEN_UNIT>::min();
    	bool found = false;
    	  {ForEachOutputs(this,source,c) {
    		  EventGraphVertex current_target = this->getTarget(c);
    		  if ((this->getTaskId(current_target) == se2.getTaskId()) && (this->getExecution(current_target) == se2.getTaskOc())) {
    			  weight = (TOKEN_UNIT) this->getWeight(c); // FIXME : les poids sont stockés en double pour le moment

            	  VERBOSE_ASSERT(!found, "already exist");
            	  found = true;
    		  }

    	  }}

    	  if (!found) {
    		  VERBOSE_ERROR("path not found : " << se1.toString() << " - " << se2.toString());
    		  std::cout << "path not found : " << se1.toString() << " - " << se2.toString() << std::endl;
    	  }

    	  VERBOSE_ASSERT(found, "unfound path");
    	return weight;
    }


} // end of algorithms namespace
