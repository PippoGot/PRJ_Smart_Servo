function estimated_params = system_id2(dataset_stair, dataset_triangle, known_params, ref_data)
%SYSTEM_ID1 Summary of this function goes here
%   Estimate Kphi, Be, Tc, Je from known Ra, La, Kg

Kg = known_params.Kg;
estimated_params.Kg = Kg;


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

% Exclude first interval (zero velocity) and reshape
velocity = reshape(velocity(:, 2:end), [], 1);
current = reshape(current(:, 2:end), [], 1);
voltage = reshape(voltage(:, 2:end), [], 1);

% Compute flux function
flux = velocity./sqrt(velocity.^2+0.002);

% Least square estimate for Ra, Kphi_0, Wo
PHI = [current, velocity/Kg, flux/Kg];
Y = voltage;

LS = PHI\Y;

estimated_params.Ra = LS(1);
Ra = estimated_params.Ra;
estimated_params.Kphi_0 = LS(2);
Kphi_0 = estimated_params.Kphi_0;
estimated_params.Wo = LS(3);
Wo = estimated_params.Wo;


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

% Compute flux function
flux = current./sqrt(velocity.^2+0.002);

% Least square estimate of Be, Tc, Je, Wo (for verification)
% PHI = [velocity, Kg*sign(velocity), acceleration, Kg*flux];
PHI = [velocity, Kg*sign(velocity), acceleration];
Y = Kg*Kphi_0*current;
LS = PHI\Y;

estimated_params.Be = LS(1);
Be = LS(1);
estimated_params.Tc = LS(2);
Tc = LS(2);
estimated_params.Je = LS(3);
Je = LS(3);
% estimated_params.Wo2 = LS(4);
% Wo2 = LS(4);

estimated_params

end

