/*
brief : c implementation of DBSCAN clustring algorithm
author : SZU, N103, thc
date  : 2022-11-21
*/

#ifndef __DBSCAN_H
#define __DBSCAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>



/* 2 dimention */
typedef struct 
{
    float x;
    float y;
}point_t;

/* cluster information */
typedef struct 
{
    float center_x;
    float center_y;
    uint32_t pcNum;
}clusterInfo_t;


uint8_t dbscan(point_t* pointArray, 
                uint16_t numPoint,
                uint16_t mpts,
                float eps2,
                uint8_t numMaxCluster,
                int8_t* clusterIdxArray,
                clusterInfo_t* clusterInfoArray,
                uint8_t* scratch);



#endif