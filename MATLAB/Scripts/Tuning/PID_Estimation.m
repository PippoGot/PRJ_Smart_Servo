%% PID Parameter Tuning for Estimation Experiments
% Continuous Time
motor_params;

% Compute System TF
SYS = ss(motor.F, motor.G, [0 1 0], 0);
[NUM, DEN] = tfdata(SYS);
TF = tf(NUM, DEN);

% Controller Specs
Mp = 0.01;
% delta = sqrt(2) / 2;
delta = log(1/Mp) / sqrt(pi^2 + log(1/Mp)^2);
wgc = 100;
phi_m = atan(2*delta/sqrt(sqrt(1 + 4*delta^4)-2*delta^2));
alpha = 10;

% TF Parameters @ wgc
[MAG, PHASE] = bode(TF, wgc);
delta_K = MAG^-1;
delta_phi = phi_m - pi - deg2rad(PHASE);

% PID Parameters
Kp = delta_K * cos(delta_phi);

Td = (tan(delta_phi) + sqrt(tan(delta_phi)^2 + 4 / alpha)) / (2 * wgc);
% Kd = Td;
Kd = 0;

Ti = alpha * Td;
% Ki = Ti^-1;
Ki = -wgc * delta_K * sin(delta_phi);
