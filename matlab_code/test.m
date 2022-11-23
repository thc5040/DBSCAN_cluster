clear; clc; clf; close all;
rng default; % For reproducibility
X = [randn(100,2)*0.22+0.5*ones(100,2);
    randn(100,2)*0.22-0.5*ones(100,2);
    randn(100,2)*0.22+[-0.5*ones(100,1), 0.8*ones(100,1)];
    randn(100,2)*0.22+[-1.3*ones(100,1), 0.2*ones(100,1)];
    ];

figure;
plot(X(:,1),X(:,2),'.');
title 'Randomly Generated Data';