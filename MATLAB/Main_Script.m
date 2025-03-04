%% Initialization
clear
close all
clc

addpath(genpath('Data'));
addpath(genpath('Inspectors'));
addpath(genpath('Modular Models'));
addpath(genpath('Scripts'));

%% Parameters Settings

motor_params;
AS5600_params;
INA219_params;

% Sign function approximation
lambda = 500;

% Speed filter params
wn = 100;
csi = 1/sqrt(2);


%% Controller Tuning

% Optimal controller
LQR;

% PID Tuning

%% Change Simulink Model

%% Start SimulationExperiment