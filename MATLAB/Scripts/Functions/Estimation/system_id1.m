function estimated_params = system_id1(dataset_stair, dataset_triangle, known_params, ref_data)
%SYSTEM_ID1 Summary of this function goes here
%   Estimate Kphi, Be, Tc, Je from known Ra, La, Kg

Kg = known_params.Kg;
estimated_params.Kg = Kg;
Ra = known_params.Ra;
estimated_params.Ra = Ra;
La = known_params.La;
estimated_params.La = La;


%% Staircase section - Kphi, Be, Tc
periods = ref_data.stair_periods;
data_trim = ref_data.stair_trim;

% Compute raw_velocity from discrete derivation of position
raw_velocity = gradient(dataset_stair.position)/1e-3;

% Filter velocity and current usign Savitzky-Golay filter
velocity = sgolayfilt(raw_velocity, 5, 25);
current = sgolayfilt(dataset_stair.current, 5, 25);

% Divide data in intervals and remove transients
velocity = symmetric_trim(stack_periods(velocity', periods)', data_trim);
current = symmetric_trim(stack_periods(current', periods)', data_trim);
voltage = symmetric_trim(stack_periods(dataset_stair.voltage', periods)', data_trim);

% Exclude first interval (zero velocity)
velocity = velocity(:, 2:end);
current = current(:, 2:end);
voltage = voltage(:, 2:end);

% Least square test estimate for Ra, Kphi_0, Wo
flux = velocity./sqrt(velocity.^2+0.002);
PHI = [reshape(current, [], 1), reshape(velocity, [], 1), reshape(flux, [], 1)];
Y = reshape(voltage, [], 1);

LS = PHI\Y;
Ra = LS(1)
Kphi_0 = Kg*LS(2)
Wo = Kg*LS(3)

% Compute other signal mean
avg_velocity = mean(velocity);
avg_current = mean(current);
avg_voltage = mean(voltage);

% Apply formula to estimate Kphi
Kphi = Kg*(avg_voltage - Ra*avg_current) ./ avg_velocity;
estimated_params.Kphi_curve = reshape(Kphi, [], 2);
% plot(Kphi)
estimated_params.Kphi = mean(Kphi);
Kphi = estimated_params.Kphi;

% NOTICE: computing the mean and then applying the formula yields same results
% as doing the opposite, i.e. applying the formula and then computing the mean.

% NOTICE: Kphi seems to be inversely proportional to speed. Verify by modeling.

% Least square estimate of Be and Tc
PHI = [avg_velocity; Kg*sign(avg_velocity)]';
Y = Kphi*Kg*avg_current';

LS = PHI\Y;

% x = [-flip(0:1:30), 0:1:30];
% plot(avg_raw_velocity, Kphi*Kg*avg_current, ' o', x, LS(1)*x + Kg*LS(2)*sign(x), 'r')
% grid on

% estimated_params.Be = LS(1);
% Be = estimated_params.Be;
% estimated_params.Tc = LS(2);
% Tc = estimated_params.Tc;


%% Triangle section - Be, Tc, Je
periods = ref_data.triangle_periods;
data_trim = ref_data.triangle_trim;

% Compute velocity and acceleration from discrete derivation of position
% and Savitzky-Golay filtering
full_raw_velocity = gradient(dataset_triangle.position)/1e-3;
full_velocity = sgolayfilt(full_raw_velocity, 5, 251);
full_raw_acceleration = gradient(full_velocity)/1e-3;
full_acceleration = sgolayfilt(full_raw_acceleration, 5, 501);

% Divide data in intervals and remove transient
velocity = symmetric_trim(stack_periods(full_velocity', periods)', data_trim);
acceleration = symmetric_trim(stack_periods(full_acceleration', periods)', data_trim);
current = symmetric_trim(stack_periods(dataset_triangle.current', periods)', data_trim);

% Reshape data in a single column
periods = 28;

velocity = stack_periods(reshape([velocity(:, end), velocity(:, 1:end-1)], [], 1)', periods)';
velocity = reshape(velocity, [], 1);

acceleration = stack_periods(reshape([acceleration(:, end), acceleration(:, 1:end-1)], [], 1)', periods)';
acceleration = reshape(acceleration, [], 1);

current = stack_periods(reshape([current(:, end), current(:, 1:end-1)], [], 1)', periods)';
current = reshape(sgolayfilt(current, 5, 251, [], 1), [], 1);

flux = transpose(Wo/sqrt(velocity.^2+0.002));

% Least square estimate of Be, Tc and Je
PHI = [velocity, Kg*sign(velocity), acceleration];
Y = Kg*current.*flux;
LS = PHI\Y;

estimated_params.Be = LS(1);
Be = LS(1)
estimated_params.Tc = LS(2);
Tc = LS(2)
estimated_params.Je = LS(3);
Je = LS(3)

% hold on
% plot(raw_velocity, Kg*Kphi*current, ' g*', x, LS(1)*x + Kg*LS(2)*sign(x), 'k')
% legend('datapoints - stair', 'estimate - stair', 'datapoints -triangle', 'estimate - triangle')
% hold off



end

