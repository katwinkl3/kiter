/*
 * phase_schedule_minmax.cpp
 *
 *  Created on: 17 juin 2013
 *      Author: toky
 */

#include "phase_schedule_minmax.h"
/*
bool algorithms::PhaseScheduleMinMax_Algo(models::Dataflow * dataflow,Vertex t, std::vector<TIME_UNIT>& res) {

    res.clear();
    TIME_UNIT OMEGA = dataflow->getPeriod();

    res.push_back(0);

    const EXEC_COUNT  max_k       = dataflow->getPhasesQuantity(t);
    const TIME_UNIT   mut         = OMEGA  * ((TIME_UNIT) max_k / (TIME_UNIT) dataflow->getNi(t));

    int i,j,k,temp1;
    double somme=0;
    double tmp=0;
    Edge pta;
    const TOKEN_UNIT        wa           = dataflow->getEdgeIn(pta);
    const TOKEN_UNIT gcda = dataflow->getFineGCD(pta);
    int tab_ind_w[max_k];
    double  tab_w[max_k];

    int tab_ind_v[max_k];
    double  tab_v[max_k];

    pta = 0;// premier arc en entrée
    // pta=ptrG->tab_transitions[index].liste_in;

    tab_ind_w[0]=0;
    tab_w[0]=(dataflow->getVertexDuration(t,1));
    tab_ind_v[0]=0;
    tab_v[0]=((dataflow->getEdgeInPhase(pta,1)*mut)/wa;
    somme=tab_v[0];
    int sommeb=dataflow->getEdgeInPhase(pta,1);

    for(i=1;i<max_k;i++)
    {
        const TIME_UNIT lti = dataflow->getVertexDuration(t,i);
        const TIME_UNIT ltip = dataflow->getVertexDuration(t,i+1);
        res.push_back(res.back() + lti);
        tmp=ltip-((((sommeb/gcda)*gcda)*mut)/wa)+res.back();
        for(j=0;j<i;j++)
        {
            if(tmp<tab_w[j]) break;
        }
        if(j==i)
        {
            tab_ind_w[j]=i;
            tab_w[j]=tmp;
        }
        else
        {
            for(k=i;k>j;k--)
            {
                tab_ind_w[k]=tab_ind_w[k-1];
                tab_w[k]=tab_w[k-1];
            }
            tab_ind_w[j]=i;
            tab_w[j]=tmp;
        }

        int wip = dataflow->getEdgeInPhase(pta,i+1)
                    somme=somme+((wip)*mut)/wa;
        sommeb=sommeb+(wip);
        tmp=somme-res.back();
        for(j=0;j<i;j++)
        {
            if(tmp<tab_v[j]) break;
        }
        if(j==i)
        {
            tab_ind_v[j]=i;
            tab_v[j]=tmp;
        }
        else
        {
            for(k=i;k>j;k--)
            {
                tab_ind_v[k]=tab_ind_v[k-1];
                tab_v[k]=tab_v[k-1];
            }
            tab_ind_v[j]=i;
            tab_v[j]=tmp;
        }
    }

    //Amax
    double Amax=tab_w[max_k-1];
    int ind1=max_k;
    int ind2=tab_ind_v[max_k-1];
    double d=mut- res[max_k-1] - dataflow->getVertexDuration(t,max_k);
    printf("d=%f\n",d);
    do
    {
        for(k=0;k<=max_k-1;k++)
        {
            if(tab_ind_w[k]>=ind2 && tab_ind_w[k]<ind1)
            {
                if((Amax-tab_w[k])<d) d=Amax-tab_w[k];
            }
        }
        printf("d=%f\n",d);

        for(k=max_k-1;k>=0;k--)
        {
            if(tab_ind_w[k]>=ind2  && tab_ind_w[k]<ind1)
            {
                res[tab_ind_w[k]] = res[tab_ind_w[k]] +d;
                tab_w[k]=tab_w[k]+d;
                for(j=k;j<max_k-1;j++)
                {
                    if(tab_w[j]>tab_w[j+1])
                    {
                        tmp=tab_w[j];
                        tab_w[j]=tab_w[j+1];
                        tab_w[j+1]=tmp;
                        temp1=tab_ind_w[j];
                        tab_ind_w[j]=tab_ind_w[j+1];
                        tab_ind_w[j+1]=temp1;
                    }
                }
            }
        }

        for(k=0;k<=max_k-1;k++)
        {
            if(tab_ind_v[k]>=ind2  && tab_ind_v[k]<ind1)
            {
                tab_v[k]=tab_v[k]-d;
                for(j=k;j>0;j--)
                {
                    if(tab_v[j]<tab_v[j-1])
                    {
                        tmp=tab_v[j];
                        tab_v[j]=tab_v[j-1];
                        tab_v[j-1]=tmp;
                        temp1=tab_ind_v[j];
                        tab_ind_v[j]=tab_ind_v[j-1];
                        tab_ind_v[j-1]=temp1;
                    }
                }
            }
        }

        ind1=ind2;
        ind2=tab_ind_v[max_k-1];
    }
    while(ind2<ind1);

    return false;
}
*/
