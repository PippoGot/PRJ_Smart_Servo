%% Motor Model (from measurements and estimations)

motor.Vcc = 5;          % motor voltage supply                              [V]

% Measured Parameters
motor.Ra = 2.2;         % armature resistance                               [Ohm]
motor.La = 7e-3;        % armature inductance                               [H]

motor.Kg = 11/(61*36);  % gearbox ratio (motor to shaft)                    [#]

% Estimated Parameters
motor.Kphi = 1.8e-3;    % motor torque/BEMF constant                        [V*s]

motor.Je = 1.0624e-8;   % equivalent inertia                                [Kg*m^2]
motor.Be = 1.4949e-8;   % equivalent viscous friction                       [N*m*s]

motor.tau_c = 7.5179e-5;% coulomb friction                                  [N*m]

motor.w_sb = 0.1;       % breakaway speed                                   [rad/s]
motor.tau_sb = 2*motor.tau_c; % breakaway torque                            [N*m]

%% State Space Model (Linear part)

motor.F = [0, 1, 0;
    0, -motor.Be/motor.Je, motor.Kphi/motor.Je;
    0, -motor.Kphi/motor.La, -motor.Ra/motor.La];

motor.G = [0; 0; motor.Vcc/motor.La];

motor.h1 = [motor.Kg, 0, 0];
motor.h2 = [0, 0, 1];
motor.h3 = -motor.h2;

motor.H = [motor.h1; motor.h2; motor.h3];

motor.D = zeros(3, 1);


% matrix = [motor.F, motor.G; motor.H, motor.D];
