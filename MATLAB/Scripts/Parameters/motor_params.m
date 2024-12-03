%% Motor Model (from measurements and estimations)

% Measured Parameters
motor.Ra = 2.2;         % armature resistance                               [Ohm]
motor.La = 7e-3;        % armature inductance                               [H]

motor.Kg = 11/(61*36);  % gearbox ratio (motor to shaft)                    [#]

% Estimated Parameters
motor.Kphi = 1.8e-3;    % motor torque/BEMF constant                        [V*s]

motor.Je = 1.0624e-8;   % equivalent inertia                                [Kg*m^2]
motor.Be = 1.4949e-8;   % equivalent viscous friction                       [N*m*s]

motor.tauc = 7.5179e-5; % coulomb friction                                  [N*m]


%% State Space Model

motor.A = [ 0, 1, 0;
            0, -motor.Be/motor.Je, motor.Kphi/motor.Je;
            0, -motor.Kphi/(motor.Kg*motor.La), -motor.Ra/motor.La];
motor.B = [0; 0; 1/motor.La];
motor.C = [1, 0, 0; 0, 0, 1];
motor.D = zeros(2,1);