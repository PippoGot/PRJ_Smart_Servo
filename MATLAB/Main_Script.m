%% Initialization

clear
close all
clc

addpath(genpath('Data'));
addpath(genpath('Modular Models'));
addpath(genpath('Scripts'));
 
%% Model Parameters

motor_params;
components_params;

estimate.Kg = motor.Kg;


% Parameters Settings Reference (for documentation)
% sim_params.filepath = 'Full_Model'; % simulation model filepath             [string]
% sim_params.Tsample = 1e-3;          % sampling time                         [s]
% sim_params.Tstop = 180;             % simulation stop time                  [s]
% 
% 
% % Reference Signal
% sim_blocks.ref.model_type = 'sequence';            
% % reference block types + parameters needed 
% % 'step'        -> (final value, step time)
% % 'sine'        -> (frequency, amplitude, bias)
% % 'triangle'    -> (frequency, amplitude, bias)
% % 'square'      -> (frequency, amplitude, bias)
% % 'sequence'    -> (values vector, step duration)
% % 'none'        -> no parameters (step with value 0)
% 
% % Reference Parameters
% sim_blocks.ref.frequency = 2;                  % periodic signal frequency             [Hz]
% sim_blocks.ref.amplitude = 2;                  % periodic signal amplitude             [unit]
% sim_blocks.ref.bias = 1;                       % periodic signal offset                [unit]
% 
% sim_blocks.ref.Tint = 5;                       % repeating sequence step duration      [s]
% sim_blocks.ref.values = 10*[0.2:0.1:1, 1:-0.1:0.2,  -(0.2:0.1:1), -(1:-0.1:0.2)]';               % repeating sequence values vector      [unit]
% 
% sim_blocks.ref.Tstep = 0.1;                    % step reference time                   [s]
% sim_blocks.ref.value = 4;                      % step reference final value            [unit]
% 
% 
% % Controller Strategy
% sim_blocks.controller.strategy = 'PID-velocity';
% % controller block strategies + parameters needed
% % 'PID-position'    -> (Kp, Ki, Kd) not implemented
% % 'PID-velocity'    -> (Kp, Ki, Kd)
% % 'LQR-position'    -> not implemented
% % 'LQR-velocity'    -> not implemented
% % 'none'            -> no parameters (direct voltage drive)
% 
% % Controller Parameters
% sim_blocks.controller.Kp = 0.012;
% sim_blocks.controller.Ki = 5.3;
% sim_blocks.controller.Kd = 0;
% 
% 
% % Driver and Motor Models
% sim_blocks.system.driver_model = 'ideal';
% % driver block models
% % 'ideal'
% % 'DRV8830'
% 
% sim_blocks.system.motor_model = 'linear';
% % motor block models
% % 'linear'      non-linear friction not modeled
% % 'lugre'       non-linear friction modeled with state-space
% % 'simscape'    non-linear friction modeled with simscape library
% 
% 
% % Sensor Models
% sim_blocks.sensors.a_model = 'ideal';
% % position sensor block models
% % 'ideal'       measure is perfectly matching the system state
% % 'AS5600'      measure through the component
% % 'none'        no measurement
% 
% sim_blocks.sensors.w_model = 'ideal';
% % velocity sensor block models
% % 'ideal'       measure is perfectly matching the system state
% % 'filter'      measure through position filter (not implemented)
% % 'AH49E'       measure through custom sensor (not implemented)
% % 'none'        no measurement
% 
% sim_blocks.sensors.i_model = 'ideal';
% % current sensor block models
% % 'ideal'       measure is perfectly matching the system state
% % 'INA219'      measure through the component (not implemented)
% % 'none'        no measurement
% 
% sim_blocks.sensors.v_model = 'ideal';
% % voltage sensor block models
% % 'ideal'       measure is perfectly matching the system state
% % 'INA219'      measure through the component (not implemented)
% % 'none'        no measurement
% 
% 
% % Observer Strategy
% sim_blocks.observer.strategy = 'none';
% % observer block strategies
% % 'none'        no observer, measures directly ued for feedback
% % 'kalman'      kalman filter algorithm (not implemented)
% % 'extended'    extended kalman filter algorithm (not implemented)

%% Components Model Selection for Parameters Estimation

% model parameters
sim_blocks.system.driver_model = 'ideal';
sim_blocks.system.motor_model = 'modular';
sim_blocks.system.flux_weakening = 'on';
sim_blocks.system.coulomb_friction = 'on';
sim_blocks.system.stribeck_friction = 'on';
sim_blocks.sensors.a_model = 'AS5600'; 
sim_blocks.sensors.w_model = 'filter';
sim_blocks.sensors.i_model = 'ideal';
sim_blocks.sensors.v_model = 'ideal';

% simulation parameters
sim_params.filepath = 'Full_Model'; % simulation model filepath             [string]
sim_params.Tsample = 1e-3;          % sampling time                         [s]

% controller parameters
% sim_blocks.controller.strategy = 'PID-velocity';
% sim_blocks.controller.Kp = 0.2;
% sim_blocks.controller.Ki = 0.5;
% sim_blocks.controller.Kd = 0;

% reference parameters
% sim_params.Tstop = 140-1e-3;
% sim_blocks.ref.model_type = 'triangle';
% sim_blocks.ref.frequency = 0.05;
% sim_blocks.ref.amplitude = 28;
% sim_blocks.ref.bias = 0;


sim_blocks.controller.strategy = 'SWPID-position';
sim_blocks.controller.Kp = 25;
sim_blocks.controller.Ki = 2.4;
sim_blocks.controller.Kd = 0.1;

% observer parameters
sim_blocks.observer.strategy = 'none';




sim_params.Tstop = 10-1e-3;
sim_blocks.ref.model_type = 'step';
sim_blocks.ref.value = pi;
sim_blocks.ref.Tstep = 2;

% sim_params.Tstop = 541.5-1e-3;
% sim_blocks.ref.model_type = 'sequence';
% sim_blocks.ref.values = 0:pi/180:2*pi;
% sim_blocks.ref.Tint = 1.5;

sim_blocks.system.initial_conditions = zeros(4, 1);

datasim = simulate_system(sim_params, sim_blocks);
% save('./Data/PID_Velocity_TriangleFW+SB_Simulation.mat', "datasim");
stop

%% Simulate Model

% sine reference parameters
% sim_params.Tstop = 140-1e-3;
% sim_blocks.ref.model_type = 'sine';
% sim_blocks.ref.frequency = 0.05;
% sim_blocks.ref.amplitude = 25;
% sim_blocks.ref.bias = 0;
% 
% 
% sim_blocks.system.initial_conditions = [0; zeros(3, 1)];
% datasim = simulate_system(sim_params, sim_blocks);
% stop


% staircase reference parameters
sim_params.Tstop = 150-1e-3;
sim_blocks.ref.model_type = 'sequence';
sim_blocks.ref.values = [0, 4:4:28, -(4:4:28)];
sim_blocks.ref.Tint = 10;


sim_blocks.system.initial_conditions = [5.9702; zeros(3, 1)];
datasim = simulate_system(sim_params, sim_blocks);
[DATASIM_stair_r, DATASIM_stair_m] = sim_to_std(datasim);
% save('./Data/PID_Velocity_Staircase_Simulation.mat', "datasim");


% triangle reference parameters
sim_params.Tstop = 140-1e-3;
sim_blocks.ref.model_type = 'triangle';
sim_blocks.ref.frequency = 0.05;
sim_blocks.ref.amplitude = 28;
sim_blocks.ref.bias = 0;


sim_blocks.system.initial_conditions = [5.3996; zeros(3, 1)];
datasim = simulate_system(sim_params, sim_blocks);
[DATASIM_triangle_r, DATASIM_triangle_m] = sim_to_std(datasim);
% save('./Data/PID_Velocity_Triangle_Simulation.mat', "datasim");


clear datasim


%% Estimate System Parameters
ref_data.stair_periods = 15;
ref_data.stair_trim = 1e3;
ref_data.triangle_periods = 28;
ref_data.triangle_trim = 1.5e3;

estimate = system_id2(DATASIM_stair_r, DATASIM_triangle_r, estimate, ref_data);


%% Plot Differences

load('Data\PID_Velocity_Staircase_Experiment.mat')
DATASET_stair = exp_to_std(data);
load('Data\PID_Velocity_Triangle_Experiment.mat')
DATASET_triangle = exp_to_std(data);
clear data

ref_data.stair_periods = 15;
ref_data.stair_trim = 1e3;
ref_data.triangle_periods = 28;
ref_data.triangle_trim = 1.5e3;
estimate = system_id2(DATASET_stair, DATASET_triangle, estimate, ref_data);

% compare_model_trj(DATASIM_stair_m, DATASET_stair)
compare_model_trj(DATASIM_triangle_m, DATASET_triangle)

%%
close all

voltage = [DATASET_triangle.voltage, DATASIM_triangle_r.voltage];
% voltage_diff = voltage - DATASIM_triangle_r.voltage;

current = [sgolayfilt(DATASET_triangle.current, 3, 51), DATASIM_triangle_r.current];
% current_diff = current - DATASIM_triangle_r.current;

velocity = [sgolayfilt(gradient(DATASET_triangle.position)/1e-3, 3, 151), sgolayfilt(gradient(DATASIM_triangle_r.position)/1e-3, 3, 151)];
% velocity_diff = velocity - DATASIM_triangle_r.velocity;

acceleration = [sgolayfilt(gradient(velocity(:, 1))/1e-3, 5, 351), sgolayfilt(gradient(velocity(:, 2))/1e-3, 5, 351)];

% figure
% plot(voltage)
% title('Voltage')
% grid on
% figure
% plot(current)
% title('Current')
% grid on
% figure
% plot(velocity)
% title('Velocity')
% grid on
% figure
% plot(acceleration)
% title('Acceleration')
% grid on


flux = estimate.Kg*(voltage - estimate.Ra*current)./ velocity;
flux(isnan(flux)|isinf(flux)) = 0;

flux = [mean(reshape(flux(:, 1), 20000, []), 2), mean(reshape(flux(:, 2), 20000, []), 2)];
velocity = [mean(reshape(velocity(:, 1), 20000, []), 2), mean(reshape(velocity(:, 2), 20000, []), 2)];
acceleration = [mean(reshape(acceleration(:, 1), 20000, []), 2), mean(reshape(acceleration(:, 2), 20000, []), 2)];


% velocity(isinf(flux)|isnan(flux)|abs(flux)>1e-2|flux<5e-4) = [];
% acceleration(isinf(flux)|isnan(flux)|abs(flux)>1e-2|flux<5e-4) = [];
% flux(isinf(flux)|isnan(flux)|abs(flux)>1e-2|flux<5e-4) = [];



% flux(abs(flux) > 5e-3) = 0;
figure(1)
plot(velocity, flux)
grid on

figure(2)
plot3(flux, velocity, acceleration, ' .');
xlabel('Flux')
ylabel('Velocity')
zlabel('Acceleration')
xlim([0, 5e-2])
grid on

% figure(4)
% plot(velocity, flux.*sign(velocity))
% ylim([-5e-3, 5e-3])
% % xlim([0, 20000])
% grid on


