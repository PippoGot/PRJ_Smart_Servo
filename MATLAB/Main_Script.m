%% Initialization
clear
close all
clc

addpath(genpath('Data'));
addpath(genpath('Modular Models'));
addpath(genpath('Scripts'));

%% Parameters Settings

motor_params;
AS5600_params;
INA219_params;

% Low Speed Tachometer Parameters
s = tf('s');
wn = 200;               % cutoff frequency [Hz]
csi = 1/sqrt(2);        % damping coefficient

lowpass = s / (s^2 + 2*csi*wn*s + wn^2);
[LP_NUM, LP_DEN] = tfdata(lowpass);
LP_NUM = cell2mat(LP_NUM);
LP_DEN = cell2mat(LP_DEN);

[LP_MAG, LP_PHASE] = bode(lowpass, 1);
LP_GAIN = 1/LP_MAG;

noise_pwr = 1e-7;

sim("Full_Model.slx");

%% Controller Tuning

LQR;
% MRAC;


%% Change Simulink Model

%% Start Simulation Experiment

% Phase_Space_Plotter




