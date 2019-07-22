/*
 * SDF3Wrapper.cpp
 *
 *  Created on: 15 mars 2012
 *      Author: toky
 *
 *
 */

#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/parser.h>

#include <commons/SDF3Wrapper.h>
#include <commons/commons.h>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <iostream>
#include <cstring>
#include <fstream>

using std::vector;

#define TXT_XML_ERROR "TXT_XML_ERROR"

// TODO add timing reader for SDF3

namespace commons {



    template <typename T1,typename T2>
    std::pair<T1,T2> splitAndDefault(const std::string &s,char del,T2 d) {
        std::vector<std::string> sub;
        std::pair<T1,T2>  res;
        sub = split (s, del);

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

        std::vector<std::string>  phases = split (rates, ',');
        std::vector<TOKEN_UNIT>   cons;
        for (unsigned int i = 1 ; i <= phases.size() ; i++) {
            	   cons.push_back(commons::fromString<TOKEN_UNIT>(phases[i-1]));
        }
        to->setEdgeOutPhases(c,cons);

    }

    void                        readSDF3InputSpec      (models::Dataflow *to, const Edge c, const std::string rates) {

        std::vector<std::string>  phases = split (rates, ',');
        std::vector<TOKEN_UNIT>   prod;
        for (unsigned int i = 1 ; i <= phases.size() ; i++) {
        	prod.push_back(commons::fromString<TOKEN_UNIT>(phases[i-1]));
        }
        to->setEdgeInPhases(c,prod);
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
                         if (pEdge != NULL_EDGE)
                             readSDF3OutputSpec(to,pEdge,rate);
                     } else

                     if (type == "out") {
                         Edge pEdge = to->getOutputEdgeByPortName(pVertex,name);
                         if (pEdge != NULL_EDGE)
                             readSDF3InputSpec(to,pEdge,rate);
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


        std::vector<std::string> list = splitSDF3List(timings);
        std::vector<TIME_UNIT> times;
        to->setPhasesQuantity(pVertex,list.size());
        //to->initVertexDuration(pVertex,1);
        for (unsigned int i = 1 ; i <= list.size() ; i++) {
        	times.push_back(commons::fromString<TIME_UNIT>(list[i-1]));
            //to->setVertexDuration(pVertex,(EXEC_COUNT)i,commons::fromString<TIME_UNIT>(list[i-1]));
        }
        to->setVertexDuration(pVertex,times);

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

    bool onlyOneRate(std::string rate) {
        // '1' and ',' alternate
        bool val = (rate[0] == '1') ? true : false;
        for (unsigned int i = 0 ; i < rate.size() ; i++) {
            if (!(val?rate[i]=='1':rate[i]==',')) return false;
            val = !val;
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

        if (sdf3 == NULL) {
        	FAILED("Document XML invalide");
        }

        if (std::string((const char*)sdf3->name) != "sdf3") {
        	FAILED("Document XML invalide");
        }


        for (xmlNodePtr cur_node = sdf3->children; cur_node; cur_node = cur_node->next) {
                if (cur_node->type == XML_ELEMENT_NODE) {
                    if (std::string((const char*)cur_node->name) == std::string("applicationGraph")) AG = cur_node;
                }
        }

        if (!AG) {
        	FAILED("Document XML invalide");
        }

        for (xmlAttrPtr cur_attr = AG->properties; cur_attr; cur_attr = cur_attr->next) {
            if (strcmp((const char*)cur_attr->name,"name") == 0) {

                to->setName((const char*)cur_attr->children->content);
            }
        }

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


        // STEP 1 - Generate Vertex list with good names

        for (xmlNodePtr cur_node = csdf->children; cur_node; cur_node = cur_node->next) {
             if (cur_node->type == XML_ELEMENT_NODE) {

                 if (std::string((const char*)cur_node->name) == std::string("actor")) {
                     Vertex newVertex = to->addVertex();
                     for (xmlAttrPtr cur_attr = cur_node->properties; cur_attr; cur_attr = cur_attr->next) {
                         if (strcmp((const char*)cur_attr->name,"name") == 0) {

                             to->setVertexName(newVertex,(const char*)cur_attr->children->content);
                             to->setReentrancyFactor(newVertex,0); // par defaut une tâche SDF3 est reetrante à l'infini
                         }
                     }
                 }
             }
        }

        // STEP 2 - Generate Edge list with good names

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

        // STEP 4 - get task timings
        for (xmlNodePtr cur_node = csdfproperties->children; cur_node; cur_node = cur_node->next) {
                    if (cur_node->type == XML_ELEMENT_NODE) {

                        if (std::string((const char*)cur_node->name) == std::string("actorProperties")) {
                        	 readSDF3VertexTimings(to,cur_node);
                        	 readSDF3VertexReentrancy(to,cur_node); //fail function
                            }
                        }
                    }

        // STEP 3 - get task phase count and productions
        for (xmlNodePtr cur_node = csdf->children; cur_node; cur_node = cur_node->next) {
                    if (cur_node->type == XML_ELEMENT_NODE) {

                        if (std::string((const char*)cur_node->name) == std::string("actor")) {
                                    readSDF3VertexPorts(to,cur_node);
                        }
                    }
               }





        return to;
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


    std::string        writeSDF3File         (const models::Dataflow* )  {
      return "";
    }


} // end of namespace commons

