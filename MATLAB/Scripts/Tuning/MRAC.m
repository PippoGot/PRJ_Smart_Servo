%% MRAC Controller Tuning

LQR;

ctrl_mrac.P = lyap(ctrl_lqr.Fa-ctrl_lqr.Ga*ctrl_lqr.K, eye(4));
ctrl_mrac.B = ctrl_lqr.Gref;
ctrl_mrac.gamma_phi = 10;
ctrl_mrac.w0 = zeros(6, 1);

% Simulation run
% sim("Test_Model.slx");