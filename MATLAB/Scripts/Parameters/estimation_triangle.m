%% Simulation Parameters Preset for Estimation Staircase Reference and Controller
% Need to choose driver, motor and sensors blocks

sim_params.filepath = 'Full_Model';
sim_params.Tsample = 1e-3;
sim_params.Tstop = 50;


sim_blocks.ref.model_type = 'triangle';
sim_blocks.ref.frequency = 0.5;
sim_blocks.ref.amplitude = 5;
sim_blocks.ref.bias = 8;


sim_blocks.controller.strategy = 'PID-velocity';
sim_blocks.controller.Kp = 0.012;
sim_blocks.controller.Ki = 5;
sim_blocks.controller.Kd = 0;


sim_blocks.observer.strategy = 'none';
