/*
 * normalization.h
 *
 *  Created on: 10 mars 2011
 *      Author: toky
 */

#ifndef NORMALIZATION_H_
#define NORMALIZATION_H_
namespace commons { class GLPSol;   }
namespace models  { class Dataflow; }

namespace algorithms {
    /**
     * \fn bool normalize(models::Dataflow *from );
     * \brief Given a dataflow in parameter tests whether this dataflow
     * can be normalized or not. If yes, it sets Zi of each task and
     * updates initial data count (preload, or Mop) of each edge.
     * @param[out] from The dataflow to normalize
     * @return bool If from is normalizable, false if not.
     */

    bool normalize(models::Dataflow *from);
    bool check_validity (models::Dataflow *from, std::map<Vertex,TOKEN_UNIT> *  normalization );
    bool setNormalization(models::Dataflow *to, std::map<Vertex,TOKEN_UNIT> *  normalization);

    std::map<Vertex,TOKEN_UNIT> * gcdNormalize(models::Dataflow *from);
    std::map<Vertex,TOKEN_UNIT> * rationalNormalize(models::Dataflow *from);
    std::map<Vertex,TOKEN_UNIT> * newRationalNormalize(models::Dataflow *from);
    std::map<Vertex,TOKEN_UNIT> * sdfNormalize(models::Dataflow *from);


    std::map<Vertex,TOKEN_UNIT> * alphaNormalize(models::Dataflow *from);
    std::map<Vertex,TOKEN_UNIT> * tmpNormalize(models::Dataflow *from,bool);
    std::map<Vertex,TOKEN_UNIT> * oldNormalize(models::Dataflow *from);

}

#endif /* NORMALIZATION_H_ */
