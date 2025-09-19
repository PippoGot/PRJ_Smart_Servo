%% Simulation Parameters Preset for Estimation Staircase Reference and Controller
% Need to choose driver, motor and sensors blocks

sim_params.filepath = 'Full_Model'; % simulation model filepath             [string]
sim_params.Tsample = 1e-3;          % sampling time                         [s]
sim_params.Tstop = 180;             % simulation stop time                  [s]


sim_blocks.ref.model_type = 'sequence'; 
sim_blocks.ref.Tint = 5;                       
sim_blocks.ref.values = 10*[0.2:0.1:1, 1:-0.1:0.2,  -(0.2:0.1:1), -(1:-0.1:0.2)]';


sim_blocks.controller.strategy = 'PID-velocity';
sim_blocks.controller.Kp = 0.012;
sim_blocks.controller.Ki = 5;
sim_blocks.controller.Kd = 0;


sim_blocks.observer.strategy = 'none';
