function [estimated_params, data] = estimate_params2(dataset, known_params)
%ESTIMATE_PARAMS Summary of this function goes here
%   Detailed explanation goes here

estimated_params.Kg = known_params.Kg;


%% Real Derivative and Discrete Filter Parameters
wc = 2*pi*10;                                               % cutoff frequency
delta = 1 / sqrt(2);                                        % damping factor
s = tf('s');                                                % s variable
derivative = (wc^2*s)/(s^2 + 2*wc*delta*s + wc^2);          % derivative transfer function
filter = tf(0.00995, [1, -0.99005], 1e-3);

Time = dataset.time;


%% Data Loading
Voltage = dataset.voltage;
Position = dataset.position;
Velocity = lsim(filter, dataset.velocity, Time);
Current = dataset.current;

Acceleration = lsim(filter*filter, lsim(derivative, Velocity, Time), Time);
CurrentD = lsim(filter*filter, lsim(derivative, Current, Time), Time);

P = symmetric_trim(stack_periods(Position', 10)', 1000);
A = symmetric_trim(stack_periods(Acceleration', 10)', 1000);
ID = symmetric_trim(stack_periods(CurrentD', 10)', 1000);


figure(4)
plot(A)
title('Acceleration')
figure(5)
plot(ID)
title('CurrentD')


%% Sample Trimming
V = symmetric_trim(stack_periods(Voltage', 10)', 1000);
W = symmetric_trim(stack_periods(Velocity', 10)', 1000);
I = symmetric_trim(stack_periods(Current', 10)', 1000);

data = zeros(8000, 10, 3);
data(:, :, 1) = V;
data(:, :, 2) = W;
data(:, :, 3) = I;


figure(1)
plot(data(:, :, 1))
title('Voltage')
figure(2)
plot(data(:, :, 2))
title('Velocity')
figure(3)
plot(data(:, :, 3))
title('Current')


%% Least Square Filtering
X = (1:1:8000)';
U = ones(8000, 1);

Phi = [X, U];
Phi2 = [X.*X, X, U];

% position
p_eqn = zeros(10, 3);
for i = 1:1:10
    p_eqn(i, :) = Phi2\P(:, i);
end
p_a = mean(reshape(p_eqn(:, 1), 2, []), 2);
p_b = mean(reshape(p_eqn(:, 2), 2, []), 2);
p_c = mean(reshape(p_eqn(:, 3), 2, []), 2);

% voltage
v_eqn = zeros(10, 2);
for i = 1:1:10
    v_eqn(i, :) = Phi\data(:, i, 1);
end
v_a = mean(reshape(v_eqn(:, 1), 2, []), 2);
v_b = mean(reshape(v_eqn(:, 2), 2, []), 2);
voltage = (v_a*X' + v_b*U')';
figure(1)
hold on
plot(voltage, 'r', 'LineWidth', 2)
hold off

% velocity
w_eqn = zeros(10, 2);
for i = 1:1:10
    w_eqn(i, :) = Phi\data(:, i, 2);
end
w_a = mean(reshape(w_eqn(:, 1), 2, []), 2);
w_b = mean(reshape(w_eqn(:, 2), 2, []), 2);
velocity = (w_a*X' + w_b*U')';
velocity2 = (2*p_a*X' + p_b*U')'/1e-3;
figure(2)
hold on
plot(velocity, 'r', 'LineWidth', 2)
plot(velocity2, 'b', 'LineWidth', 2)
hold off

% current
i_eqn = zeros(10, 2);
for i = 1:1:10
    i_eqn(i, :) = Phi\data(:, i, 3);
end
i_a = mean(reshape(i_eqn(:, 1), 2, []), 2);
i_b = mean(reshape(i_eqn(:, 2), 2, []), 2);
current = (i_a*X' + i_b*U')';
figure(3)
hold on
plot(current, 'r', 'LineWidth', 2)
hold off


%% Derivative Calculation

acceleration = (w_a*U'/1e-3)';
acceleration2 = (2*p_a*U'/1e-6)';
currentd = (i_a*U'/1e-3)';

figure(4)
hold on
plot(acceleration, 'r', 'LineWidth', 2)
plot(acceleration2, 'b', 'LineWidth', 2)
hold off
figure(5)
hold on
plot(currentd, 'r', 'LineWidth', 2)
hold off


%% Reformatting Data

voltage = reshape(voltage, [], 1);
velocity = reshape(velocity, [], 1);
velocity2 = reshape(velocity2, [], 1);
current = reshape(current, [], 1);
acceleration = reshape(acceleration, [], 1);
acceleration2 = reshape(acceleration2, [], 1);
currentd = reshape(currentd, [], 1);


%% Parameters Estimation

% electrical parameters [Kphi, Ra, La]
Y = voltage;
PHI = [velocity/estimated_params.Kg, current, currentd];
LS = PHI\Y;

estimated_params.Kphi = LS(1);
estimated_params.Ra = LS(2);
estimated_params.La = LS(3);

% mechanical parameters [Be, Tc, Je]
Y = estimated_params.Kphi * estimated_params.Kg * current;
PHI = [velocity, estimated_params.Kg*sign(velocity), acceleration];
LS = PHI\Y;

estimated_params.Be = LS(1);
estimated_params.Tc = LS(2);
estimated_params.Je = LS(3);


end

