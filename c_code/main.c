#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "dbscan.h"

#define POINT_NUM   400

point_t pointArray[POINT_NUM];
int8_t clusterIdx[POINT_NUM];
uint8_t scratch[100*1024*1024];

clusterInfo_t clusterInfo[20];


int main(void)
{
    FILE* fp = fopen("pc_400.bin", "rb");
    if(fp == NULL) {
        printf("open file error.\n");
        return 0;
    }
    fread(pointArray, sizeof(pointArray), 1, fp);
    fclose(fp);

    uint8_t clusterNum;

clusterNum = dbscan(pointArray, 
                POINT_NUM,
                10,
                0.23*0.23,
                20,
                clusterIdx,
                clusterInfo,
                scratch);

    for(int i=0; i<clusterNum; i++)
    {
        printf("%d, %f, %f\n", clusterInfo[i].pcNum, clusterInfo[i].center_x, clusterInfo[i].center_y);
    }

    return 0;
}



/* ------------------ the second example --------------------*/

// #define POINT_NUM   15000

// point_t pointArray[POINT_NUM];
// int8_t clusterIdx[POINT_NUM];
// uint8_t scratch[100*1024*1024];

// clusterInfo_t clusterInfo[20];


// int main(void)
// {
//     FILE* fp = fopen("pc.bin", "rb");
//     if(fp == NULL) {
//         printf("open file error.\n");
//         return 0;
//     }
//     fread(pointArray, sizeof(pointArray), 1, fp);
//     fclose(fp);

//     uint8_t clusterNum;

// clusterNum = dbscan(pointArray, 
//                 POINT_NUM,
//                 30,
//                 2*2,
//                 20,
//                 clusterIdx,
//                 clusterInfo,
//                 scratch);

//     for(int i=0; i<clusterNum; i++)
//     {
//         printf("%d, %f, %f\n", clusterInfo[i].pcNum, clusterInfo[i].center_x, clusterInfo[i].center_y);
//     }

//     return 0;
// }