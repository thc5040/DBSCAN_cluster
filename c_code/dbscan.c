#include "dbscan.h"

/* unvisited state*/
#define DBSCAN_UNVISITED_STATE  -1

/* visited state */
#define DBSCAN_VISITED_STATE    0

/* initial point */
#define DBSCAN_INIT_POINT       -2

/* noise point */
#define DBSCAN_NOISE_POINT      -1


/*
brief: dbscan clustering algorithm, pre calculate the distance to optimize time, but require more memory 

pointArray      : the 2d point array
numPoint        ：the number of point
mpts            : dbscan parameter, minimum number of neighbors required for a core point
eps2            : dbscan parameter, epsilon neighborhood of a point, square square square
numMaxCluster   : output, maximum number of clsuters
clusterIdxArray : output, cluster index, from 0 to clusterId-1, noise point is DBSCAN_NOISE_POINT
clusterInfoArray: output, cluster information
scratch         ：scratch buffer, require (6*numPoint) + (numPoint*numPoint) bytes

return result   : the numebr of clusters
*/
uint8_t dbscan(point_t* pointArray, 
                uint16_t numPoint,
                uint16_t mpts,
                float eps2,
                uint8_t numMaxCluster,
                int8_t* clusterIdxArray,
                clusterInfo_t* clusterInfoArray,
                uint8_t* scratch)
{
    uint32_t i, j, k, pointIdx;
    uint8_t clusterId;
    float ftemp1, ftemp2, d2;
    uint16_t epsPts;    /* the number of  */

    /* global buffer : mark point visited or unvisited state */
    int8_t* visited = (int8_t*)scratch;  
    scratch = &scratch[numPoint*sizeof(int8_t)];

    /* local buffer : avoid to add repeatly */
    uint8_t* addIn = (uint8_t*)scratch;  
    scratch = &scratch[numPoint*sizeof(uint8_t)];

    /* local buffer : save the neighbor, only unvisited point or noise point can be included */
    uint16_t neighborCnt;
    uint16_t* neighbor = (uint16_t*)scratch;
    scratch = &scratch[numPoint*sizeof(uint16_t)];

    /* temporary neighbor buffer */
    uint16_t newNeighborCnt;
    uint16_t* newNeighbor = (uint16_t*)scratch;
    scratch = &scratch[numPoint*sizeof(uint16_t)];

    /* pre calculate the distance */
    uint8_t* preCalDistance = (uint8_t*)scratch;
    scratch = &scratch[numPoint*numPoint*sizeof(uint8_t)];

    memset(preCalDistance, 0, numPoint*numPoint*sizeof(uint8_t));
    for(i=0; i<numPoint; i++)
    {
        preCalDistance[i*numPoint+i] = 1;
        for(j=i+1; j<numPoint; j++)
        {
            ftemp1 = pointArray[i].x - pointArray[j].x;
            ftemp2 = pointArray[i].y - pointArray[j].y;
            d2 = ftemp1*ftemp1 + ftemp2*ftemp2;
            if(d2 < eps2)
            {
                preCalDistance[i*numPoint+j] = 1;
            }
        }
    }

/* search preCalDistance */
#define isNeighbor(i, j) i<j ? preCalDistance[i*numPoint+j] : preCalDistance[j*numPoint+i]

    /* initial state */
    clusterId = 0;
    memset(visited, DBSCAN_UNVISITED_STATE, numPoint*sizeof(int8_t));
    memset(clusterIdxArray, DBSCAN_INIT_POINT, numPoint*sizeof(int8_t));

    for(i=0; i<numPoint; i++)
    {
        /* skip the visited point */
        if(visited[i] == DBSCAN_UNVISITED_STATE)
        {
            visited[i] = DBSCAN_VISITED_STATE;
            neighborCnt = 0;
            epsPts = 0;

            /* firstly find neighbor */
            for(j=0; j<numPoint; j++)
            {
                if(isNeighbor(i, j) == 1)
                {
                    epsPts++;
                    if(visited[j] == DBSCAN_UNVISITED_STATE || clusterIdxArray[j] == DBSCAN_NOISE_POINT)
                    {
                        neighbor[neighborCnt] = j;
                        neighborCnt++;
                    }
                }

            }

            if(epsPts < mpts)
            {
                /* mark it as noise, only this position mark point as noise */
                clusterIdxArray[i] = DBSCAN_NOISE_POINT;
            }else  /* (epsPts >= mpts) */
            {
                /* core point, start a new clsuter */
                clusterIdxArray[i] = clusterId;

                clusterInfoArray[clusterId].center_x = pointArray[i].x;
                clusterInfoArray[clusterId].center_y = pointArray[i].y;
                clusterInfoArray[clusterId].pcNum = 1;

                memset(addIn, 0, numPoint*sizeof(uint8_t));
                for(j=0; j<neighborCnt; j++)
                {
                    addIn[neighbor[j]] = 1;
                }

                /* iterator the neighbor */
                k = 0;
                while(k < neighborCnt)
                {
                    clusterIdxArray[neighbor[k]] = clusterId;

                    clusterInfoArray[clusterId].center_x += pointArray[neighbor[k]].x;
                    clusterInfoArray[clusterId].center_y += pointArray[neighbor[k]].y;
                    clusterInfoArray[clusterId].pcNum++;
                    
                    /* expand the cluster */
                    if(visited[neighbor[k]] == DBSCAN_UNVISITED_STATE)
                    {
                        visited[neighbor[k]] = DBSCAN_VISITED_STATE;
                        clusterIdxArray[neighbor[k]] = clusterId;

                        epsPts = 0;
                        newNeighborCnt = 0;
                        for(j=0; j<numPoint; j++)
                        {
                            if(isNeighbor(neighbor[k], j) == 1)
                            {
                                epsPts++;
                                if(visited[j] == DBSCAN_UNVISITED_STATE || clusterIdxArray[j] == DBSCAN_NOISE_POINT)
                                {
                                    newNeighbor[newNeighborCnt] = j;
                                    newNeighborCnt++;
                                }
                            }                       
                        }

                        /* pointArray[neighbor[k]] is a core point, now expand the neighbor */
                        if(epsPts >= mpts)
                        {
                            for(j=0; j<newNeighborCnt; j++)
                            {
                                if(addIn[newNeighbor[j]] == 0)
                                {
                                    addIn[newNeighbor[j]] = 1;
                                    neighbor[neighborCnt] = newNeighbor[j];
                                    neighborCnt++;
                                }
                            }
                        }

                    }

                    k++;
                }

                clusterId++;
            }
        }
    }

    for(i=0; i<clusterId; i++)
    {
        clusterInfoArray[i].center_x = clusterInfoArray[i].center_x / clusterInfoArray[i].pcNum;
        clusterInfoArray[i].center_y = clusterInfoArray[i].center_y / clusterInfoArray[i].pcNum;
    }

    return clusterId;
}



// /*
// brief: dbscan clustering algorithm

// pointArray      : the 2d point array
// numPoint        ：the number of point
// mpts            : dbscan parameter, minimum number of neighbors required for a core point
// eps2            : dbscan parameter, epsilon neighborhood of a point, square square square
// numMaxCluster   : output, maximum number of clsuters
// clusterIdxArray : output, cluster index, from 0 to clusterId-1, noise point is DBSCAN_NOISE_POINT
// clusterInfoArray: output, cluster information
// scratch         ：scratch buffer, require (6*numPoint) bytes

// return result   : the numebr of clusters
// */
// uint8_t dbscan(point_t* pointArray, 
//                 uint16_t numPoint,
//                 uint16_t mpts,
//                 float eps2,
//                 uint8_t numMaxCluster,
//                 int8_t* clusterIdxArray,
//                 clusterInfo_t* clusterInfoArray,
//                 uint8_t* scratch)
// {
//     uint32_t i, j, k, pointIdx;
//     uint8_t clusterId;
//     float ftemp1, ftemp2, d2;
//     uint16_t epsPts;    /* the number of  */

//     /* global buffer : mark point visited or unvisited state */
//     int8_t* visited = (int8_t*)scratch;  
//     scratch = &scratch[numPoint*sizeof(int8_t)];

//     /* local buffer : avoid to add repeatly */
//     uint8_t* addIn = (uint8_t*)scratch;  
//     scratch = &scratch[numPoint*sizeof(uint8_t)];

//     /* local buffer : save the neighbor, only unvisited point or noise point can be included */
//     uint16_t neighborCnt;
//     uint16_t* neighbor = (uint16_t*)scratch;
//     scratch = &scratch[numPoint*sizeof(uint16_t)];

//     /* temporary neighbor buffer */
//     uint16_t newNeighborCnt;
//     uint16_t* newNeighbor = (uint16_t*)scratch;
//     scratch = &scratch[numPoint*sizeof(uint16_t)];    

//     /* initial state */
//     clusterId = 0;
//     memset(visited, DBSCAN_UNVISITED_STATE, numPoint*sizeof(int8_t));
//     memset(clusterIdxArray, DBSCAN_INIT_POINT, numPoint*sizeof(int8_t));

//     for(i=0; i<numPoint; i++)
//     {
//         /* skip the visited point */
//         if(visited[i] == DBSCAN_UNVISITED_STATE)
//         {
//             visited[i] = DBSCAN_VISITED_STATE;
//             neighborCnt = 0;
//             epsPts = 0;

//             /* firstly find neighbor */
//             for(j=0; j<numPoint; j++)
//             {
//                 ftemp1 = pointArray[i].x - pointArray[j].x;
//                 ftemp2 = pointArray[i].y - pointArray[j].y;
//                 d2 = ftemp1*ftemp1 + ftemp2*ftemp2;

//                 if(d2 < eps2)
//                 {
//                     epsPts++;
//                     if(visited[j] == DBSCAN_UNVISITED_STATE || clusterIdxArray[j] == DBSCAN_NOISE_POINT)
//                     {
//                         neighbor[neighborCnt] = j;
//                         neighborCnt++;
//                     }
//                 }
//             }

//             if(epsPts < mpts)
//             {
//                 /* mark it as noise, only this position mark point as noise */
//                 clusterIdxArray[i] = DBSCAN_NOISE_POINT;
//             }else  /* (epsPts >= mpts) */
//             {
//                 /* core point, start a new clsuter */
//                 clusterIdxArray[i] = clusterId;

//                 clusterInfoArray[clusterId].center_x = pointArray[i].x;
//                 clusterInfoArray[clusterId].center_y = pointArray[i].y;
//                 clusterInfoArray[clusterId].pcNum = 1;

//                 memset(addIn, 0, numPoint*sizeof(uint8_t));
//                 for(j=0; j<neighborCnt; j++)
//                 {
//                     addIn[neighbor[j]] = 1;
//                 }

//                 /* iterator the neighbor */
//                 k = 0;
//                 while(k < neighborCnt)
//                 {
//                     clusterIdxArray[neighbor[k]] = clusterId;

//                     clusterInfoArray[clusterId].center_x += pointArray[neighbor[k]].x;
//                     clusterInfoArray[clusterId].center_y += pointArray[neighbor[k]].y;
//                     clusterInfoArray[clusterId].pcNum++;
                    
//                     /* expand the cluster */
//                     if(visited[neighbor[k]] == DBSCAN_UNVISITED_STATE)
//                     {
//                         visited[neighbor[k]] = DBSCAN_VISITED_STATE;
//                         clusterIdxArray[neighbor[k]] = clusterId;

//                         epsPts = 0;
//                         newNeighborCnt = 0;
//                         for(j=0; j<numPoint; j++)
//                         {
//                             ftemp1 = pointArray[neighbor[k]].x - pointArray[j].x;
//                             ftemp2 = pointArray[neighbor[k]].y - pointArray[j].y;
//                             d2 = ftemp1*ftemp1 + ftemp2*ftemp2;

//                             if(d2 < eps2)
//                             {
//                                 epsPts++;
//                                 if(visited[j] == DBSCAN_UNVISITED_STATE || clusterIdxArray[j] == DBSCAN_NOISE_POINT)
//                                 {
//                                     newNeighbor[newNeighborCnt] = j;
//                                     newNeighborCnt++;
//                                 }
//                             }                            
//                         }

//                         /* pointArray[neighbor[k]] is a core point, now expand the neighbor */
//                         if(epsPts >= mpts)
//                         {
//                             for(j=0; j<newNeighborCnt; j++)
//                             {
//                                 if(addIn[newNeighbor[j]] == 0)
//                                 {
//                                     addIn[newNeighbor[j]] = 1;
//                                     neighbor[neighborCnt] = newNeighbor[j];
//                                     neighborCnt++;
//                                 }
//                             }
//                         }

//                     }

//                     k++;
//                 }

//                 clusterId++;
//             }
//         }
//     }

//     for(i=0; i<clusterId; i++)
//     {
//         clusterInfoArray[i].center_x = clusterInfoArray[i].center_x / clusterInfoArray[i].pcNum;
//         clusterInfoArray[i].center_y = clusterInfoArray[i].center_y / clusterInfoArray[i].pcNum;
//     }

//     return clusterId;
// }
