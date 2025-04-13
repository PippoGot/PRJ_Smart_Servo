%% LQR Optimal Controller Tuning

%% Position Controller
% Augmented system with error space
ctrl_lqr.Fa = [motor.F, zeros(3, 1); -motor.h1, 0];
ctrl_lqr.Ga = [motor.G; 0];
ctrl_lqr.Gref = [zeros(3, 1); 1];

% Q-R matrices
ctrl_lqr.Q = [5e2, 0, 0, 0;
    0, 1, 0, 0;
    0, 0, 1, 0;
    0, 0, 0, 8e3];
ctrl_lqr.R = 1;

% Gain computation
ctrl_lqr.K = lqr(ctrl_lqr.Fa, ctrl_lqr.Ga, ctrl_lqr.Q, ctrl_lqr.R);
ctrl_lqr.Ke = ctrl_lqr.K(4);
ctrl_lqr.Kx = ctrl_lqr.K(:, 1:3);

% Simulation run
% sim("Test_Model.slx");


%% Speed Controller (NEED TO TEST)
% Augmented system with error space
% ctrl_lqr.Fa = [motor.F, zeros(3, 1); -[0, 1, 0], 0];
% ctrl_lqr.Ga = [motor.G; 0];
% ctrl_lqr.Gref = [zeros(3, 1); 1];
% 
% % Q-R matrices
% ctrl_lqr.Q = [100, 0, 0, 0;
%     0, 1e-4, 0, 0;
%     0, 0, 1e-4, 0;
%     0, 0, 0, 1e4];
% ctrl_lqr.R = 1e-2;
% 
% % Gain computation
% ctrl_lqr.K = lqr(ctrl_lqr.Fa, ctrl_lqr.Ga, ctrl_lqr.Q, ctrl_lqr.R);
% ctrl_lqr.Ke = ctrl_lqr.K(4);
% ctrl_lqr.Kx = ctrl_lqr.K(:, 1:3);

% Simulation run
% sim("Full_Model.slx");
