clear; clc;
load('pc2.mat');

pcNum = size(pc, 1);
pcBuf = zeros(pcNum*2, 1);
pcBuf(1:2:end) = pc(:,1);
pcBuf(2:2:end) = pc(:,2);

fp = fopen('pc_400.bin', 'wb');
fwrite(fp, pcBuf, 'single');
fclose(fp);

