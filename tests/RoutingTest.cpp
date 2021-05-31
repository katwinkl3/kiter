#define BOOST_TEST_MODULE RoutingTest
#include "helpers/test_classes.h"


#include <commons/basic_types.h>
#include <algorithms/schedulings.h>

BOOST_FIXTURE_TEST_SUITE( RoutingTest , WITH_VERBOSE)

// BOOST_AUTO_TEST_CASE( RoutingTest )
// {
//     models::Dataflow* g = generateSample ();
//     double idx = 0;
//     std::vector<std::vector<edge_id_t>> tests;
//     tests.push_back({133,86});
//     tests.push_back({86,124,65,86});
//     ForEachEdge(g, e){
//         BOOST_REQUIRE_EQUAL( g->getRoute(e), tests[idx]);
//         idx++;
//     }
    

// }

// BOOST_AUTO_TEST_CASE( MappingTest )
// {
//     models::Dataflow* g = generateSample ();
//     double idx = 0;
//     std::vector<std::pair<node_id_t,node_id_t>> tests;
//     tests.push_back({10,9});
//     tests.push_back({2,3});
//     ForEachEdge(g, e){
//         BOOST_REQUIRE_EQUAL( g->getMapping(g->getEdgeSource(e)), tests[idx]);
//         idx++;
//     }
    

// }

BOOST_AUTO_TEST_SUITE_END()