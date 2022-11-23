clear; clc;
load('pc_400.mat') 

[clusterIdx, ] = dbscan(pc, 0.23, 10, 'Distance', 'euclidean');
gscatter(pc(:,1), pc(:,2), clusterIdx);
grid;

clusterNum = size(find(unique(clusterIdx) > 0), 1);

clusterInfo = [];
for i=1:clusterNum
   idx = find(clusterIdx(:) == i);
   pointCluster = pc(idx, :);
   
   clusterInfo(i,1).pcNum = size(pointCluster, 1);
   clusterInfo(i,1).center = mean(pointCluster, 1);
end
