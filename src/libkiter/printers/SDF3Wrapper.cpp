/*
 * SDF3Wrapper.cpp
 *
 *  Created on: 15 mars 2012
 *      Author: toky
 *
 *
 */

#include <libxml/xmlversion.h>
#include <libxml/xmlwriter.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/encoding.h>

#include <printers/SDF3Wrapper.h>
#include <commons/commons.h>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <iostream>
#include <cstring>
#include <fstream>

using std::vector;

#define TXT_XML_ERROR "TXT_XML_ERROR"
static const char INIT_PERIODIC_SEPARATOR = ';';

template<typename T>
std::string vectorAsStr(const std::vector<T>& t)
{
	 std::stringstream s;
		for (typename std::vector<T>::size_type idx = 0 ; idx < t.size() ; idx++) {
			if (idx > 0) {
				 s << ",";
			}
			s << commons::toString(t[idx]);
		}
		return s.str();
}
template<>
std::string vectorAsStr(const std::vector<TIME_UNIT>& t)
{
	 std::stringstream s;
		for (typename std::vector<TIME_UNIT>::size_type idx = 0 ; idx < t.size() ; idx++) {
			if (idx > 0) {
				 s << ",";
			}
			TIME_UNIT v = t[idx];
			if ((std::floor(v)==std::ceil(v))) {
				s << (unsigned long) (v) ;
			} else {
				s << commons::toString(v);
			}
		}
		return s.str();
}


namespace printers {



template <typename T1,typename T2>
std::pair<T1,T2> splitAndDefault(const std::string &s,char del,T2 d) {
	std::vector<std::string> sub;
	std::pair<T1,T2>  res;
	sub = commons::split <std::string> (s, del);

	switch (sub.size()) {
	case 0  :   VERBOSE_FAILURE(); break;
	case 2  :   res.first  = commons::fromString<T1>(sub[0]);
	res.second = commons::fromString<T2>(sub[1]);
	break;
	case 1  :   res.first  = commons::fromString<T1>(sub[0]);
	res.second = d;
	break;
	default :   VERBOSE_FAILURE(); break;
	}

	return res;
}



void                        readSDF3OutputSpec      (models::Dataflow *to, const Edge c, const std::string rates) {

	std::vector<std::string>  init_periodic = commons::split<std::string> (rates, INIT_PERIODIC_SEPARATOR);
	std::vector<std::string>  init_phases = (init_periodic.size() == 2) ? commons::split<std::string> (init_periodic[0], ',') : std::vector<std::string>() ;
	std::vector<std::string>  periodic_phases = (init_periodic.size() == 1) ? commons::split<std::string> (init_periodic[0], ',') : commons::split<std::string> (init_periodic[1], ',') ;
	std::vector<TOKEN_UNIT>   init_cons;
	std::vector<TOKEN_UNIT>   periodic_cons;

	for (unsigned int i = 1 ; i <= init_phases.size() ; i++) {
		init_cons.push_back(commons::fromString<TOKEN_UNIT>(init_phases[i-1]));
	}

	for (unsigned int i = 1 ; i <= periodic_phases.size() ; i++) {
		periodic_cons.push_back(commons::fromString<TOKEN_UNIT>(periodic_phases[i-1]));
	}

	VERBOSE_ASSERT(periodic_cons.size() > 0, "Edges output rates must have a periodic pattern. ");

	to->setEdgeOutInitPhases(c,init_cons);
	to->setEdgeOutPhases(c,periodic_cons);

}

void                        readSDF3InputSpec      (models::Dataflow *to, const Edge c, const std::string rates) {

	std::vector<std::string>  init_periodic = commons::split<std::string> (rates, INIT_PERIODIC_SEPARATOR);
	std::vector<std::string>  init_phases = (init_periodic.size() == 2) ? commons::split<std::string> (init_periodic[0], ',') : std::vector<std::string>() ;
	std::vector<std::string>  periodic_phases = (init_periodic.size() == 1) ? commons::split<std::string> (init_periodic[0], ',') : commons::split<std::string> (init_periodic[1], ',') ;
	std::vector<TOKEN_UNIT>   init_prod;
	std::vector<TOKEN_UNIT>   periodic_prod;

	for (unsigned int i = 1 ; i <= init_phases.size() ; i++) {
		init_prod.push_back(commons::fromString<TOKEN_UNIT>(init_phases[i-1]));
	}

	for (unsigned int i = 1 ; i <= periodic_phases.size() ; i++) {
		periodic_prod.push_back(commons::fromString<TOKEN_UNIT>(periodic_phases[i-1]));
	}

	VERBOSE_ASSERT(periodic_prod.size() > 0, "Edges input rates must have a periodic pattern. ");
	to->setEdgeInInitPhases(c,init_prod);
	to->setEdgeInPhases(c,periodic_prod);

}

void readSDF3VertexPorts (models::Dataflow *to,xmlNodePtr taskNode) {

	std::string taskName = "";
	Vertex pVertex;

	// get Vertex name
	for (xmlAttrPtr cur_attr = taskNode->properties; cur_attr; cur_attr = cur_attr->next) {
		if (strcmp((const char*)cur_attr->name,"name") == 0) {
			taskName = (const char*)cur_attr->children->content ;
		}
	}

	VERBOSE_ASSERT(taskName != "",TXT_XML_ERROR);

	// get Vertex
	pVertex = to->getVertexByName(taskName);

	// list ports
	for (xmlNodePtr cur_node = taskNode->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (strcmp((const char*)cur_node->name,"port") == 0) {

				std::string type,rate,name = "";

				for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
					if (strcmp((const char*)cur_attr->name,"type") == 0) { type = (const char*)cur_attr->children->content;}
					if (strcmp((const char*)cur_attr->name,"name") == 0) { name = (const char*)cur_attr->children->content;}
					if (strcmp((const char*)cur_attr->name,"rate") == 0) { rate = (const char*)cur_attr->children->content;}
				}

				if (type == "in") {
					Edge pEdge = to->getInputEdgeByPortName(pVertex,name);
					if (pEdge != NULL_EDGE) {
						readSDF3OutputSpec(to,pEdge,rate);
					} else {
						VERBOSE_DEBUG ("Could not find in edge " << name << " might be reentrancy.");
					}
				} else

					if (type == "out") {
						Edge pEdge = to->getOutputEdgeByPortName(pVertex,name);
						if (pEdge != NULL_EDGE) {
							readSDF3InputSpec(to,pEdge,rate);
						} else {
							VERBOSE_DEBUG ("Could not find out edge " << name << " might be reentrancy.");
						}
					} else {
						VERBOSE_FAILURE();
					}

			}


		}
	}

}




void readSDF3VertexTimings (models::Dataflow *to,xmlNodePtr taskNode) {

	std::string taskName,timings = "";
	Vertex pVertex;

	// get Vertex name
	for (xmlAttrPtr cur_attr = taskNode->properties; cur_attr; cur_attr = cur_attr->next) {
		if (strcmp((const char*)cur_attr->name,"actor") == 0) {
			taskName = (const char*)cur_attr->children->content ;
			break;
		}
	}

	VERBOSE_ASSERT(taskName.size() > 0,TXT_XML_ERROR);

	// get Vertex
	pVertex = to->getVertexByName(taskName);


	//
	for (xmlNodePtr cur_node = taskNode->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (strcmp((const char*)cur_node->name,"processor") == 0) {

				// // Get cluster
				// for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
				// if (strcmp((const char*)cur_attr->name,"type") == 0) {
				//         std::string cluster_string = (const char*)cur_attr->children->content ;
				//         if (cluster_string.find('_') != cluster_string.npos) {
				//             int cluster_id = commons::fromString<int>(cluster_string.substr(cluster_string.find('_')+1,cluster_string.size() - cluster_string.find('_') - 1));
				//             to->setVertexCuster(pVertex,models::Cluster(cluster_id));
				//         }
				//
				//     }
				// }

				for (xmlNodePtr cur_node2 = cur_node   ->children; cur_node2; cur_node2 = cur_node2->next) {
					if (cur_node2->type == XML_ELEMENT_NODE) {
						if (strcmp((const char*)cur_node2->name,"executionTime") == 0) {
							for (xmlAttrPtr cur_attr = cur_node2->properties; cur_attr; cur_attr = cur_attr->next) {
								if (strcmp((const char*)cur_attr->name,"time") == 0) {
									timings = (const char*)cur_attr->children->content ;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	std::vector<std::string>  init_periodic = commons::split<std::string> (timings, INIT_PERIODIC_SEPARATOR);
	std::vector<std::string>  init_phases = (init_periodic.size() == 2) ? commons::splitSDF3List (init_periodic[0]) : std::vector<std::string>() ;
	std::vector<std::string>  periodic_phases = (init_periodic.size() == 1) ? commons::splitSDF3List (init_periodic[0]) : commons::splitSDF3List (init_periodic[1]) ;

	std::vector<TIME_UNIT> init_times;
	std::vector<TIME_UNIT> periodic_times;

	to->setInitPhasesQuantity(pVertex,init_phases.size());
	to->setPhasesQuantity(pVertex,periodic_phases.size());

	for (unsigned int i = 1 ; i <= periodic_phases.size() ; i++) {
		periodic_times.push_back(commons::fromString<TIME_UNIT>(periodic_phases[i-1]));
	}
	for (unsigned int i = 1 ; i <= init_phases.size() ; i++) {
		init_times.push_back(commons::fromString<TIME_UNIT>(init_phases[i-1]));
	}
	to->setVertexDuration(pVertex,periodic_times);
	to->setVertexInitDuration(pVertex,init_times);

}


void readSDF3VertexReentrancy (models::Dataflow *to,xmlNodePtr taskNode) {

	std::string taskName,max_reen = "";
	Vertex pVertex;

	// get Vertex name
	for (xmlAttrPtr cur_attr = taskNode->properties; cur_attr; cur_attr = cur_attr->next) {
		if (strcmp((const char*)cur_attr->name,"actor") == 0) {
			taskName = (const char*)cur_attr->children->content ;
			break;
		}
	}

	VERBOSE_ASSERT(taskName.size() > 0,TXT_XML_ERROR);

	// get Vertex
	pVertex = to->getVertexByName(taskName);


	//
	for (xmlNodePtr cur_node = taskNode->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (strcmp((const char*)cur_node->name,"memory") == 0) {

				for (xmlNodePtr cur_node2 = cur_node   ->children; cur_node2; cur_node2 = cur_node2->next) {
					if (cur_node2->type == XML_ELEMENT_NODE) {
						if (strcmp((const char*)cur_node2->name,"stateSize") == 0) {
							for (xmlAttrPtr cur_attr = cur_node2->properties; cur_attr; cur_attr = cur_attr->next) {
								if (strcmp((const char*)cur_attr->name,"max") == 0) {
									max_reen = (const char*)cur_attr->children->content ;
									break;
								}
							}
						}
					}
				}
			} // end of memory slot
		}
	}

	VERBOSE_DEBUG("Set reetrancy On");
	to->setReentrancyFactor(pVertex,1);

}

bool onlyOneRate(std::string rates) {
	std::vector<std::string>  init_periodic = commons::split <std::string> (rates, INIT_PERIODIC_SEPARATOR);
	if (init_periodic.size() > 1) {
		for (auto x : init_periodic) {
			if (not onlyOneRate(x)) {return false;}
		}
	} else {
		auto rate = init_periodic[0];
		// '1' and ',' alternate
		bool val = (rate[0] == '1') ? true : false;
		for (unsigned int i = 0 ; i < rate.size() ; i++) {
			if (!(val?rate[i]=='1':rate[i]==',')) return false;
			val = !val;
		}
	}

	return true;
}
bool checkReentrancy( xmlNodePtr csdf, xmlNodePtr cur_node) {



	std::string sourceName,targetName,inPortName,outPortName,preload = "";
	preload = "0";

	for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
		if (strcmp((const char*)cur_attr->name,"srcActor") == 0) {
			sourceName = (const char*)cur_attr->children->content;
			continue;
		}
		if (strcmp((const char*)cur_attr->name,"dstActor") == 0) {
			targetName = (const char*)cur_attr->children->content;
			continue;
		}
		if (strcmp((const char*)cur_attr->name,"srcPort") == 0) {
			inPortName = (const char*)cur_attr->children->content;
			continue;
		}
		if (strcmp((const char*)cur_attr->name,"dstPort") == 0) {
			outPortName = (const char*)cur_attr->children->content;
			continue;
		}
		if (strcmp((const char*)cur_attr->name,"initialTokens") == 0) {
			preload = (const char*)cur_attr->children->content;
			continue;
		}

	}

	//not a loop, not a reentrancy loop !
	if (sourceName != targetName) return false;

	VERBOSE_DEBUG("a loop Buffer ?");

	if (preload != "1") {
		VERBOSE_DEBUG("preload not work :" << preload);
		return false;
	}


	//check prod and cons for inPortName  and  outPortName
	bool foundIn = false , foundOut = false;
	// STEP 3 - get task phase count and productions
	for (xmlNodePtr cur_node = csdf->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (strcmp((const char*)cur_node->name,"actor") == 0) {
				for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
					if (strcmp((const char*)cur_attr->name,"name") == 0) {
						if (strcmp((const char*)cur_attr->children->content,sourceName.c_str()) == 0) {
							// acteur source
							for (xmlNodePtr port_node = cur_node->children; port_node; port_node = port_node->next) {
								if (port_node->type == XML_ELEMENT_NODE) {
									if (strcmp((const char*)port_node->name,"port") == 0) {

										std::string type,rate,name = "";

										for (xmlAttrPtr cur_attr = port_node->properties; cur_attr; cur_attr = cur_attr->next) {
											if (strcmp((const char*)cur_attr->name,"type") == 0) { type = (const char*)cur_attr->children->content;}
											if (strcmp((const char*)cur_attr->name,"name") == 0) { name = (const char*)cur_attr->children->content;}
											if (strcmp((const char*)cur_attr->name,"rate") == 0) { rate = (const char*)cur_attr->children->content;}
										}
										if ((type == "out") && (name == inPortName)) {
											//check rate all 1
											if (!onlyOneRate(rate)) {
												VERBOSE_DEBUG("Rate not Ok :" << rate);
												return false;
											}
											foundIn = true;
											if (foundOut) return true; // speed up
										}
									}
								}
							}

						}
						if (strcmp((const char*)cur_attr->children->content,targetName.c_str()) == 0) {
							// acteur target
							for (xmlNodePtr port_node = cur_node->children; port_node; port_node = port_node->next) {
								if (port_node->type == XML_ELEMENT_NODE) {
									if (strcmp((const char*)port_node->name,"port") == 0) {

										std::string type,rate,name = "";

										for (xmlAttrPtr cur_attr = port_node->properties; cur_attr; cur_attr = cur_attr->next) {
											if (strcmp((const char*)cur_attr->name,"type") == 0) { type = (const char*)cur_attr->children->content;}
											if (strcmp((const char*)cur_attr->name,"name") == 0) { name = (const char*)cur_attr->children->content;}
											if (strcmp((const char*)cur_attr->name,"rate") == 0) { rate = (const char*)cur_attr->children->content;}
										}
										if ((type == "in") && (name == outPortName)) {
											if (!onlyOneRate(rate)) {
												VERBOSE_DEBUG("Rate not Ok :" << rate);
												return false;
											}
											foundOut = true;
											if (foundIn) return true; // speed up
										}
									}
								}
							}

						}




					}
				}
			}
		}
	}
	// list ports


	if (!(foundIn && foundOut)) {
		VERBOSE_DEBUG("not found ... foundIn=" << foundIn << " foundOut=" << foundOut);
	}


	return foundIn && foundOut;

}

models::Dataflow* wrapSDF3Dataflow (xmlDocPtr doc) {

	xmlNodePtr AG= NULL,csdf= NULL,csdfproperties = NULL;
	models::Dataflow *to = new models::Dataflow(0);
	xmlNodePtr sdf3 = xmlDocGetRootElement(doc);

	// Check file is correct
	//--------------------------------

	if (sdf3 == NULL) {
		FAILED("Document XML invalide");
	}

	if (std::string((const char*)sdf3->name) != "sdf3") {
		FAILED("Document XML invalide");
	}


	// Look for ApplicationGraph Node
	//--------------------------------

	for (xmlNodePtr cur_node = sdf3->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (std::string((const char*)cur_node->name) == std::string("applicationGraph")) AG = cur_node;
		}
	}

	if (!AG) {
		FAILED("Document XML invalide");
	}


	// Get graph name
	//--------------------------------

	for (xmlAttrPtr cur_attr = AG->properties; cur_attr; cur_attr = cur_attr->next) {
		if (strcmp((const char*)cur_attr->name,"name") == 0) {

			to->setAppName((const char*)cur_attr->children->content);
		}
	}


	// get the sdf/csdf node and the sdfProperties/csdfproperties node
	//--------------------------------------------------------------------------------

	for (xmlNodePtr cur_node = AG->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			if (std::string((const char*)cur_node->name) == std::string("csdf"))           csdf = cur_node;
			if (std::string((const char*)cur_node->name) == std::string("csdfProperties")) csdfproperties = cur_node;
			if (std::string((const char*)cur_node->name) == std::string("sdf"))           csdf = cur_node;
			if (std::string((const char*)cur_node->name) == std::string("sdfProperties")) csdfproperties = cur_node;
		}
	}

	if (csdf == NULL)           { FAILED("Document XML invalide, csdf not found");}
	if (csdfproperties == NULL) { FAILED("Document XML invalide, csdfproperties not found");}


	for (xmlAttrPtr cur_attr = csdf->properties; cur_attr; cur_attr = cur_attr->next) {
		if (strcmp((const char*)cur_attr->name,"name") == 0) {
			to->setGraphName((const char*)cur_attr->children->content);
		}
		if (strcmp((const char*)cur_attr->name,"type") == 0) {
			to->setGraphType((const char*)cur_attr->children->content);
		}
	}

	// Generate Vertex list with names and zero reentrancy
	//--------------------------------------------------------------------------------

	for (xmlNodePtr cur_node = csdf->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			if (std::string((const char*)cur_node->name) == std::string("actor")) {

				Vertex newVertex = to->addVertex();
				to->setReentrancyFactor(newVertex,0); // par defaut une tâche SDF3 est reetrante à l'infini

				for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
					if (strcmp((const char*)cur_attr->name,"name") == 0) {

						to->setVertexName(newVertex,(const char*)cur_attr->children->content);
					}
					if (strcmp((const char*)cur_attr->name,"type") == 0) {

						to->setVertexType(newVertex,(const char*)cur_attr->children->content);
					}
				}
			}
		}
	}


	// Generate Edge list with names, in and out vertices, tokensize and preload
	// If the edge is seen as a reentrancy edge, only set reentrancy to the task
	//--------------------------------------------------------------------------------

	for (xmlNodePtr cur_node = csdf->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			if (std::string((const char*)cur_node->name) == std::string("channel")) {
				std::string bufferName,sourceName,targetName,inPortName,outPortName,preload,tokenSize = "";
				preload = "0";
				for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
					if (strcmp((const char*)cur_attr->name,"name") == 0) {
						bufferName = (const char*)cur_attr->children->content;
					}
					if (strcmp((const char*)cur_attr->name,"srcActor") == 0) {
						sourceName = (const char*)cur_attr->children->content;
					}
					if (strcmp((const char*)cur_attr->name,"dstActor") == 0) {
						targetName = (const char*)cur_attr->children->content;
					}

					if (strcmp((const char*)cur_attr->name,"srcPort") == 0) {
						inPortName = (const char*)cur_attr->children->content;
					}
					if (strcmp((const char*)cur_attr->name,"dstPort") == 0) {
						outPortName = (const char*)cur_attr->children->content;
					}
					if (strcmp((const char*)cur_attr->name,"initialTokens") == 0) {
						preload = (const char*)cur_attr->children->content;
					}
					if (strcmp((const char*)cur_attr->name,"size") == 0) {
						tokenSize = (const char*)cur_attr->children->content;
					}
				}

				if (checkReentrancy(csdf,cur_node)) {
					VERBOSE_DEBUG("checkReentrancy(csdf,cur_node) returns TRUE");
					//arc de reetrance
					to->setReentrancyFactor(to->getVertexByName(sourceName),1);
				} else {
					Edge newEdge = to->addEdge(to->getVertexByName(sourceName),to->getVertexByName(targetName));

					to->setEdgeName(newEdge,bufferName);

					to->setEdgeInputPortName(newEdge,inPortName);
					to->setEdgeOutputPortName(newEdge,outPortName);
					if (tokenSize != "") {
						to->setTokenSize(newEdge,commons::fromString<TOKEN_UNIT>(tokenSize));
					} else {
						to->setTokenSize(newEdge,1);
					}
					if (preload != "") {
						to->setPreload(newEdge,commons::fromString<TOKEN_UNIT>(preload));
					} else {
						to->setPreload(newEdge,0);
					}
				}
			}
		}
	}


	// get task timings
	//--------------------------------------------------------------------------------
	for (xmlNodePtr cur_node = csdfproperties->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			if (std::string((const char*)cur_node->name) == std::string("actorProperties")) {
				readSDF3VertexTimings(to,cur_node);
			}
		}
	}

	// get task reentrancy again
	//--------------------------------------------------------------------------------
	for (xmlNodePtr cur_node = csdfproperties->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			if (std::string((const char*)cur_node->name) == std::string("actorProperties")) {
				readSDF3VertexReentrancy(to,cur_node); //fail function
			}
		}
	}

	// get task phase count and productions
	//--------------------------------------------------------------------------------
	for (xmlNodePtr cur_node = csdf->children; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			if (std::string((const char*)cur_node->name) == std::string("actor")) {
				readSDF3VertexPorts(to,cur_node);
			}
		}
	}





	return to;
}

models::Dataflow*  parseSDF3XML         (const std::string data) {

	xmlDocPtr doc =  xmlReadMemory(data.c_str(), data.size(), NULL, NULL,XML_PARSE_NOERROR|XML_PARSE_NOWARNING|XML_PARSE_RECOVER);

	if (doc == NULL) {
		VERBOSE_ERROR("Document XML invalide");
		return NULL;
	}



	models::Dataflow* wrappedDataflow = wrapSDF3Dataflow(doc);
	wrappedDataflow->setFilename("FromMemory");

	xmlFreeDoc(doc);

	return wrappedDataflow;
}

models::Dataflow*  readSDF3File         (const std::string f) {

	xmlDocPtr doc =  xmlReadFile(f.c_str(),NULL,XML_PARSE_NOERROR|XML_PARSE_NOWARNING|XML_PARSE_RECOVER);

	if (doc == NULL) {
		VERBOSE_ERROR("Document XML invalide");
		return NULL;
	}



	models::Dataflow* wrappedDataflow = wrapSDF3Dataflow(doc);
	wrappedDataflow->setFilename(f);

	xmlFreeDoc(doc);

	return wrappedDataflow;
}

void writeLoopbackChannel (xmlTextWriterPtr writer, const models::Dataflow* dataflow, const Vertex t) {

	// TODO/URGENT : We do not check that there is no channel already with this name or port with these names.

	const std::string edge_name = "R" + dataflow->getVertexName(t);
	const std::string edge_srcActor = dataflow->getVertexName(t);
	const std::string edge_srcPort = "in_R" + dataflow->getVertexName(t);
	const std::string edge_dstActor =dataflow->getVertexName(t);
	const std::string edge_dstPort = "out_R" + dataflow->getVertexName(t);
	const DATA_UNIT edge_size = 1;
	const TOKEN_UNIT edge_initialTokens = 1;

	xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "channel");
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) edge_name.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"srcActor", (const xmlChar*) edge_srcActor.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"srcPort", (const xmlChar*) edge_srcPort.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"dstActor", (const xmlChar*) edge_dstActor.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"dstPort", (const xmlChar*) edge_dstPort.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"size", (const xmlChar*) commons::toString(edge_size).c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"initialTokens", (const xmlChar*)  commons::toString(edge_initialTokens).c_str());

	xmlTextWriterEndElement(writer);
}


void writeLoopbackPorts (xmlTextWriterPtr writer, const models::Dataflow* dataflow, const Vertex t) {
	const std::string out_port = "out_R" + dataflow->getVertexName(t);
	const std::string in_port = "in_R" + dataflow->getVertexName(t);
	const std::vector<TOKEN_UNIT> periodic_rates        =    std::vector<TOKEN_UNIT> (dataflow->getPhasesQuantity(t),1) ;
	const std::vector<TOKEN_UNIT> init_rates        =    std::vector<TOKEN_UNIT> (dataflow->getInitPhasesQuantity(t),1) ;
	std::string rates = vectorAsStr(periodic_rates);
	if (init_rates.size()) rates = vectorAsStr(init_rates) + INIT_PERIODIC_SEPARATOR +   rates ;

	xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "port");
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) in_port.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) "out");
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"rate", (const xmlChar*) rates.c_str());
	xmlTextWriterEndElement(writer);

	xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "port");
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) out_port.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) "in");
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"rate", (const xmlChar*) rates.c_str());
	xmlTextWriterEndElement(writer);

}

void writeChannel (xmlTextWriterPtr writer, const models::Dataflow* dataflow, const Edge e) {

	const std::string edge_name = dataflow->getEdgeName(e);
	const std::string edge_srcActor = dataflow->getVertexName(dataflow->getEdgeSource(e));
	const std::string edge_srcPort ="in_" + commons::toString(dataflow->getEdgeId(e));
	const std::string edge_dstActor =dataflow->getVertexName(dataflow->getEdgeTarget(e));
	const std::string edge_dstPort = "out_" + commons::toString(dataflow->getEdgeId(e));
	const DATA_UNIT edge_size = dataflow->getTokenSize(e);
	const TOKEN_UNIT edge_initialTokens = dataflow->getPreload(e);
	xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "channel");
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) edge_name.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"srcActor", (const xmlChar*) edge_srcActor.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"srcPort", (const xmlChar*) edge_srcPort.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"dstActor", (const xmlChar*) edge_dstActor.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"dstPort", (const xmlChar*) edge_dstPort.c_str());
	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"size", (const xmlChar*) commons::toString(edge_size).c_str());
	if (edge_initialTokens) {
		xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"initialTokens", (const xmlChar*)  commons::toString(edge_initialTokens).c_str());
	}

	xmlTextWriterEndElement(writer);
}


std::string  generateSDF3XML         (const models::Dataflow* dataflow)  {







	xmlBufferPtr buf = xmlBufferCreate();
	if (buf == NULL) {
		VERBOSE_ERROR("xmlBufferCreate: Error creating the xml buffer");
		return "";
	}

	xmlTextWriterPtr writer = xmlNewTextWriterMemory(buf,0);
	// xmlTextWriterPtr writer = xmlNewTextWriterFilename(filename.c_str(), 0);
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	xmlTextWriterSetIndent(writer,1); xmlTextWriterStartElement(writer, (const xmlChar*) "sdf3");

	xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"xmlns:xsi", (const xmlChar*)"http://www.w3.org/2001/XMLSchema-instance");
	xmlTextWriterWriteAttribute (writer,(const xmlChar*)"version",(const xmlChar*)"1.0");
	xmlTextWriterWriteAttribute (writer,(const xmlChar*)"type", (const xmlChar*)"csdf");
	xmlTextWriterWriteAttribute (writer,(const xmlChar*)"xsi:noNamespaceSchemaLocation",(const xmlChar*) "http://www.es.ele.tue.nl/sdf3/xsd/sdf3-csdf.xsd");
	{
		xmlTextWriterSetIndent(writer,2); xmlTextWriterStartElement(writer,(const xmlChar*) "applicationGraph");

		const std::string app_name = dataflow->getAppName();
		const std::string graph_name = dataflow->getGraphName();
		const std::string graph_type = dataflow->getGraphType();
		xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) app_name.c_str());

		{
			xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "csdf");
			xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) graph_name.c_str());
			xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) graph_type.c_str());

			{
				for (Vertex t : dataflow->vertices()) {
					const std::string vertex_name = dataflow->getVertexName(t);
					const std::string vertex_type = dataflow->getVertexType(t);
					xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "actor");
					xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) vertex_name.c_str());
					xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) vertex_type.c_str());



					for (auto it : dataflow->in_edges(t)) {
									Edge e = *it;
									const std::string out_port =  "out_" + commons::toString(dataflow->getEdgeId(e));
									const auto in_init_rates = dataflow->getEdgeInitOutVector(e);
									const auto in_rates = dataflow->getEdgeOutVector(e);
									xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "port");
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) out_port.c_str());
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) "in");
									std::string rates_str = vectorAsStr(in_rates);
									if (in_init_rates.size()) {
										rates_str = vectorAsStr(in_init_rates) + INIT_PERIODIC_SEPARATOR + rates_str;
									}
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"rate", (const xmlChar*) rates_str.c_str());
									xmlTextWriterEndElement(writer);
					}

					for (auto it : dataflow->out_edges(t)) {
									Edge e = *it;
									const std::string in_port =  "in_" + commons::toString(dataflow->getEdgeId(e));
									const auto out_init_rates = dataflow->getEdgeInitInVector(e);
									const auto out_rates = dataflow->getEdgeInVector(e);
									xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "port");
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"name", (const xmlChar*) in_port.c_str());
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) "out");
									std::string rates_str = vectorAsStr(out_rates);
									if (out_init_rates.size()) {
										rates_str = vectorAsStr(out_init_rates) + INIT_PERIODIC_SEPARATOR + rates_str;
									}
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"rate", (const xmlChar*) rates_str.c_str());
									xmlTextWriterEndElement(writer);
					}
					if (dataflow->getReentrancyFactor(t)) {
						writeLoopbackPorts(writer,dataflow,t);
					}

					xmlTextWriterEndElement(writer);
				}


				for (auto it : dataflow->edges()) {
					Edge e = *it;
					writeChannel(writer,dataflow,e);
				}

				for (Vertex t : dataflow->vertices()) {
					if (dataflow->getReentrancyFactor(t)) {
						writeLoopbackChannel(writer,dataflow,t);
					}
				}
			}
			xmlTextWriterEndElement(writer);
			xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "csdfProperties");
			{
				for (Vertex t : dataflow->vertices()) {
							const std::string vertex_name = dataflow->getVertexName(t);
							xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "actorProperties");
							xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"actor", (const xmlChar*) vertex_name.c_str());
							{

								xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "processor");
								xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"type", (const xmlChar*) "cluster_0");
								xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"default", (const xmlChar*) "true");
								{

									auto periodicExecutionTime = dataflow->getVertexPhaseDuration(t);
									auto initExecutionTime = dataflow->getVertexInitPhaseDuration(t);
									std::string times_str = vectorAsStr(periodicExecutionTime);
									if (initExecutionTime.size()) {
										times_str = vectorAsStr(initExecutionTime) + INIT_PERIODIC_SEPARATOR + times_str;
									}
									xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "executionTime");
									xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"time", (const xmlChar*)  times_str.c_str());
									xmlTextWriterEndElement(writer);
								}
								xmlTextWriterEndElement(writer);
							}
							xmlTextWriterEndElement(writer);
				}
				for (auto it : dataflow->edges()) {
					Edge e = *it;
					std::string channel_name = dataflow->getEdgeName(e);
					xmlTextWriterSetIndent(writer,3); xmlTextWriterStartElement(writer,(const xmlChar*) "channelProperties");
					xmlTextWriterWriteAttribute	(writer,(const xmlChar*)"channel", (const xmlChar*) channel_name.c_str());
					xmlTextWriterEndElement(writer);
				}
			}
			xmlTextWriterEndElement(writer);
		}
		xmlTextWriterEndElement(writer);
	}
	xmlTextWriterEndElement(writer);

	xmlTextWriterEndDocument(writer);
	xmlFreeTextWriter(writer);
	std::string res = (const char*)buf->content;
    xmlBufferFree(buf);
    return res;
}


void writeSDF3File         (std::string filename, const models::Dataflow* dataflow)  {
	std::ofstream mfile;
	mfile.open (filename);
	mfile << generateSDF3XML(dataflow);
	mfile.close();

}

} // end of namespace commons

