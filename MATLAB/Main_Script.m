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

% Speed filter params
wn = 100;
csi = 1/sqrt(2);

% PID Tuning
PID;

%% Change Simulink Model

%% Start Simulation/Experiment